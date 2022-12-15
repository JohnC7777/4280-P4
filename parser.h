typedef struct treenode{
	token value;
	char name[50];
	struct treenode *first;
	struct treenode *second;
	struct treenode *third;
	struct treenode *fourth;
	struct treenode *fifth
} treenode;

extern treenode* parsedTree;

void printtabs(int);
void printtree_rec(treenode *root, int level);
void printtree(treenode* root);
void nextToken();
void parser();
treenode *createnode(token value, char* name);
void program();
treenode* block();
treenode* expr();
treenode* N();
treenode* A();
treenode* A_();
treenode* M();
treenode* R();
treenode* vars();
treenode* stats();
treenode* stat_();
treenode* mstat();
treenode* in();
treenode* out();
treenode* if_();
treenode* loop();
treenode* assign();
treenode* RO();
treenode* goto_();
treenode* label();

