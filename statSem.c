#include <stdio.h>
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdbool.h>
#include <stddef.h>

#include "scanner.h"
#include "parser.h"
#include "statSem.h"

typedef struct stack{
    char* value;
    struct stack *next;
}stack;

stack* stack1 = NULL;
stack* globals = NULL;
stack* testing = NULL;

FILE * fPointer;
char* fileName2;
char* assemblyFileName;
bool isGlobal;

void push(char* value){
	stack *newnode = malloc(sizeof(stack));
	if (newnode == NULL) printf("ERROR: newnode is NULL");
    
	newnode->value = value;
	newnode->next = stack1;
	stack1 = newnode;
	return;
}


void pop(){
	if(stack1 == NULL) printf("ERROR: stack1 is empty");

	char* result = stack1->value;
	stack *tmp = stack1;
	stack1 = stack1->next;
	free(tmp);
	printf("%s\n", result);
}


int find(char* word){
	if(!stack1){
		if(!globals){
		}else{
			stack* tmp1 = malloc(sizeof(stack));
			strcpy(tmp1->value, globals->value);
			tmp1->next = globals->next;
			while (tmp1 != NULL){
				if (strcmp(tmp1->value, word)==0){
					return 1000;
				}else{
					if(tmp1->next != NULL){
						tmp1=tmp1->next;
					}
				}
			}
		}
		return -1;
	}else{
	}

	stack* tmp = stack1;
	int level = 0;
	if(tmp == NULL){
		return -1;
	}
	while (tmp != NULL){
		if (strcmp(tmp->value, word)==0){
			return level;
		}else{
			level++;
			if(tmp->next != NULL){
				tmp=tmp->next;
			}else{
				if(!globals){
        		        }else{
                        		stack* tmp1 = malloc(sizeof(stack));
                        		strcpy(tmp1->value, globals->value);
                        		tmp1->next = globals->next;
                        		while (tmp1 != NULL){
                        		        if (strcmp(tmp1->value, word)==0){
                        		                return 1000;
                        		        }else{
                        		                if(tmp1->next != NULL){
                        		                        tmp1=tmp1->next;
                        		                }
                        		        }
					}
				}
				return -1;
			}
		}
	}
}




void statSem(char* filename){
	fileName2 = filename;
	assemblyFileName = strcat(fileName2, ".asm");
	printf("This is the filename:%s\n", assemblyFileName);	
	
	fPointer = fopen(assemblyFileName, "w");
	fprintf(fPointer, "This is a test\nnewline\n");
	fclose(fPointer);
	
	writeFile("Test write", "arg1");
	writeFile("This is another Test write without an argument", "N/A");
	
	isGlobal = true;
	traversal(parsedTree);
	printf("No errors detected!\n");
}

void writeFile(char* statement, char* arg1){
	fPointer = fopen(assemblyFileName, "a");
	char* completeStatement;
	strcpy(completeStatement, "");
	
	if(strcmp(arg1, "N/A")==0){
		completeStatement = strcat(completeStatement, statement);
	}else{
		completeStatement = strcat(completeStatement, statement);
		completeStatement = strcat(completeStatement, " ");
		completeStatement = strcat(completeStatement, arg1);
	}
	completeStatement = strcat(completeStatement, "\n");
	fprintf(fPointer, completeStatement);
	fclose(fPointer);
}


traversal(treenode* myNode){
	int varsCount = 0;
	int i;
	for (i = 1; i < 6; i++) {
		treenode* currentChild;
		if (i == 1) {
			if(myNode->first==NULL){
				continue;
			}
			currentChild = myNode->first;
		} else if (i == 2) {
			if(myNode->second==NULL){
				continue;
			}
			currentChild = myNode->second;
		} else if (i == 3) {
			if(myNode->third==NULL){
				continue;
			}
			currentChild = myNode->third;
		} else if (i == 4) {
			if(myNode->fourth==NULL){
				continue;
			}
			currentChild = myNode->fourth;
		} else if(i==5){
			if(myNode->fifth==NULL){
				continue;
			}
			currentChild = myNode->fifth;
		}

		if(currentChild == NULL){
			continue;
		}
		if (strcmp(currentChild->name, "<vars>")==0) {
			funcVars(currentChild, &varsCount);
		} else if (strcmp(currentChild->name, "<assign>")==0) {
			funcAssign(currentChild);
		} else if (strcmp(currentChild->name, "<in>")==0) {
			funcInput(currentChild);
		} else if (strcmp(currentChild->name, "<R>")==0) {
			funcR(currentChild);
		} else if (strcmp(currentChild->name, "")!=0) {
			traversal(currentChild);
		}
	}
	if (varsCount != 0) {
		while (varsCount > 0) {
			pop();
			varsCount--;
		}
	}	
}


void checkNode(treenode* myNode){
	
	if (strcmp(myNode->name, "<assign>")==0) {
		processAssign(myNode);
	} else if (strcmp(myNode->name, "<in>")==0) {
		processInput(myNode);
	} else if (strcmp(myNode->name, "<R>")==0) {
		processR(myNode);
	} else if (strcmp(myNode->name, "<label>")==0) {
		processLabel(myNode);
	} else if (strcmp(myNode->name, "<label>")==0) {
		processBlock(myNode);
	} else if (strcmp(myNode->name, "<expr>")==0) {
		processExpr(myNode);
	} else if (strcmp(myNode->name, "<N>")==0) {
		processN(myNode);
	} else if (strcmp(myNode->name, "<A>")==0) {
		processA(myNode);
	} else if (strcmp(myNode->name, "<A_>")==0) {
		processA2(myNode);
	} else if (strcmp(myNode->name, "<M>")==0) {
		processM(myNode);
	} else if (strcmp(myNode->name, "<stats>")==0) {
		processStats(myNode);
	} else if (strcmp(myNode->name, "<stat>")==0) {
		processStat(myNode);
	} else if (strcmp(myNode->name, "<mStat>")==0) {
		processMStat(myNode);
	} else if (strcmp(myNode->name, "<out>")==0) {
		processOut(myNode);
	} else if (strcmp(myNode->name, "<if>")==0) {
		processIf(myNode);
	} else if (strcmp(myNode->name, "<loop>")==0) {
		processLoop(myNode);
	} else if (strcmp(myNode->name, "<RO>")==0) {
		processRO(myNode);
	} else if (strcmp(myNode->name, "<goto>")==0) {
		processGoto(myNode);
	} else if (strcmp(myNode->name, "")==0) {
		printf("ERROR: Found null unexpectedly\n")
		exit(0);
	}	
}


void funcR(treenode* myNode){
	if (myNode->first->value.tokenID == 1) {
		int found = find(myNode->first->value.tkInstance);
		if (found == -1) {
			printf("ERROR: unknown variable found! On line:%d Instance:'%s'\n", myNode->first->value.lineNum, myNode->first->value.tkInstance);
			exit(1);
		}
	} else if (strcmp(myNode->first->value.tkInstance, "<expr>")==0) {
		traversal(myNode->first);
	}

}

void funcInput(treenode* myNode){
	int found = find(myNode->first->value.tkInstance);
	if (found == -1) {
		printf("ERROR: Input found unknown variable! On line:%d Instance'%s'\n", myNode->first->value.lineNum, myNode->first->value.tkInstance);
		exit(1);
	}

}

void funcAssign(treenode* myNode){
	int found = find(myNode->first->value.tkInstance);
	if (found == -1) {
		printf("ERROR: Assign found unknown variable! On line:%d Intance:'%s'\n", myNode->first->value.lineNum, myNode->first->value.tkInstance);
		exit(1);
	}
	traversal(myNode->second);

}

void funcVars(treenode* myNode, int *varsCount){
	treenode* currentNode = myNode;
	treenode* identNode;
	while(true) {
		if (strcmp(currentNode->name, "<vars>")==0) {
			if(currentNode->first == NULL) break;
			identNode = currentNode->first;
		} else {
			printf("Unexpected Error\n");
		}
		if (strcmp(identNode->value.tkInstance, "")==0) {
			break;
		} else {
			int found = find(identNode->value.tkInstance);
			if (found != -1) {
				printf("ERROR: Duplicate variable name! On Line:%d Instance:'%s'\n", identNode->value.lineNum, identNode->value.tkInstance);
				exit(1);
			} else {
				if(isGlobal){
					stack* tmp = malloc(sizeof(stack));
					strcpy(tmp->value, identNode->value.tkInstance);
					tmp->next = globals;
					globals = tmp;
				}else{
					*varsCount = *varsCount+1;			
					push(identNode->value.tkInstance);
				}
				currentNode = currentNode->third;
			}
		}
	}
	if(isGlobal){
		isGlobal = false;
	}

}

void funcLabel(treenode* myNode){

}

void funcBlock(treenode* myNode){

}

void funcExpr(treenode* myNode, bool){

}

void funcN(treenode* myNode){

}

void funcA(treenode* myNode){

}

void funcA2(treenode* myNode){

}

void funcM(treenode* myNode){

}

void funcStats(treenode* myNode){

}

void funcStat(treenode* myNode){

}

void funcMStat(treenode* myNode){

}

void funcOut(treenode* myNode){

}

void funcIf(treenode* myNode){

}


void funcLoop(treenode* myNode){

}

void funcRO(treenode* myNode){

}

void funcGoto(treenode* myNode){

}




