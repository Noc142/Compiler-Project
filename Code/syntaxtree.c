#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <memory.h>
#include "syntaxtree.h"
#include "syntax.tab.h"
#include "scanner.h"
struct ASTNode* newNode(int l, int t, char *text, char empty) {
	struct ASTNode *node = malloc(sizeof(struct ASTNode));
	node->lineno = l;
	node->type = t;
	node->isEmpty = empty;
	memset(node->val, '\0', sizeof(char)*BUFFER_LEN);
	if (text != NULL)
		strcpy(node->val, text);
	//if (text == NULL)
	//	printf("%d\n", t);
	node->next = NULL;
	node->child = NULL;
	return node;
}

struct ASTNode *buildChildren(int num, ...) {
	va_list ap;
	va_start(ap, num);
	int i = 1;
	struct ASTNode *ret = va_arg(ap, struct ASTNode*);
	struct ASTNode *cur = ret;
	for ( ; i < num; ++i) {
		cur->next = va_arg(ap, struct ASTNode*);
		cur = cur->next;
	}
	va_end(ap);
	return ret;
}
void insertChildren(struct ASTNode *parent, struct ASTNode *child) {
	parent->child = child;
}
void output(struct ASTNode *root) {
	switch (root->type) {
		case Program: printf("Program (%d)\n", root->lineno); break;
		case ExtDefList: printf("ExtDefList (%d)\n", root->lineno); break;
		case ExtDef: printf("ExtDef (%d)\n", root->lineno); break;
		case ExtDecList: printf("ExtDecList (%d)\n", root->lineno); break;
		case Specifier: printf("Specifier (%d)\n", root->lineno); break;
		case StructSpecifier: printf("StructSpecifier (%d)\n", root->lineno); break;
		case OptTag: printf("OptTag (%d)\n", root->lineno); break;
		case Tag: printf("Tag (%d)\n", root->lineno); break;
		case VarDec: printf("VarDec (%d)\n", root->lineno); break;
		case FunDec: printf("FunDec (%d)\n", root->lineno); break;
		case VarList: printf("VarList (%d)\n", root->lineno); break;
		case ParamDec: printf("ParamDec (%d)\n", root->lineno); break;
		case CompSt: printf("CompSt (%d)\n", root->lineno); break;
		case StmtList: printf("StmtList (%d)\n", root->lineno); break;
		case Stmt: printf("Stmt (%d)\n", root->lineno); break;
		case DefList: printf("DefList (%d)\n", root->lineno); break;
		case Def: printf("Def (%d)\n", root->lineno); break;
		case DecList: printf("DecList (%d)\n", root->lineno); break;
		case Dec: printf("Dec (%d)\n", root->lineno); break;
		case Exp: printf("Exp (%d)\n", root->lineno); break;
		case Args: printf("Args (%d)\n", root->lineno); break;
		case ID: printf("ID: %s\n", root->val); break;
		case INT: printf("INT: %d\n", atoi(root->val)); break;
		case FLOAT: printf("FLOAT: %f\n", atof(root->val)); break;
		case SEMI: printf("SEMI\n"); break;
		case TYPE: printf("TYPE: %s\n", root->val); break;
		case STRUCT: printf("STRUCT\n"); break;
		case RETURN: printf("RETURN\n"); break;
		case IF: printf("IF\n"); break;
		case WHILE: printf("WHILE\n"); break;
		case COMMA: printf("COMMA\n"); break;
		case OR: printf("OR\n"); break;
		case AND: printf("AND\n"); break;
		case RELOP: printf("RELOP\n"); break;
		case PLUS: printf("PLUS\n"); break;
		case MINUS: printf("MINUS\n"); break;
		case STAR: printf("STAR\n"); break;
		case DIV: printf("DIV\n"); break;
		case LB: printf("LB\n"); break;
		case RB: printf("RB\n"); break;
		case LC: printf("LC\n"); break;
		case RC: printf("RC\n"); break;
		case DOT: printf("DOT\n"); break;
		case LP: printf("LP\n"); break;
		case RP: printf("RP\n"); break;
		case NOT: printf("NOT\n"); break;
		case ASSIGNOP: printf("ASSIGNOP\n"); break;
		case ELSE: printf("ELSE\n"); break;
	}
}
void preOrderShow(struct ASTNode *root, int depth) {
	//printf("one call");
	if (root != NULL) {
		if (!root->isEmpty) {
			for (int i = 0; i < depth; ++i) printf("  ");
			output(root);
		}
		preOrderShow(root->child, depth + 1);
		preOrderShow(root->next, depth);
	}
}
void deleteTree(struct ASTNode *root) {
	if (root != NULL) {
		deleteTree(root->child);
		deleteTree(root->next);
		free(root);
	}
}
