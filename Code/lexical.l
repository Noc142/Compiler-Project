%{
	#include "syntax.tab.h"
	#include "stdio.h"
	//#include "syntaxtree.h"
	int yycolumn = 1;
	int errorLexical = 0;
	#define YY_USER_ACTION \
		yylloc.first_line = yylloc.last_line = yylineno; \
		yylloc.first_column = yycolumn; \
		yylloc.last_column = yycolumn + yyleng - 1; \
		yycolumn += yyleng;
%}

%option yylineno

WS [ \t\r]
LETTER_ [_a-zA-Z]
DIGIT [0-9]
DIGITS {DIGIT}+
DEC [1-9][0-9]*|0
INT {DEC}
FLOAT {DIGITS}\.{DIGITS}
TYPE int|float
ID {LETTER_}({LETTER_}|{DIGIT})*
SEMI ;
COMMA ,
RELOP >|<|>=|<=|==|!=
ASSIGNOP =
PLUS \+
MINUS \-
STAR \*
DIV \/
AND &&
OR \|\|
DOT \.
NOT !
LP \(
RP \)
LB \[
RB \]
LC \{
RC \}
STRUCT struct
RETURN return
IF if
ELSE else
WHILE while
LINEANNO \/\/.*
ERRORFLOAT {DIGIT}{DOT}{DIGIT}*{LETTER_}+[{DIGIT}{LETTER_}]*
ERRORINT {DIGIT}+{LETTER_}+[{DIGIT}{LETTER_}]*
%%
{WS} {}
{FLOAT} {  
	yylval.type_node = newNode(yylineno, FLOAT, yytext, 0); 
return FLOAT;

}
{INT} {  yylval.type_node = newNode(yylineno, INT, yytext, 0); return INT;
}
{SEMI} { yylval.type_node = newNode(yylineno, SEMI, yytext, 0); return SEMI;
}
{COMMA} { yylval.type_node = newNode(yylineno, COMMA, yytext, 0); return COMMA;
}
{ASSIGNOP} { yylval.type_node = newNode(yylineno, ASSIGNOP, yytext, 0);return ASSIGNOP;
}
{RELOP} { yylval.type_node = newNode(yylineno, RELOP, yytext, 0); return RELOP;
}
{PLUS} { yylval.type_node = newNode(yylineno, PLUS, yytext, 0); return PLUS;
}
{MINUS} { yylval.type_node = newNode(yylineno, MINUS, yytext, 0); return MINUS;
}
{STAR} { yylval.type_node = newNode(yylineno, STAR, yytext, 0); return STAR;
}
{DIV} { yylval.type_node = newNode(yylineno, DIV, yytext, 0); return DIV;
}
{AND} { yylval.type_node = newNode(yylineno, AND, yytext, 0); return AND;
}
{OR} { yylval.type_node = newNode(yylineno, OR, yytext, 0); return OR;
}
{DOT} { yylval.type_node = newNode(yylineno, DOT, yytext, 0); return DOT;
}
{NOT} { yylval.type_node = newNode(yylineno, NOT, yytext, 0); return NOT;
}
{TYPE} { yylval.type_node = newNode(yylineno, TYPE, yytext, 0); return TYPE;
}
{LP} { yylval.type_node = newNode(yylineno, LP, yytext, 0); return LP;
}
{RP} { yylval.type_node = newNode(yylineno, RP, yytext, 0); return RP;
}
{LB} { yylval.type_node = newNode(yylineno, LB, yytext, 0); return LB;
}
{RB} { yylval.type_node = newNode(yylineno, RB, yytext, 0); return RB;
}
{LC} { yylval.type_node = newNode(yylineno, LC, yytext, 0); return LC;
}
{RC} { yylval.type_node = newNode(yylineno, RC, yytext, 0); return RC;
}
{STRUCT} { yylval.type_node = newNode(yylineno, STRUCT, yytext, 0); return STRUCT;
}
{RETURN} { yylval.type_node = newNode(yylineno, RETURN, yytext, 0); return RETURN;
}
{IF} { yylval.type_node = newNode(yylineno, IF, yytext, 0); return IF;
}
{ELSE} { yylval.type_node = newNode(yylineno, ELSE, yytext, 0); return ELSE;
} 
{WHILE}  { yylval.type_node = newNode(yylineno, WHILE, yytext, 0); return WHILE;
}
{ID} { yylval.type_node = newNode(yylineno, ID, yytext, 0); return ID;
}
{ERRORFLOAT} {errorLexical = 1;fprintf(stderr, "Error type A at Line %d: Illegal floating number \"%s\".\n", yylineno, yytext);}
{ERRORINT} {errorLexical = 1;fprintf(stderr, "Error type A at Line %d: Illegal integral number \"%s\".\n", yylineno, yytext);}
{LINEANNO} {}
"/*" { 
	int flag = 0;
	char c = input(); 
	while (c != 0) {
		if (c == '*') {
			c = input();
			if (c == '/') {
				flag = 1; break;
			}
			else unput(c);
		}
		c = input();
	}
	if (flag == 0) {
		fprintf(stderr, "Error type A at Line %d: Unterminated Comment\n", yylineno);
		errorLexical = 1;
	}
}
. {  errorLexical = 1;fprintf(stderr, "Error type A at Line %d: Mysterious characters\'%s\'\n", yylineno, yytext);}
\n { yycolumn = 1; 
}
%%
