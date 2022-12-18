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
#include "parser.h"


token currentToken = {-1,-1,-1,"Beginning"};
token nullToken = {0,0,0,""};
int tkIndex = 0;
bool lookedAhead = false;
treenode *parseTree;
treenode *parsedTree;

//Sets the global token 'currentToken' to the next token
void nextToken(){
	if(lookedAhead==false){ //If we are already looked ahead there is no need to get the next token
		nextNode(tkIndex); //calls function to retrieve the next node from linked list
		tkIndex++;
		currentToken.tokenID=current->value.tokenID;//setting global token to the new values
		currentToken.lineNum=current->value.lineNum;
		currentToken.columnNum=current->value.columnNum;
		strcpy(currentToken.tkInstance,current->value.tkInstance);
	}else{
		lookedAhead=false;
	}
}

//Inserts given token into given node
treenode* insertNode(treenode* node, token* tk){
    token temp = {0,0,0,""};
    temp.tokenID=tk->tokenID;
    temp.lineNum=tk->lineNum;
    temp.columnNum=tk->columnNum;
    strcpy(temp.tkInstance, tk->tkInstance);
    
	//if the first is empty then create
	if(node->first == NULL){
		node->first = createnode(temp, "tk");
	}
	else if(node->second == NULL){
		node->second = createnode(temp, "tk");
	}
	else if(node->third == NULL){
		node->third = createnode(temp, "tk");
	}
	else if(node->fourth == NULL){
		node->fourth = createnode(temp, "tk");
	}
	else{
		perror("ERROR:all of the child nodes are full");
		exit(1);
	}
	return node; 
} 

//Creates treenode with given token and word
treenode *createnode(token value, char* word){//The word will be enclosed in <> or be 'tk' indicating that the contents are a token
    treenode* result = malloc(sizeof(treenode));
    if (result != NULL) {
        result->value = value;
        strcpy(result->name, word);
        result->first = NULL;
        result->second = NULL;
        result->third = NULL;
        result->fourth = NULL;
    }
    return result;
}

//This function will join two nodes together
void joinNodes(treenode* root, treenode* leaf){
    if (root == NULL){
		printf("ERROR: Expected a root to add leaf to but found NULL");
		exit(1);
	}
	if(leaf == NULL){
		//we have nothing to add
		return;
	}
	//add the tree as a node
	if(root->first == NULL){
		root->first = leaf;
	}else if(root->second == NULL){
		root->second = leaf;
	}else if(root->third == NULL){
		root->third = leaf;
	}else if(root->fourth == NULL){
		root->fourth = leaf;
	}else if(root->fifth == NULL){
		root->fifth = leaf;
	}else{
        perror("ERROR: All of the child nodes are full and we cannot add a subtree.");
        exit(1);
    }
}


//prints tabs when printing tree
void printtabs(int numtabs){
	int i;
	for(i=0; i<numtabs; i++){
		printf("\t");
	}
}

//prints tree
void printtree_rec(treenode *root, int level){
    if(root == NULL){
        printtabs(level);
        printf("---<empty>---\n");
        return;
    }
    printtabs(level);
    printf("***value = %d instance = %s name: %s\n", root->value.tokenID, root->value.tkInstance, root->name);
    
    if(root->first != NULL){
        printtabs(level);
        printf("first\n");
        printtree_rec(root->first, level+1);
    }
    
    if(root->second != NULL){
        printtabs(level);
        printf("second\n");
        printtree_rec(root->second, level+1);
    }
    
    if(root->third != NULL){
        printtabs(level);
        printf("third\n");
        printtree_rec(root->third, level+1);
    }
    
    
    if(root->fourth != NULL){
        printtabs(level);
        printf("fourth\n");
        printtree_rec(root->fourth, level+1);
    }

	if(root->fifth != NULL){
		printtabs(level);
		printf("fifth\n");
		printtree_rec(root->fifth, level+1);
	}
    
    printtabs(level);
    printf("done\n");
    
}

void printtree(treenode* root){
    printtree_rec(root, 0);
}

//This function is called from scanner.c
void parser(){
    program();
}

//BNF: <program> -> <vars> program <block>
void program(){
    parsedTree = createnode(nullToken, "<program>");
    joinNodes(parsedTree, vars());
    nextToken();
    if(strcmp (currentToken.tkInstance, "program")==0){
        //leave out program
    }else{
        printf("ERROR: Expected 'program' reserve word token at line number:%d column number:%d but got tokenID:%d Instance:%s\n", currentToken.lineNum, currentToken.columnNum, currentToken.tokenID, currentToken.tkInstance);
	exit(1); 
   }
    joinNodes(parsedTree, block());
    //printtree(parsedTree);//Prints tree after the above functions have executed
}

//BNF: <block> -> begin <vars> <stats> end
treenode* block(){
    treenode *tree = createnode(nullToken, "<block>");
    nextToken();
    if(strcmp (currentToken.tkInstance, "begin")==0){
        //leave out begin
        joinNodes(tree, vars());
        joinNodes(tree, stats());
        nextToken();
        if(strcmp (currentToken.tkInstance, "end")==0){
            //leave out end
        }else{
            printf("ERROR: Expected 'end' reserve word token at line number:%d column number:%d but got tokenID:%d Instance:%s\n", currentToken.lineNum, currentToken.columnNum, currentToken.tokenID, currentToken.tkInstance);
            exit(1);
        }
    }else{
        printf("ERROR: Expected 'begin' reserve word token at line number:%d column number:%d but got tokenID:%d Instance:%s\n", currentToken.lineNum, currentToken.columnNum, currentToken.tokenID, currentToken.tkInstance);
        exit(1);
    }
    return tree;
}

//BNF: <expr> -> <N> - <expr> | <N>
treenode* expr(){
    treenode *tree = createnode(nullToken, "<expr>");
    joinNodes(tree, N());
    nextToken();
    if(strcmp (currentToken.tkInstance, "-")==0){
        tree = insertNode(tree,&currentToken);
        joinNodes(tree, expr());
    }else{
        lookedAhead = true; //We looked ahead to check for '-'
    }
    return tree;
}

//BNF: <N> -> <A> + <N> | <A> * <N> | <A>
treenode* N(){
    treenode *tree = createnode(nullToken, "<N>");
    joinNodes(tree, A());
    nextToken();
    if((strcmp (currentToken.tkInstance, "+")==0)||(strcmp (currentToken.tkInstance, "*")==0)){
        tree = insertNode(tree,&currentToken);
        joinNodes(tree, N());
    }else{
        lookedAhead = true; //We looked ahead to check for '+' or '*'
    }
    return tree;
}


//*ADJUSTED TO REMOVE LEFT RECURSION* BNF: <A> -> <M> <A*>
treenode* A(){
    treenode *tree = createnode(nullToken, "<A>");
    joinNodes(tree, M());
    joinNodes(tree, A_());
    return tree;
}

//*CREATED TO REMOVE LEFT RECUSRION* BNF: / <M> <A> | NULL
treenode* A_(){
    treenode *tree = createnode(nullToken, "<A*>");
    nextToken();
    if(strcmp (currentToken.tkInstance, "/")==0){
	tree = insertNode(tree,&currentToken);
        joinNodes(tree, M());
        joinNodes(tree, A_());
    }else{
        lookedAhead = true; //We looked ahead to check for '/'
    }
    return tree;
}

//BNF: <M> -> : <M> | <R>
treenode* M(){
    treenode *tree = createnode(nullToken, "<M>");
    nextToken();
    if(strcmp (currentToken.tkInstance, ":")==0){
        tree = insertNode(tree,&currentToken);
        joinNodes(tree, M());
    }else{
        lookedAhead = true;
        joinNodes(tree, R());
    }
    return tree;
}

//BNF: <R> -> ( <expr> ) | Identifier | Integer
treenode* R(){
    treenode *tree = createnode(nullToken, "<R>");
    nextToken();
    if(strcmp (currentToken.tkInstance, "(")==0){
        tree = insertNode(tree,&currentToken);
        joinNodes(tree, expr());
        nextToken();
        if(strcmp (currentToken.tkInstance, ")")==0){
            tree = insertNode(tree,&currentToken);
        }else{
            printf("ERROR: Expected ')' reserve word token at line number:%d column number:%d but got tokenID:%d Instance:%s\n", currentToken.lineNum, currentToken.columnNum, currentToken.tokenID, currentToken.tkInstance);
        	exit(1);
	}
    }else if((currentToken.tokenID == 1)||(currentToken.tokenID == 2)){
        tree = insertNode(tree,&currentToken);
    }else{
        printf("ERROR: Expected '(' reserve word token, numtk, or idtk at line number:%d column number:%d but got tokenID:%d Instance:%s\n", currentToken.lineNum, currentToken.columnNum, currentToken.tokenID, currentToken.tkInstance);
	exit(1);
    }
    return tree;
}

//BNF: <vars> -> empty | whole Identifier := Integer ; <vars> 
treenode* vars(){
    nextToken();
    treenode *tree = createnode(nullToken, "<vars>");
    if(strcmp (currentToken.tkInstance, "whole")==0){
        //Leave out whole
        nextToken();
        if(currentToken.tokenID == 1){
            tree = insertNode(tree,&currentToken);
            nextToken();
            if(strcmp (currentToken.tkInstance, ":=")==0){
                //Leave out assign tk
                nextToken();
                if(currentToken.tokenID == 2){
                    tree = insertNode(tree,&currentToken);
                    nextToken();
                    if(strcmp (currentToken.tkInstance, ";")==0){
                        //leave out ;
                        joinNodes(tree,vars());
                    }else{
                        printf("ERROR: Expected ';' at line number:%d column number:%d but got tokenID:%d Instance:%s\n", currentToken.lineNum, currentToken.columnNum, currentToken.tokenID, currentToken.tkInstance);
			exit(1);
                    }
                }else{
                    printf("ERROR: Expected Number token at line number:%d column number:%d but got tokenID:%d Instance:%s\n", currentToken.lineNum, currentToken.columnNum, currentToken.tokenID, currentToken.tkInstance);
			exit(1);
                }
            }else{
                printf("ERROR: Expected ':=' at line number:%d column number:%d but got tokenID:%d Instance:%s\n", currentToken.lineNum, currentToken.columnNum, currentToken.tokenID, currentToken.tkInstance);
		exit(1);
            }
        }else{
            printf("ERROR: Expected ID token at line number:%d column number:%d but got tokenID:%d Instance:%s\n", currentToken.lineNum, currentToken.columnNum, currentToken.tokenID, currentToken.tkInstance);
		exit(1);
        }
    }else{
        lookedAhead = true; //We looked ahead for 'whole' keyword
    }
    return tree;
}

//BNF: <stats> -> <stat>  <mStat>
treenode* stats(){
    treenode *tree = createnode(nullToken, "<stats>");
    joinNodes(tree, stat_());
    joinNodes(tree, mstat());
    return tree;
}

//BNF: <mStat> -> empty | <stat> <mStat>
treenode* mstat(){
    nextToken();
    treenode *tree = createnode(nullToken, "<mstat>");
    if((strcmp(currentToken.tkInstance,"input")==0)||(strcmp(currentToken.tkInstance,"output")==0)||(strcmp(currentToken.tkInstance,"begin")==0)||(strcmp(currentToken.tkInstance,"if")==0)||(strcmp(currentToken.tkInstance,"while")==0)||(strcmp(currentToken.tkInstance,"assign")==0)||(strcmp(currentToken.tkInstance,"warp")==0)||(strcmp(currentToken.tkInstance,"label")==0)){
        lookedAhead=true;
        joinNodes(tree, stat_());
        joinNodes(tree, mstat());
    }else{
        lookedAhead=true; //We looked ahead to check for any reserve words that follow calling the stat function
    }
    return tree;
}

//BNF: <stat> -> <in> ; | <out> ; | <block> | <if> ; | <loop> ; | <assign> ; | <goto> ; | <label> ;
treenode* stat_(){
    nextToken();
    lookedAhead = true;
    treenode *tree = createnode(nullToken, "<stat>");
    if(strcmp(currentToken.tkInstance,"input")==0){
        joinNodes(tree, in());
    }else if(strcmp(currentToken.tkInstance,"output")==0){
        //printf("Values when out() called-> line number:%d column number:%d but got tokenID:%d Instance:%s\n", currentToken.lineNum, currentToken.columnNum, currentToken.tokenID, currentToken.tkInstance);
        joinNodes(tree, out());
    }else if(strcmp(currentToken.tkInstance,"begin")==0){
        joinNodes(tree, block());
	return tree;//Return early for this one because this is the only one that doesnt need to be followed by a ';' which we check bellow 
    }else if(strcmp(currentToken.tkInstance,"if")==0){
        joinNodes(tree, if_());
    }else if(strcmp(currentToken.tkInstance,"while")==0){
        joinNodes(tree, loop());
    }else if(strcmp(currentToken.tkInstance,"assign")==0){
        joinNodes(tree, assign());
    }else if(strcmp(currentToken.tkInstance,"warp")==0){
        joinNodes(tree, goto_());
    }else if(strcmp(currentToken.tkInstance,"label")==0){
        joinNodes(tree, label());
    }else{
        printf("ERROR: could not find any options for 'stat' function. tokenID:%d tkInstance: %s\n", currentToken.tokenID, currentToken.tkInstance);
	exit(1);
    }
    nextToken();
    if(strcmp(currentToken.tkInstance,";")!=0){
        printf("ERROR: Expected ';' token at line number:%d column number:%d but got tokenID:%d Instance:%s\n", currentToken.lineNum, currentToken.columnNum, currentToken.tokenID, currentToken.tkInstance);
	exit(1);
    }
    return tree;
}

//BNF: <in> -> input Identifier  
treenode* in(){
    treenode *tree = createnode(nullToken, "<in>");
    nextToken();
    if(strcmp(currentToken.tkInstance,"input")==0){
        //leave out input
        nextToken();
        if(currentToken.tokenID==1){
            tree = insertNode(tree,&currentToken);
        }else{
            printf("ERROR: Expected Identifier token after 'input' reserve word token at line number:%d column number:%d but got tokenID:%d Instance:%s\n", currentToken.lineNum, currentToken.columnNum, currentToken.tokenID, currentToken.tkInstance);
	exit(1);
        }
    }else{
        printf("ERROR: Expected 'input' reserve word token at line number:%d column number:%d but got tokenID:%d Instance:%s\n", currentToken.lineNum, currentToken.columnNum, currentToken.tokenID, currentToken.tkInstance);
	exit(1);
    }
    return tree;
}

//BNF: <out> -> output <expr>
treenode* out(){
    treenode *tree = createnode(nullToken, "<out>");
    nextToken();
    if(strcmp(currentToken.tkInstance,"output")==0){
        //leave out output
    }else{
        printf("ERROR: Expected 'output' reserve word token at line number:%d column number:%d but got tokenID:%d Instance:%s\n", currentToken.lineNum, currentToken.columnNum, currentToken.tokenID, currentToken.tkInstance);
	exit(1);
    }
    joinNodes(tree, expr());
    return tree;
}

//BNF: <if> -> if [ <expr> <RO> <expr> ] then <stat>
//           | if [ <expr> <RO> <expr> ] then <stat> pick <stat>
treenode* if_(){
    treenode *tree = createnode(nullToken, "<if>");
    nextToken();
    //leave out if (no need to check for if because if it got here then there is an if token)
	nextToken();
	if(strcmp(currentToken.tkInstance,"[")==0){
	    //leave out [
		joinNodes(tree,expr());
		joinNodes(tree,RO());
		joinNodes(tree,expr());
		nextToken();
		if(strcmp(currentToken.tkInstance,"]")==0){
			//leave out ]
			nextToken();
			if(strcmp(currentToken.tkInstance,"then")==0){
			    //leave out then
				joinNodes(tree,stat_());
			}else{
         		       printf("ERROR: Expected 'then' reserve word token at line number:%d column number:%d but got tokenID:%d Instance:%s\n", currentToken.lineNum, currentToken.columnNum, currentToken.tokenID, currentToken.tkInstance);
				exit(1);
			}
		}else{
	            printf("ERROR: Expected ']' reserve word token at line number:%d column number:%d but got tokenID:%d Instance:%s\n", currentToken.lineNum, currentToken.columnNum, currentToken.tokenID, currentToken.tkInstance);
			exit(1);
		}
	}else{
	        printf("ERROR: Expected '[' reserve word token at line number:%d column number:%d but got tokenID:%d Instance:%s\n", currentToken.lineNum, currentToken.columnNum, currentToken.tokenID, currentToken.tkInstance);
		exit(1);
	}
	
    nextToken();
    if(strcmp(currentToken.tkInstance,"pick")==0){
        //leave out pick
        joinNodes(tree,stat_());
    }else{
        lookedAhead = true; //We looked ahead for 'pick' here
    }
    return tree;
}

//BNF: <loop> -> while [ <expr> <RO> <expr> ] <stat>
treenode* loop(){
    treenode *tree = createnode(nullToken, "<loop>");
    nextToken();
    //Leave out while keyword
    nextToken();
    if(strcmp(currentToken.tkInstance,"[")==0){
        //Leave out [
        joinNodes(tree,expr());
        joinNodes(tree,RO());
        joinNodes(tree,expr());
        nextToken();
        if(strcmp(currentToken.tkInstance,"]")==0){
            //Leave out ]
            joinNodes(tree,stat_());
        }else{
            printf("ERROR: Expected ']' reserve word token at line number:%d column number:%d but got tokenID:%d Instance:%s\n", currentToken.lineNum, currentToken.columnNum, currentToken.tokenID, currentToken.tkInstance);
		exit(1);
        }
    }else{
        printf("ERROR: Expected '[' reserve word token at line number:%d column number:%d but got tokenID:%d Instance:%s\n", currentToken.lineNum, currentToken.columnNum, currentToken.tokenID, currentToken.tkInstance);
	exit(1);
    }
    return tree;
}

//BNF: assign> -> assign Identifier = <expr>  
treenode* assign(){
    treenode *tree = createnode(nullToken, "<assign>");
    nextToken();
    //leave out assign
    nextToken();
    if(currentToken.tokenID==1){
        tree = insertNode(tree,&currentToken);
        nextToken();
        if(strcmp(currentToken.tkInstance,"=")==0){
            //leave out =
            joinNodes(tree, expr());
        }else{
            printf("ERROR: Expected '=' token at line number:%d column number:%d but got tokenID:%d Instance:%s\n", currentToken.lineNum, currentToken.columnNum, currentToken.tokenID, currentToken.tkInstance);
		exit(1);
        }
    }else{
        printf("ERROR: Expected identifier token at line number:%d column number:%d but got tokenID:%d Instance:%s\n", currentToken.lineNum, currentToken.columnNum, currentToken.tokenID, currentToken.tkInstance);
	exit(1);
    }
    return tree;
}

//BNF: <RO>  ->  > | < | == | [ = ](three tokens) | !=
treenode* RO(){
    treenode *tree = createnode(nullToken, "<RO>");
    nextToken();
    if(strcmp(currentToken.tkInstance,">")==0){
        tree = insertNode(tree,&currentToken);
    }else if(strcmp(currentToken.tkInstance,"<")==0){
        tree = insertNode(tree,&currentToken);
    }else if(strcmp(currentToken.tkInstance,"==")==0){
        tree = insertNode(tree,&currentToken);
    }else if(strcmp(currentToken.tkInstance,"!=")==0){
        tree = insertNode(tree,&currentToken);
    }else if(strcmp(currentToken.tkInstance,"[")==0){
        tree = insertNode(tree,&currentToken);
        nextToken();
        if(strcmp(currentToken.tkInstance,"=")==0){
            tree = insertNode(tree,&currentToken);
            nextToken();
            if(strcmp(currentToken.tkInstance,"]")==0){
                tree = insertNode(tree,&currentToken);
            }else{
                printf("ERROR: Expected ']' at line number:%d column number:%d but got tokenID:%d Instance:%s\n", currentToken.lineNum, currentToken.columnNum, currentToken.tokenID, currentToken.tkInstance);
		exit(1);
            }
        }else{
            printf("ERROR: Expected '=' at line number:%d column number:%d but got tokenID:%d Instance:%s\n", currentToken.lineNum, currentToken.columnNum, currentToken.tokenID, currentToken.tkInstance);
		exit(1);
        }
    }else{
        printf("ERROR: Expected a relational operator at line number:%d column number:%d but got tokenID:%d Instance:%s\n", currentToken.lineNum, currentToken.columnNum, currentToken.tokenID, currentToken.tkInstance);
	exit(1);
    }
    return tree;
}

//BNF: label> -> label Identifier
treenode* label(){
    treenode *tree = createnode(nullToken, "<label>");
    nextToken();
    //leave out label
    nextToken();
    if(currentToken.tokenID==1){
        tree = insertNode(tree,&currentToken);
    }else{
        printf("ERROR: Expected identifier token at line number:%d column number:%d but got tokenID:%d Instance:%s\n", currentToken.lineNum, currentToken.columnNum, currentToken.tokenID, currentToken.tkInstance);
	exit(1);
    }
    return tree;
}

//BNF: <goto> -> warp Identifier
treenode* goto_(){
    treenode *tree = createnode(nullToken, "<goto>");
    nextToken();
    //leave out warp
    nextToken();
    if(currentToken.tokenID==1){
        tree = insertNode(tree,&currentToken);
    }else{
        printf("ERROR: Expected identifier token at line number:%d column number:%d but got tokenID:%d Instance:%s\n", currentToken.lineNum, currentToken.columnNum, currentToken.tokenID, currentToken.tkInstance);
	exit(1);
    }
    return tree;
}





