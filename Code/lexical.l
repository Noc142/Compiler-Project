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

%%
{WS} {}
{FLOAT} {  
	yylval.type_node = newNode(yylineno, FLOAT, yytext); 
return FLOAT;

}
{INT} {  yylval.type_node = newNode(yylineno, INT, yytext); return INT;
}
{SEMI} { yylval.type_node = newNode(yylineno, SEMI, yytext); return SEMI;
}
{COMMA} { yylval.type_node = newNode(yylineno, COMMA, yytext); return COMMA;
}
{ASSIGNOP} { yylval.type_node = newNode(yylineno, ASSIGNOP, yytext);return ASSIGNOP;
}
{RELOP} { yylval.type_node = newNode(yylineno, RELOP, yytext); return RELOP;
}
{PLUS} { yylval.type_node = newNode(yylineno, PLUS, yytext); return PLUS;
}
{MINUS} { yylval.type_node = newNode(yylineno, MINUS, yytext); return MINUS;
}
{STAR} { yylval.type_node = newNode(yylineno, STAR, yytext); return STAR;
}
{DIV} { yylval.type_node = newNode(yylineno, DIV, yytext); return DIV;
}
{AND} { yylval.type_node = newNode(yylineno, AND, yytext); return AND;
}
{OR} { yylval.type_node = newNode(yylineno, OR, yytext); return OR;
}
{DOT} { yylval.type_node = newNode(yylineno, DOT, yytext); return DOT;
}
{NOT} { yylval.type_node = newNode(yylineno, NOT, yytext); return NOT;
}
{TYPE} { yylval.type_node = newNode(yylineno, TYPE, yytext); return TYPE;
}
{LP} { yylval.type_node = newNode(yylineno, LP, yytext); return LP;
}
{RP} { yylval.type_node = newNode(yylineno, RP, yytext); return RP;
}
{LB} { yylval.type_node = newNode(yylineno, LB, yytext); return LB;
}
{RB} { yylval.type_node = newNode(yylineno, RB, yytext); return RB;
}
{LC} { yylval.type_node = newNode(yylineno, LC, yytext); return LC;
}
{RC} { yylval.type_node = newNode(yylineno, RC, yytext); return RC;
}
{STRUCT} { yylval.type_node = newNode(yylineno, STRUCT, yytext); return STRUCT;
}
{RETURN} { yylval.type_node = newNode(yylineno, RETURN, yytext); return RETURN;
}
{IF} { yylval.type_node = newNode(yylineno, IF, yytext); return IF;
}
{ELSE} { yylval.type_node = newNode(yylineno, ELSE, yytext); return ELSE;
} 
{WHILE}  { yylval.type_node = newNode(yylineno, WHILE, yytext); return WHILE;
}
{ID} { yylval.type_node = newNode(yylineno, ID, yytext); return ID;
}
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
		printf("Error type A at Line %d: Unterminated Comment\n", yylineno);
		errorLexical = 1;
	}
}
. {  errorLexical = 1;printf("Error type A at Line %d: Mysterious characters\'%s\'\n", yylineno, yytext);}
\n { yycolumn = 1; 
}
%%
