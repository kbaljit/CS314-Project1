/*
 *********************************************
 *  314 Principles of Programming Languages  *
 *  Spring 2017                              *
 *  Author: Ulrich Kremer & Baljit Kaur      *
 *  Student Version                          *
 *********************************************
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "InstrUtils.h"
#include "Utils.h"

void findCriticalsForStoreAI(Instruction **head, Instruction **tail);
void findCriticalsForAddSubMulDiv(Instruction **head, Instruction **tail);
void findCriticalsForLoadAI(Instruction **head, Instruction **tail);

int main()
{
	Instruction *head;

	head = ReadInstructionList(stdin);
	if (!head) {
		WARNING("No instructions\n");
		exit(EXIT_FAILURE);
	}

	/* Baljit Kaur */
	
	Instruction *temp = head;
	Instruction *instr = head;

	while(instr != NULL){
		if(instr->opcode == OUTPUTAI){
			instr->critical = '1';
		}else{
			instr->critical = '0';
		}
		instr = instr->next;
	}

	head->critical = '1';
	
	Instruction *tail = LastInstruction(temp);
	//char found = '0';
	while(tail != head){
		if(tail->critical == '1'){
			if(tail->opcode == OUTPUTAI){
				temp = tail->prev;
				while(temp != head){
					if(temp->opcode == STOREAI){
						if((tail->field1 == temp->field2) && (tail->field2 == temp->field3)){
							temp->critical = '1';
							Instruction **headPtr = &head;
							Instruction **tailPtr = &tail;
							Instruction *t = tail;
							findCriticalsForStoreAI(headPtr, tailPtr);
							tail = t->prev;
							temp = tail->next;
						}
					}
					temp = temp->prev;
				}
			}
		}
		tail = tail->prev;
	}
	
	for (instr = head; instr != NULL; instr = instr->next){
		if(instr->critical == '0'){
			if(instr->prev == NULL && instr->next == NULL){
				free(head);
				head = NULL;
			}else if(instr->prev == NULL){
				Instruction *tmp = instr;
				head = tmp->next;
				free(tmp);
				head->prev = NULL;
			}else if(instr->next == NULL){
				Instruction *tmp = instr;
				free(tmp);
				instr->prev->next = NULL;
			}else{
				Instruction *tmp = instr;
				Instruction *prev = tmp->prev;
				Instruction *next = tmp->next;
				prev->next = next;
				next->prev = prev;
				free(tmp);
			}
		}
	}
	
	if (head)
		PrintInstructionList(stdout, head);
	
	//free list
	for (instr = head; instr != NULL; instr = instr->next){
		if(instr->prev == NULL && instr->next == NULL){
			free(head);
			head = NULL;
		}else if(instr->prev == NULL){
			Instruction *tmp = instr;
			head = tmp->next;
			free(tmp);
			head->prev = NULL;
		}else if(instr->next == NULL){
			Instruction *tmp = instr;
			free(tmp);
			instr->prev->next = NULL;
		}
	}
	
	head = NULL;
	
	return EXIT_SUCCESS;
}

void findCriticalsForStoreAI(Instruction **head, Instruction **tail){
	char found1 = '0';
	while(*tail != *head && found1 == '0'){
		if((*tail)->critical == '1'){
			if((*tail)->opcode == STOREAI){
				Instruction *temp = (*tail)->prev;
				while(temp != *head && found1 == '0'){
					if(temp->opcode == LOADAI){
						if((*tail)->field1 == temp->field3){
							temp->critical = '1';
							found1 = '1';
							return;
						}
					}else if(temp->opcode == SUB || temp->opcode == ADD || temp->opcode == MUL || temp->opcode == DIV){
						if((*tail)->field1 == temp->field3){
							temp->critical = '1';
							*tail = temp;
							findCriticalsForAddSubMulDiv(head, tail);
							found1 = '1';
							return;
						}
					}else if(temp->opcode == LOADI){
						if((*tail)->field1 == temp->field2){
							temp->critical = '1';
							found1 = '1';
							return;
						}
					}
					temp = temp->prev;
				}
			}
		}
		*tail = (*tail)->prev;
	}
}

void findCriticalsForAddSubMulDiv(Instruction **head, Instruction **tail){
	char found2 = '0';
	char found3 = '0';
	Instruction *t = *tail;
	while(*tail != *head || found3 != '1'){
		if(*tail == NULL){return;}
		if((*tail)->critical == '1'){
			if((*tail)->opcode == SUB || (*tail)->opcode == ADD || (*tail)->opcode == MUL || (*tail)->opcode == DIV){
				Instruction *temp1 = (*tail)->prev;
				Instruction *temp2 = (*tail)->prev;
				while(temp1 != *head && found2 == '0'){
					if(temp1->opcode == LOADAI){
						if((*tail)->field1 == temp1->field3){
							temp1->critical = '1';
							if(temp1->prev != *head){
								*tail = temp1;
								findCriticalsForLoadAI(head, tail);
							}
							found2 = '1';
							break;
						}
					}else if(temp1->opcode == SUB || temp1->opcode == ADD || temp1->opcode == MUL || temp1->opcode == DIV){
						if((*tail)->field1 == temp1->field3){
							temp1->critical = '1';
							if(temp1 == (*head)->next){
								return;
							}
							if(temp1->prev != *head){
								*tail = temp1;
								findCriticalsForAddSubMulDiv(head, tail);
							}
							found2 = '1';
							break;
						}
					}else if(temp1->opcode == LOADI){
						if(*tail == NULL){return;}
						if((*tail)->field1 == temp1->field2){
							temp1->critical = '1';
							found2 = '1';
							break;
						}
					}	
					temp1 = temp1->prev;
				}
				*tail = t;
				while(temp2 != *head && found3 == '0'){
					if(temp2->opcode == LOADAI){
						if((*tail)->field2 == temp2->field3){
							temp2->critical = '1';
							if(temp2->prev != *head){
								*tail = temp2;
								findCriticalsForLoadAI(head, tail);
							}
							found3 = '1';
							break;
						}
					}else if(temp2->opcode == SUB || temp2->opcode == ADD || temp2->opcode == MUL || temp2->opcode == DIV){
						if((*tail)->field2 == temp2->field3){
							temp2->critical = '1';
							if(temp2 == (*head)->next){
								return;
							}
							if(temp2->prev != *head){
								*tail = temp2;
								findCriticalsForAddSubMulDiv(head, tail);
							}
							found3 = '1';
							break;
						}
					}else if(temp2->opcode == LOADI){
						if(*tail == NULL){return;}
						if((*tail)->field2 == temp2->field2){
							temp2->critical = '1';
							found3 = '1';
							break;
						}
					}	
					temp2 = temp2->prev;
				}
				
			}
		}
		if(found2 == '1' && found3 == '1'){return;}
		*tail = (*tail)->prev;
	}
	return;
}

void findCriticalsForLoadAI(Instruction **head, Instruction **tail){
	char found4 = '0';
	while(*tail != *head && found4 == '0'){
		if((*tail)->critical == '1'){
			if((*tail)->opcode == LOADAI){
				Instruction *temp = (*tail)->prev;
				while(temp != *head && found4 == '0'){
					if(temp->opcode == STOREAI){
						if((*tail)->field1 == temp->field2 && (*tail)->field2 == temp->field3){
							temp->critical = '1';
							*tail = temp;
							findCriticalsForStoreAI(head, tail);
							found4 = '1';
							return;
						}
					}
					temp = temp->prev;
				}
			}
		}
		*tail = (*tail)->prev;
	}
}

