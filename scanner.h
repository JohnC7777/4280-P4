typedef struct token{
    int tokenID;
    int lineNum;
    int columnNum;
    char tkInstance[50];

} token;

typedef struct node_t {
    token value;
    struct node* next;
} node_t;
//typedef struct node node_t;

extern node_t* current;

void nextNode(int);
int getColumn(int);
void tokenizeString(char*);
char* formatLine(char*);
bool isReserved(char*);
node_t *newNode(token);
bool accepting;
int lineNum;
void reverse();
void print();
