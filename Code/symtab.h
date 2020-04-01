#ifndef SYMTAB_H
#define SYMTAB_H

#define DEFAULT_HASH_SIZE 512
#define STRLEN 40
/*Binary Search Tree Node*/
typedef struct BSTNode {
	char key[STRLEN]; //variable's name
	int type; //variable's type
	struct BSTNode *left, *right;
} BSTNode;

typedef struct HashTable{
	BSTNode *table[DEFAULT_HASH_SIZE];
} HashTable, Symtab;

void initBSTNode(BSTNode *node, char k[], int tp, struct BSTNode *l, struct BSTNode *r);

/*Insert a symbol to this tree*/
int Insert(BSTNode *ptr, char k[], int tp);
/*Judge if Symbol k in the tree*/
int Contains(BSTNode *ptr, char k[]);
void delete_BST(BSTNode *ptr);

int HashContains(struct HashTable table, char k[]);
int HashInsert(struct HashTable table, char k[], int tp);


#endif
