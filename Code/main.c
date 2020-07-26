#include "syntaxtree.h"
#include <stdio.h>
#include "semantic.h"
#include "translate.h"
#include "object.h"
int errorLexical = 0;
int errorSyntax = 0;
struct ASTNode *treeroot;
extern int yyparse();
extern struct InterCodes * codes;
extern void yyrestart(FILE *);
extern int have_struct;
extern int have_error;

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
	//preOrderShow(treeroot, 0);
	Program(treeroot);
	//if (have_error == 1) return 0;
	if (argc == 3) {
		FILE *interoutput = fopen(argv[2], "w");
		if (have_struct == 1) {
			printf("Cannot translate: Code contains variables or parameters of structure type.\n");
			fprintf(interoutput, "Cannot translate: Code contains variables or parameters of structure type.\n");
		}
		else {
			translate_Program(treeroot);
			//printCodes(codes, stdout);
			objTranslate(interoutput);
		}
	}
	deleteTree(treeroot);
	return 0;
}
