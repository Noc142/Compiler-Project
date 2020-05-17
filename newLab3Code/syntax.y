%{
	#include <stdio.h>
	#include <stdlib.h>
	#include "syntaxtree.h"
	#include "scanner.h"
	#include "lex.yy.c"
	//#define YYDEBUG 1
	extern struct ASTNode *treeroot;
	extern int errorLexical;
	extern int errorSyntax;
	extern int yyerror(char*);
	extern int synerror(char *);
%}
%union {
	int type_int;
	float type_float;
	double type_double;
	struct ASTNode *type_node;
}

%token INT 
%token FLOAT 
%token ID SEMI TYPE STRUCT RETURN IF WHILE
%token COMMA OR AND RELOP PLUS MINUS STAR DIV LB RB LC RC DOT LP RP NOT ASSIGNOP ELSE

%type <type_node> Program ExtDefList ExtDef ExtDecList
%type <type_node> Specifier StructSpecifier OptTag Tag
%type <type_node> VarDec FunDec VarList ParamDec
%type <type_node> CompSt StmtList Stmt
%type <type_node> DefList Def DecList Dec
%type <type_node> Exp Args
%type <type_node> INT 
%type <type_node> FLOAT 
%type <type_node> ID SEMI TYPE STRUCT RETURN IF WHILE
%type <type_node> COMMA OR AND RELOP PLUS MINUS STAR DIV LB RB LC RC DOT LP RP NOT ASSIGNOP ELSE

/*Priority, fronter, lower; backer, higher*/
%left COMMA
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left LB RB LC RC DOT
%left LP RP
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%
/*High-level Definitions*/
Program : ExtDefList { $$ = newNode(1, @$.first_line, Program, NULL, 0); insertChildren($$, buildChildren(1, $1)); treeroot = $$; }
	;
ExtDefList : ExtDef ExtDefList { $$ = newNode(1, @$.first_line, ExtDefList, NULL, 0); insertChildren($$, buildChildren(2, $1, $2));}
	| /*empty*/ { $$ = newNode(2, @$.first_line, ExtDefList, NULL, 1); @$.first_line = yylineno;}
	;
ExtDef : Specifier ExtDecList SEMI { $$ = newNode(1, @$.first_line, ExtDef, NULL, 0); insertChildren($$, buildChildren(3, $1, $2, $3));}
	| Specifier SEMI { $$ = newNode(2, @$.first_line, ExtDef, NULL, 0); insertChildren($$, buildChildren(2, $1, $2));}
	| Specifier FunDec CompSt { $$ = newNode(3, @$.first_line, ExtDef, NULL, 0); insertChildren($$, buildChildren(3, $1, $2, $3));}
	| Specifier FunDec SEMI { $$ = newNode(4, @$.first_line, ExtDef, NULL, 0); insertChildren($$, buildChildren(3, $1, $2, $3));}
	| Specifier ExtDecList error { errorSyntax = 1; synerror("Missing \";\".");}	
	| Specifier error { errorSyntax = 1; synerror("Missing \";\".");}
	;

ExtDecList : VarDec { $$ = newNode(1, @$.first_line, ExtDecList, NULL, 0); insertChildren($$, buildChildren(1, $1));}
	| VarDec COMMA ExtDecList { $$ = newNode(2, @$.first_line, ExtDecList, NULL, 0); insertChildren($$, buildChildren(3, $1, $2, $3));}	
	| error CompSt {synerror("Missing something before \"{\".");}
	;

/*Specifiers*/
Specifier : TYPE { $$ = newNode(1, @$.first_line, Specifier, NULL, 0); insertChildren($$, buildChildren(1, $1));}
	| StructSpecifier { $$ = newNode(2, @$.first_line, Specifier, NULL, 0); insertChildren($$, buildChildren(1, $1));}
	;
StructSpecifier : STRUCT OptTag LC DefList RC { $$ = newNode(1, @$.first_line, StructSpecifier, NULL, 0); insertChildren($$, buildChildren(5, $1, $2, $3, $4, $5));}
	| STRUCT Tag { $$ = newNode(2, @$.first_line, StructSpecifier, NULL, 0); insertChildren($$, buildChildren(2, $1, $2));}
	| STRUCT error LC DefList RC {synerror("Wrong struct.");}
	;
OptTag : ID { $$ = newNode(1, @$.first_line, OptTag, NULL, 0); insertChildren($$, buildChildren(1, $1));}
	| /*empty*/ { $$ = newNode(2, @$.first_line, OptTag, NULL, 1);}
	;
Tag : ID { $$ = newNode(1, @$.first_line, Tag, NULL, 0); insertChildren($$, buildChildren(1, $1));}
	;

/*Declarators*/
VarDec : ID { $$ = newNode(1, @$.first_line, VarDec, NULL, 0); insertChildren($$, buildChildren(1, $1));}
	| VarDec LB INT RB { $$ = newNode(2, @$.first_line, VarDec, NULL, 0); insertChildren($$, buildChildren(4, $1, $2, $3, $4));}
	;
FunDec : ID LP VarList RP { $$ = newNode(1, @$.first_line, FunDec, NULL, 0); insertChildren($$, buildChildren(4, $1, $2, $3, $4));}
	| ID LP RP { $$ = newNode(2, @$.first_line, FunDec, NULL, 0); insertChildren($$, buildChildren(3, $1, $2, $3));}
	| ID LP VarList error { synerror("Missing \")\".");}		
	| ID LP error { synerror("Missing \")\".");}	
	;
VarList : ParamDec COMMA VarList { $$ = newNode(1, @$.first_line, VarList, NULL, 0); insertChildren($$, buildChildren(3, $1, $2, $3));}
	| ParamDec { $$ = newNode(2, @$.first_line, VarList, NULL, 0); insertChildren($$, buildChildren(1, $1));}
	| ParamDec error VarList { synerror("Missing \",\".");}	
	;
ParamDec : Specifier VarDec { $$ = newNode(1, @$.first_line, ParamDec, NULL, 0); insertChildren($$, buildChildren(2, $1, $2));}
	;

/*Statements*/
CompSt : LC DefList StmtList RC { $$ = newNode(1, @$.first_line, CompSt, NULL, 0); insertChildren($$, buildChildren(4, $1, $2, $3, $4)); }
	| LC DefList StmtList error { errorSyntax = 1; synerror("Missing \"}\".");}		
	;
StmtList : Stmt StmtList { $$ = newNode(1, @$.first_line, StmtList, NULL, 0); insertChildren($$, buildChildren(2, $1, $2));}
	| /*empty*/ { $$ = newNode(2, @$.first_line, StmtList, NULL, 1);}
	;
Stmt : Exp SEMI { $$ = newNode(1, @$.first_line, Stmt, NULL, 0); insertChildren($$, buildChildren(2, $1, $2));}
	| CompSt { $$ = newNode(2, @$.first_line, Stmt, NULL, 0); insertChildren($$, buildChildren(1, $1));}
	| RETURN Exp SEMI { $$ = newNode(3, @$.first_line, Stmt, NULL, 0); insertChildren($$, buildChildren(3, $1, $2, $3));}
	| RETURN Exp error  { synerror("Missing \";\".");}	
	| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE { $$ = newNode(4, @$.first_line, Stmt, NULL, 0); insertChildren($$, buildChildren(5, $1, $2, $3, $4, $5));}
	| IF LP Exp RP Stmt ELSE Stmt { $$ = newNode(5, @$.first_line, Stmt, NULL, 0); insertChildren($$, buildChildren(7, $1, $2, $3, $4, $5, $6, $7));}
	| WHILE LP Exp RP Stmt { $$ = newNode(6, @$.first_line, Stmt, NULL, 0); insertChildren($$, buildChildren(5, $1, $2, $3, $4, $5));}
	| Exp error { errorSyntax = 1; synerror("Missing \";\".");}	
	| error SEMI { synerror("Missing statement before \";\".");}	
	;

/*Local Definitions*/
DefList : Def DefList { $$ = newNode(1, @$.first_line, DefList, NULL, 0); insertChildren($$, buildChildren(2, $1, $2));}
	| /*empty*/ {$$ = newNode(2, @$.first_line, DefList, NULL, 1);}
	;
Def : Specifier DecList SEMI { $$ = newNode(1, @$.first_line, Def, NULL, 0); insertChildren($$, buildChildren(3, $1, $2, $3));}
	| Specifier error SEMI 	{ errorSyntax = 1; synerror("Wrong defination.");}
	| Specifier	DecList error { errorSyntax = 1; synerror("Missing \";\".");}
	| error SEMI { errorSyntax = 1; synerror("Wrong defination."); }
	;
DecList : Dec { $$ = newNode(1, @$.first_line, DecList, NULL, 0); insertChildren($$, buildChildren(1, $1));}
	| Dec COMMA DecList { $$ = newNode(2, @$.first_line, DecList, NULL, 0); insertChildren($$, buildChildren(3, $1, $2, $3));}
	;
Dec : VarDec { $$ = newNode(1, @$.first_line, Dec, NULL, 0); insertChildren($$, buildChildren(1, $1));}
	| VarDec ASSIGNOP Exp { $$ = newNode(2, @$.first_line, Dec, NULL, 0); insertChildren($$, buildChildren(3, $1, $2, $3));}
	;


/*Expressions*/
Exp : Exp ASSIGNOP Exp { $$ = newNode(1, @$.first_line, Exp, NULL, 0); insertChildren($$, buildChildren(3, $1, $2, $3));}
	| Exp AND Exp { $$ = newNode(2, @$.first_line, Exp, NULL, 0); insertChildren($$, buildChildren(3, $1, $2, $3));}
	| Exp OR Exp { $$ = newNode(3, @$.first_line, Exp, NULL, 0); insertChildren($$, buildChildren(3, $1, $2, $3));}
	| Exp RELOP Exp { $$ = newNode(4, @$.first_line, Exp, NULL, 0); insertChildren($$, buildChildren(3, $1, $2, $3));}
	| Exp PLUS Exp { $$ = newNode(5, @$.first_line, Exp, NULL, 0); insertChildren($$, buildChildren(3, $1, $2, $3));}
	| Exp MINUS Exp { $$ = newNode(6, @$.first_line, Exp, NULL, 0); insertChildren($$, buildChildren(3, $1, $2, $3));}
	| Exp STAR Exp { $$ = newNode(7, @$.first_line, Exp, NULL, 0); insertChildren($$, buildChildren(3, $1, $2, $3));}
	| Exp DIV Exp { $$ = newNode(8, @$.first_line, Exp, NULL, 0); insertChildren($$, buildChildren(3, $1, $2, $3));}
	| LP Exp RP { $$ = newNode(9, @$.first_line, Exp, NULL, 0); insertChildren($$, buildChildren(3, $1, $2, $3));}
	| LP Exp error { errorSyntax = 1; synerror("Missing \")\".");}
	| MINUS Exp { $$ = newNode(10, @$.first_line, Exp, NULL, 0); insertChildren($$, buildChildren(2, $1, $2));}
	| NOT Exp { $$ = newNode(11, @$.first_line, Exp, NULL, 0); insertChildren($$, buildChildren(2, $1, $2));}
	| ID LP Args RP { $$ = newNode(12, @$.first_line, Exp, NULL, 0); insertChildren($$, buildChildren(4, $1, $2, $3, $4));}
	| ID LP Args error { synerror("Missing \")\"."); }
	| ID LP RP { $$ = newNode(13, @$.first_line, Exp, NULL, 0); insertChildren($$, buildChildren(3, $1, $2, $3));}
	| Exp LB Exp RB { $$ = newNode(14, @$.first_line, Exp, NULL, 0); insertChildren($$, buildChildren(4, $1, $2, $3, $4));}
	| Exp LB error RB { errorSyntax = 1; synerror("Wrong expression in \"[]\".");}
	| Exp LB Exp error RB { errorSyntax = 1; synerror("Missing \"]\".");}
	| Exp ASSIGNOP error { errorSyntax = 1; synerror("Wrong \"=\" expression.");}
	| Exp AND error { errorSyntax = 1; synerror("Wrong \"&&\" expression.");}
	| Exp OR error { errorSyntax = 1; synerror("Wrong \"||\" expression.");}
	| Exp RELOP error { errorSyntax = 1; synerror("Wrong \"relop\" expression.");}
	| Exp PLUS error { errorSyntax = 1; synerror("Wrong \"+\" expression.");}
	| Exp MINUS error { errorSyntax = 1; synerror("Wrong \"-\" expression.");}
	| Exp STAR error { errorSyntax = 1; synerror("Wrong \"*\" expression.");}
	| Exp DIV error { errorSyntax = 1; synerror("Wrong \"/\" expression.");}
	| Exp DOT ID { $$ = newNode(15, @$.first_line, Exp, NULL, 0); insertChildren($$, buildChildren(3, $1, $2, $3));}
	| ID { $$ = newNode(16, @$.first_line, Exp, NULL, 0); insertChildren($$, buildChildren(1, $1));}
	| INT { $$ = newNode(17, @$.first_line, Exp, NULL, 0); insertChildren($$, buildChildren(1, $1));}
	| FLOAT { $$ = newNode(18, @$.first_line, Exp, NULL, 0); insertChildren($$, buildChildren(1, $1));}
	;
Args : Exp COMMA Args { $$ = newNode(1, @$.first_line, Args, NULL, 0); insertChildren($$, buildChildren(3, $1, $2, $3));}
	| Exp { $$ = newNode(2, @$.first_line, Args, NULL, 0); insertChildren($$, buildChildren(1, $1));}
	| Exp error Args {synerror("Missing comma ',' between args.");}	
	;


%%
int yyerror(char* msg) {
	errorSyntax = 1;
	//fprintf(stderr, "Error type B at Line %d: %s\n",yylineno,msg);
}
int synerror(char *msg) {
	fprintf(stderr, "Error type B at Line %d: %s\n", yylineno, msg);
}
