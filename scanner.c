#include <stdio.h>
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdbool.h>

#include "scanner.h"

node_t *head;
node_t *tmp;
node_t *current;

int FSA[9][12] = {
{1, 2, 0, 1004, 1, 3, 4, 5, 6, 7, 8, -1},
{1, 1, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001, 1001},
{1002, 2, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002, 1002},
{1003, 1003, 1003, 1003, 1003, 8, 1003, 1003, 1003, 1003, 1003, 1003},
{1003, 1003, 1003, 1003, 1003, 8, 1003, 1003, 1003, 1003, 1003, 1003},
{1003, 1003, 1003, 1003, 1003, 8, 1003, 1003, 1003, 1003, 1003, 1003},
{1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 8, 1003, 1003, 1003},
{1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 8, 1003, 1003},
{1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003, 1003}
};


node_t *newNode(token value){
    node_t *result = malloc(sizeof(node_t));
    result->value = value;
    result->next = NULL;
    return result;
}

void nextNode(int index){
        current = head;
        int count=0;
        while(current!=NULL){
                if(count==index){
              		current = newNode(current->value);
			return;
                }
                count++;
                current = current->next;
        }
}

void print(){
	token test1 = {4,lineNum,1,""};
	tmp = newNode(test1);
	tmp->next = head;
	head = tmp;
	printReverse(head);
	
}

void printReverse(node_t *head){
	if(head == NULL){
	return;
	}
	
	printReverse(head->next);
	//printf("Tk:%d Line#%d Column#%d Instance:%s\n", head->value.tokenID, head->value.lineNum, head->value.columnNum, head->value.tkInstance);

}


void reverse(){
	node_t* prev = NULL;
	node_t* current = head;
	node_t* next = NULL;
	while (current != NULL) {
		next = current->next;
		current->next = prev;
		prev = current;
		current = next;
	}
	head = prev;
}


void printlist(node_t *head){
    node_t *temporary = head;

    while (temporary != NULL){
        //printf("Tk:%d Line#%d Column#%d Instance:%s\n", temporary->value.tokenID, temporary->value.lineNum, temporary->value.columnNum, temporary->value.tkInstance);
        temporary = temporary->next;
    }
    printf("\n");
}

void tokenizeString(char *word) {

    int FSA_Value = 0;
    int state = 0;
    int nextState = 0;
    int length = strlen(word);
    char tokenInstance[10] = "";
    int i;
    for (i = 0; i < length; i++) {

        FSA_Value = FSA[state][getColumn(word[i])];

        char temp;
        temp = word[i];

        if((temp != 32)&&(temp != 17)&&(temp != 4)&&(temp != 0)&&(temp != 9)){
            strncat(tokenInstance, &temp, 1);
        }else{
	}

        //printf("State:%d FSAValue:%d Char:%d i:%d length:%d\n", state, FSA_Value, word[i], i, length);

        if(((FSA[state][getColumn(word[i+1])]<999)&&(FSA[state][getColumn(word[i+1])]>-1))||(word[i+1]==0)){ // If next FSA is less than 1000 and more than -1
            nextState = FSA[state][getColumn(word[i])]; // set nextState to the next FSA (we checked to make sure the next FSA was a state)
        	//printf("NEXT IS A FINAL\n");

	}else{
		
		if(!state==0){
			nextState = state;
		}else{
			if(word[i]==32){
			nextState = state;
			}
		}
        }
if(FSA[nextState][getColumn(word[i+1])] > 999){
            FSA_Value = FSA[nextState][getColumn(word[i+1])];
            temp = word[i];

            if( FSA_Value > 999) {
                if (FSA_Value==1001) {
                    if (isReserved(tokenInstance)) {
                        token test1 = {5,lineNum,i,*tokenInstance};
                        strcpy(test1.tkInstance, tokenInstance);
                        tmp = newNode(test1);
                        tmp->next = head;
                        head = tmp;
                    } else {
                        token test1 = {1,lineNum,i,*tokenInstance};
                        strcpy(test1.tkInstance, tokenInstance);
                        tmp = newNode(test1);
                        tmp->next = head;
                        head = tmp;
                    }
                }else if(FSA_Value==1002){
                    token test1 = {2,lineNum,i,*tokenInstance};
                    strcpy(test1.tkInstance, tokenInstance);
                    tmp = newNode(test1);
                    tmp->next = head;
                    head = tmp;
                }else if(FSA_Value==1003){
                    token test1 = {3,lineNum,i,*tokenInstance};
                    strcpy(test1.tkInstance, tokenInstance);
                    tmp = newNode(test1);
                    tmp->next = head;
                    head = tmp;
                }else if(FSA_Value==1004){
                    token test1 = {4,lineNum,i,"EOF"};
                    strcpy(test1.tkInstance, tokenInstance);
                    tmp = newNode(test1);
                    tmp->next = head;
                    head = tmp;
                }else{

                }
                state=0;
                memset(tokenInstance, 0, 10);

            }
        }else if(FSA_Value >= 0){
            state = FSA_Value;
        }else if(FSA_Value < 0){
            printf("FSA is -1 error\n");
        }else{
            printf("\n\nERROR:***No FSA value matches*** Value is:%d\n", word[i]);
		exit(1);
        }

    }
    return;
}


int getColumn(int number) {

    if((number > 64 && number < 91) || (number > 96 && number < 123) ){
        return 0;
    }else if(number > 47 && number < 58){
        return 1;
    }else if(number == 32){
        return 2;
    }else if(number == 95){
        return 4;
    } else if(number == 61){
        return 5;
    } else if(number == 33){
        return 6;
    } else if(number == 58){
        return 7;
    } else if(number == 124){
        return 8;
    } else if(number == 38){
        return 9;
    } else if((number > 38 && number < 48) || (number > 96 && number < 123) || (number == 59) || (number == 60) || (number == 62) || (number == 91) || (number == 93) || (number == 94) || (number == 123) || (number == 125)){
        return 10;
    } else if (number == 0){
        return 11;
    }

    else{
        printf("ERROR: Input given does not match anything in the table. This was the value: %d\n", number);
        return -1;
    }

}


bool isReserved(char *word) {
    char reserved[][10] = {"begin","end","do","while","whole","label","return","input","output","program","warp","if","then","pick","declare","assign","func"};
    int i;
    for(i = 0; i < 17; i++){
        if(!strcmp(word, reserved[i])){
            return true;
        }
    }
    return false;
}


char* formatLine(char *line) {
    int length = strlen(line);
    int arr[30000] = {};
    char *result = (char*) arr;
    int i;
    for (i = 0; i < length; i++) {
        if(line[i] == 35){
            accepting = !accepting;
        }
        if(accepting){
            if(line[i] != 10){
                result[i] = line[i];
            }
        }
    }
    result[i] = 0;
    return result;
}

