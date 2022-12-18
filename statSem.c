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
#include <errno.h>

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
int numTemporaries = 0;
int numLabels = 0;

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

bool findGlobal(char* var){
	stack *tmp = globals;
	while (tmp != NULL){
		if(strcmp(var, tmp->value)==0){
			return true;
		}
		tmp = tmp->next;
	}
	return false;
}
char* returnWord = NULL;

char* getTempName(){
	//printf("**Entered getTempName function\n");
	numTemporaries++;
	free(returnWord);
	returnWord = (char *) malloc(sizeof(returnWord));
	strcpy(returnWord,"");
	strcat(returnWord,"T");
	char temp[5];
	sprintf(temp, "%d", numTemporaries);
	strcat(returnWord, temp);
	return returnWord;
}

char* returnWord1 = NULL;
char* getLabelName(){
	//printf("**Entered getLabelName function\n");
	numLabels++;
	free(returnWord1);
	returnWord1 = (char *) malloc(sizeof(returnWord1));
	strcpy(returnWord1,"");
	strcat(returnWord1,"L");
	char temp[5];
	sprintf(temp, "%d", numLabels);
	strcat(returnWord1, temp);
	return returnWord1;
}



void writeVariables(){
	stack *temp = globals;
	while (temp != NULL){
		writeFile(temp->value, "0");
		temp = temp->next;
	}
	int i=0;
	for(i=0; i<=numTemporaries; i++){
		char str1[100] = "T";
		char str2[5];
		sprintf(str2, "%d", i);
		strcat(str1, str2);
		writeFile(str1, "0");
	}
}

void statSem(char* filename){
	fileName2 = filename;
	assemblyFileName = strcat(fileName2, ".asm");
	
	fPointer = fopen(assemblyFileName, "w");
	fclose(fPointer);
        fPointer = fopen(assemblyFileName, "a");	

	isGlobal = true;
	traversal(parsedTree);
	writeFile("STOP", "N/A");
	writeVariables();
	//printf("No errors detected!\n");
	printf("Created file named: %s\n", assemblyFileName);
}

void writeFile(char* statement, char* arg1){
	//printf("**Entered writeFile function with values:%s AND %s\n", statement, arg1, assemblyFileName);
	if(!fPointer){
		printf("ERROR: Something went wrong when trying to write to file:%s", strerror(errno));
	}else{
		if(strcmp(arg1, "N/A")==0){
			fprintf(fPointer, "%s", statement);
		}else{
			fprintf(fPointer,"%s", statement);
			fprintf(fPointer, " ");
			fprintf(fPointer,"%s", arg1);
		}
		fprintf(fPointer, "\n");
	}
}


traversal(treenode* myNode){
	//printf("**Entering traversal function with name:%s\n", myNode->name);
	int varsCount = 0;
	int i;
	for (i = 1; i < 6; i++) {
		//printf("traversing children with i=%d\n", i);
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
		} else {
			checkNode(currentChild);
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

	//printf("**Entered checkNode function with name:%s\n", myNode->name);
	if (strcmp(myNode->name, "<assign>")==0) {
		funcAssign(myNode);
	} else if (strcmp(myNode->name, "<in>")==0) {
		funcInput(myNode);
	} else if (strcmp(myNode->name, "<R>")==0) {
		funcR(myNode);
	} else if (strcmp(myNode->name, "<label>")==0) {
		funcLabel(myNode);
	} else if (strcmp(myNode->name, "<block>")==0) {
		funcBlock(myNode);
	} else if (strcmp(myNode->name, "<expr>")==0) {
		funcExpr(myNode, false);
	} else if (strcmp(myNode->name, "<N>")==0) {
		funcN(myNode);
	} else if (strcmp(myNode->name, "<A>")==0) {
		funcA(myNode);
	} else if (strcmp(myNode->name, "<A*>")==0) {
		funcA2(myNode);
	} else if (strcmp(myNode->name, "<M>")==0) {
		funcM(myNode);
	} else if (strcmp(myNode->name, "<stats>")==0) {
		funcStats(myNode);
	} else if (strcmp(myNode->name, "<stat>")==0) {
		funcStat(myNode);
	} else if (strcmp(myNode->name, "<mstat>")==0) {
		funcMStat(myNode);
	} else if (strcmp(myNode->name, "<out>")==0) {
		funcOut(myNode);
	} else if (strcmp(myNode->name, "<if>")==0) {
		funcIf(myNode);
	} else if (strcmp(myNode->name, "<loop>")==0) {
		funcLoop(myNode);
	} else if (strcmp(myNode->name, "<RO>")==0) {
		funcRO(myNode);
	} else if (strcmp(myNode->name, "<goto>")==0) {
		funcGoto(myNode);
	} else if (strcmp(myNode->name, "")==0) {
		printf("ERROR: Found null unexpectedly\n");
		exit(0);
	}	
}


void funcR(treenode* myNode){
	//printf("**Entered R function\n");
	if (myNode->first->value.tokenID == 1) {
		int found = find(myNode->first->value.tkInstance);
		if (found == -1) {
			printf("ERROR: unknown variable found! On line:%d Instance:'%s'\n", myNode->first->value.lineNum, myNode->first->value.tkInstance);
			exit(1);
		}
		bool isGlobalVar = findGlobal(myNode->first->value.tkInstance);
		if(isGlobalVar){
			writeFile("LOAD", myNode->first->value.tkInstance);
		}else{
			char temp[5];
			sprintf(temp, "%d", found);
			writeFile("STACKR", temp);
		}
	} else if (myNode->second != NULL){
		if (strcmp(myNode->second->name, "<expr>")==0) {
			checkNode(myNode->second);
		}else{
			writeFile("LOAD", myNode->first->value.tkInstance);
		}
	}else{
		writeFile("LOAD", myNode->first->value.tkInstance);
	}

}

void funcInput(treenode* myNode){
	//printf("**Entering input function\n");
	int found = find(myNode->first->value.tkInstance);
	if (found == -1) {
		printf("ERROR: Input found unknown variable! On line:%d Instance'%s'\n", myNode->first->value.lineNum, myNode->first->value.tkInstance);
		exit(1);
	}
	bool isGlobalVar = findGlobal(myNode->first->value.tkInstance);
	if(isGlobalVar){
		writeFile("READ", myNode->first->value.tkInstance);
	}else{
		char* temp1 = getTempName();
		char temp[20];
		strcpy(temp, temp1);
		writeFile("READ", temp);
		writeFile("LOAD", temp);
		char temp2[5];
		sprintf(temp2, "%d", found);
		writeFile("STACKW", temp2);
	}

}

void funcAssign(treenode* myNode){
	int found = find(myNode->first->value.tkInstance);
	if (found == -1) {
		printf("ERROR: Assign found unknown variable! On line:%d Intance:'%s'\n", myNode->first->value.lineNum, myNode->first->value.tkInstance);
		exit(1);
	}
	bool isGlobalVar = findGlobal(myNode->first->value.tkInstance);
	checkNode(myNode->second);
	if(isGlobalVar){
		writeFile("STORE", myNode->first->value.tkInstance);
	}else{
		char temp[5];
		sprintf(temp, "%d", found);
		writeFile("STACKW", temp);
	}

}

void funcVars(treenode* myNode, int *varsCount){
	//printf("**Entered vars function!\n");
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
				writeFile("LOAD", currentNode->second->value.tkInstance);
				if(isGlobal){
					writeFile("STORE", identNode->value.tkInstance);
					stack* tmp = malloc(sizeof(stack));
					strcpy(tmp->value, identNode->value.tkInstance);
					tmp->next = globals;
					globals = tmp;
				}else{
					writeFile("PUSH", "N/A");
					writeFile("STACKW", "0");
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
	char temp[20];
	strcpy(temp, "");
	strcat(temp, myNode->first->value.tkInstance);
	strcat(temp, ":");
	writeFile(temp,"NOOP");
}

void funcBlock(treenode* myNode){
	//printf("**Entered block function\n");
	traversal(myNode);
}

void funcExpr(treenode* myNode, bool prevSubtraction){
	//printf("**Entered expr function\n");
	if(myNode->second != NULL){
		char* temp = getTempName();
		char exprTemp[20];
		strcpy(exprTemp, temp);
		funcExpr(myNode->third, true);
		writeFile("STORE", exprTemp);
		checkNode(myNode->first);
		if(prevSubtraction){
			writeFile("ADD", exprTemp);
		}else{
			writeFile("SUB", exprTemp);
		}
	}else{
		if(myNode->first==NULL){
			printf("It is NULL!!!!\n");
		}
		checkNode(myNode->first);
		
	}
}

void funcN(treenode* myNode){
	//printf("**Entered N function\n");
	if(myNode->third != NULL){
		char* temp1 = getTempName();
		char temp[20];
		strcpy(temp, temp1);
		checkNode(myNode->third);
		writeFile("STORE", temp);
		checkNode(myNode->first);
		if(strcmp(myNode->second->value.tkInstance, "+")==0){
			writeFile("ADD", temp);
		}else{
			writeFile("MULT", temp);
		}
	}else{
		checkNode(myNode->first);
	}
}

void funcA(treenode* myNode){
	//printf("**Entered A function\n");
	checkNode(myNode->first);
	char* temp = getTempName();
	char aTemp[20];
	strcpy(aTemp, temp);
	writeFile("STORE", aTemp);
	checkNode(myNode->second);
}

void funcA2(treenode* myNode){
	//printf("**Entered A2 function");
	if(myNode->first == NULL){
		return;
	}else if (strcmp(myNode->first->value.tkInstance, "/")==0){
		char* prevTemp = NULL;
		prevTemp = (char *) malloc(sizeof(prevTemp));
		strcpy(prevTemp, "");
		strcat(prevTemp, "T");
		char temp[5];
		sprintf(temp, "%d", numTemporaries);
		strcat(prevTemp, temp);
		char actualPrevTemp[20];
		strcpy(actualPrevTemp, prevTemp);
		

		checkNode(myNode->second);
		char* temp1 = getTempName();
		char myTemp[20];
		strcpy(myTemp, temp1);
		writeFile("STORE", myTemp);

		writeFile("LOAD", actualPrevTemp);
		writeFile("DIV", myTemp);

		checkNode(myNode->third);
	}else{
		printf("ERROR: A2 first child is not null and does not contain '/'\n");
		exit(1);
	}
}

void funcM(treenode* myNode){
	//printf("**Entered M function\n");
	if(strcmp(myNode->first->value.tkInstance, ":")==0){
		char* temp1 = getTempName();
		char temp[20];
		strcpy(temp, temp1);
		checkNode(myNode->second);
		writeFile("STORE", temp);
		writeFile("LOAD", "0");
		writeFile("SUB", temp);		
	}else{
		checkNode(myNode->first);
	}
}

void funcStats(treenode* myNode){
	//printf("**Entered stats function\n");
	traversal(myNode);
}

void funcStat(treenode* myNode){
	//printf("**Entered stat function\n");
	traversal(myNode);
}

void funcMStat(treenode* myNode){
	//printf("**Entered mstat function\n");
	if(myNode->first == NULL){
		return;
	}else if (strcmp(myNode->first->name,"")==0){
		return;
	}else{
		traversal(myNode);
	}
}

void funcOut(treenode* myNode){
	//printf("**Entered out function\n");
	char* temp1 = getTempName();
	char temp[20];
	strcpy(temp, temp1);
	checkNode(myNode->first);
	writeFile("STORE", temp);
	writeFile("WRITE", temp);
}

void funcIf(treenode* myNode){
	char* temp = getTempName();
	char tempName[20];
	strcpy(tempName, temp);
	
	char* temp1 = getLabelName();
	char labelName[20];
	strcpy(labelName, temp1);

	checkNode(myNode->first);
	writeFile("STORE", tempName);
	checkNode(myNode->third);
	
	if(strcmp(myNode->second->first->value.tkInstance, "[")==0){
		char* temp2 = getTempName();
		char tempName1[20];
		strcpy(tempName1, temp2);
		
		writeFile("STORE", tempName1);
		
		char* temp3 = getLabelName();
		char labelPos[20];
		strcpy(labelPos, temp3);
		
		writeFile("LOAD", tempName);
		writeFile("MULT", tempName1);
		writeFile("BRZPOS", labelPos);
		
		char labelPosPlus[20];
		strcpy(labelPosPlus, labelPos);
		strcat(labelPosPlus, ":");
		
		writeFile(labelPosPlus,"NOOP");
		writeFile("BRNEG", labelName);
	}else{
		writeFile("SUB", tempName);
	}
	checkNode(myNode->second);
	checkNode(myNode->fourth);
	
	char labelNamePlus[20];
	strcpy(labelNamePlus, labelName);
	strcat(labelNamePlus, ":");

	if(myNode->fifth != NULL){
		char* temp4 = getLabelName();
		char labelName2[20];
		strcpy(labelName2, temp4);
		
		writeFile("BR", labelName2);
		writeFile(labelNamePlus, "NOOP");
		checkNode(myNode->fifth);
		
		char labelName2Plus[20];
		strcpy(labelName2Plus, labelName2);
		strcat(labelName2Plus, ":");
		
		writeFile(labelName2Plus, "NOOP");
	}else{
		writeFile(labelNamePlus, "NOOP");
	}
}


void funcLoop(treenode* myNode){
	char* temp = getLabelName();
	char startLabel[20];
	strcpy(startLabel, temp);
	
	char startLabelPlus[20];
	strcpy(startLabelPlus, startLabel);
	strcat(startLabelPlus, ":");
	
	writeFile(startLabelPlus, "NOOP");
	
	char* temp1 = getLabelName();
	char exitLabel[20];
	strcpy(exitLabel, temp1);

	char exitLabelPlus[20];
	strcpy(exitLabelPlus, exitLabel);
	strcat(exitLabelPlus, ":");
	
	char* temp2 = getTempName();
	char tempName1[20];
	strcpy(tempName1, temp2);
	
	checkNode(myNode->first);
	writeFile("STORE", tempName1);
	checkNode(myNode->third);
	
	if(strcmp(myNode->second->first->value.tkInstance, "[")==0){
		char* temp3 = getTempName();
		char tempName2[20];
		strcpy(tempName2, temp3);
		
		writeFile("STORE", tempName2);
		
		char* temp4 = getLabelName();
		char posLabel[20];
		strcpy(posLabel, temp4);
		
		char posLabelPlus[20];
		strcpy(posLabelPlus, posLabel);
		strcat(posLabelPlus, ":");
		
		writeFile("LOAD", tempName1);
		writeFile("MULT", tempName2);
		writeFile("BRZPOS", posLabel);
		writeFile(posLabelPlus, "NOOP");
		writeFile("BRNEG", exitLabel);
	}else{
		writeFile("SUB", tempName1);
	}
	checkNode(myNode->second);
	checkNode(myNode->fourth);
	writeFile("BR", startLabel);
	writeFile(exitLabelPlus, "NOOP");

}

void funcRO(treenode* myNode){
	
	char temp1[20];
	strcpy(temp1, "");
	strcat(temp1, "L");
	char temp[5];
	sprintf(temp, "%d", numLabels);
	strcat(temp1, temp);

	if (strcmp(myNode->first->value.tkInstance,">")==0) {
		writeFile("BRZPOS", temp1);
	} else if (strcmp(myNode->first->value.tkInstance, "<")==0) {
		writeFile("BRZNEG", temp1);
	} else if (strcmp(myNode->first->value.tkInstance,"==")==0) {
		writeFile("BRPOS", temp1);
		writeFile("BRNEG", temp1);
	} else if (strcmp(myNode->first->value.tkInstance, "[")==0) {
	} else if (strcmp(myNode->first->value.tkInstance, "!=")==0) {
		writeFile("BRZERO", temp1);
	} else {
		printf("ERROR: Couldn't find the RO\n");
		exit(1);
	}

}

void funcGoto(treenode* myNode){
	//printf("Entered GOTO function\n");
	writeFile("BR", myNode->first->value.tkInstance);
}




