#ifndef SYNTAXTREE_H
#define SYNTAXTREE_H


#include <stdio.h>
#include <stdlib.h>
#define BUFFER_LEN 40
typedef struct ASTNode {
	int lineno;//Which line it is set	
	int type;//Which Type, like id or relop
	//char type_name[BUFFER_LEN];
	char isEmpty;
	int npro;/*which production used*/
	char val[BUFFER_LEN];//Attribute Value, = yytext
	struct ASTNode *child;//Child
	struct ASTNode *next;//Brother
} Node;

extern struct ASTNode* newNode(int n/*which production used*/, int l, int t, char *text, char empty);
extern struct ASTNode* newNode_t(int l, int t, char *text); //terminal's newNode
extern struct ASTNode *buildChildren(int num, ...);
extern void insertChildren(struct ASTNode *parent, struct ASTNode *child);
extern void deleteTree(struct ASTNode *root);
extern void output(struct ASTNode *node);
extern void preOrderShow(struct ASTNode *root, int depth);
extern void preOrderAnalyze(struct ASTNode *root);

#endif
