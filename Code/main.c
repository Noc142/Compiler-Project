#include "syntaxtree.h"
#include <stdio.h>

int errorLexical = 0;
int errorSyntax = 0;
struct ASTNode *treeroot;
extern int yyparse();
extern void yyrestart(FILE *);

int main(int argc, char **argv) {
	if (argc == 1) {

	}
	else {
		FILE *fp = fopen(argv[1], "r");
		if (fp == NULL) {
			printf("File %s does not exist!\n", argv[1]);
			return -1;
		}
		yyrestart(fp);
	}
	//yydebug = 1;
	yyparse();
	if (errorLexical) fprintf(stderr, "Lexical errors exist!\n");
	if (errorSyntax) fprintf(stderr, "Syntax errors exist!\n");
	if (errorLexical || errorSyntax) return 0;
	preOrderShow(treeroot, 0);
	deleteTree(treeroot);
	return 0;
}
