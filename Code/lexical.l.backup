%{
	#include "syntax.tab.h"
	#include "syntaxtree.h"
	int yycolumn = 1;
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

%%
{WS} {}
{FLOAT} { printf("FLOAT\n"); 
	yylval.type_node = newNode(yylineno, FLOAT, yytext); 
return FLOAT;

}
{INT} { printf("INT\n"); yylval.type_node = newNode(yylineno, INT, yytext); return INT;
}
{SEMI} { printf("SEMI\n"); yylval.type_node = newNode(yylineno, SEMI, yytext); return SEMI;
}
{COMMA} { printf("COMMA\n"); yylval.type_node = newNode(yylineno, COMMA, yytext); return COMMA;
}
{ASSIGNOP} { printf("ASSIGNOP\n"); yylval.type_node = newNode(yylineno, ASSIGNOP, yytext);return ASSIGNOP;
}
{RELOP} { printf("RELOP\n"); yylval.type_node = newNode(yylineno, RELOP, yytext); return RELOP;
}
{PLUS} { printf("PLUS\n"); yylval.type_node = newNode(yylineno, PLUS, yytext); return PLUS;
}
{MINUS} { printf("MINUS\n"); yylval.type_node = newNode(yylineno, MINUS, yytext); return MINUS;
}
{STAR} { printf("STAR\n"); yylval.type_node = newNode(yylineno, STAR, yytext); return STAR;
}
{DIV} { printf("DIV\n"); yylval.type_node = newNode(yylineno, DIV, yytext); return DIV;
}
{AND} { printf("AND\n"); yylval.type_node = newNode(yylineno, AND, yytext); return AND;
}
{OR} { printf("OR\n"); yylval.type_node = newNode(yylineno, OR, yytext); return OR;
}
{DOT} { printf("DOT\n"); yylval.type_node = newNode(yylineno, DOT, yytext); return DOT;
}
{NOT} { printf("NOT\n"); yylval.type_node = newNode(yylineno, NOT, yytext); return NOT;
}
{TYPE} { printf("TYPE\n"); yylval.type_node = newNode(yylineno, TYPE, yytext); return TYPE;
}
{LP} { printf("LP\n"); yylval.type_node = newNode(yylineno, LP, yytext); return LP;
}
{RP} { printf("RP\n"); yylval.type_node = newNode(yylineno, RP, yytext); return RP;
}
{LB} { printf("LB\n"); yylval.type_node = newNode(yylineno, LB, yytext); return LB;
}
{RB} { printf("RB\n"); yylval.type_node = newNode(yylineno, RB, yytext); return RB;
}
{LC} { printf("LC\n");yylval.type_node = newNode(yylineno, LC, yytext); return LC;
}
{RC} { printf("RC\n"); yylval.type_node = newNode(yylineno, RC, yytext); return RC;
}
{STRUCT} { printf("STRUCT\n"); yylval.type_node = newNode(yylineno, STRUCT, yytext); return STRUCT;
}
{RETURN} { printf("RETURN\n"); yylval.type_node = newNode(yylineno, RETURN, yytext); return RETURN;
}
{IF} { printf("IF\n"); yylval.type_node = newNode(yylineno, IF, yytext); return IF;
}
{ELSE} { printf("ELSE\n"); yylval.type_node = newNode(yylineno, ELSE, yytext); return ELSE;
} 
{WHILE}  { printf("WHILE\n"); yylval.type_node = newNode(yylineno, WHILE, yytext); return WHILE;
}
{ID} { printf("ID\n"); yylval.type_node = newNode(yylineno, ID, yytext); return ID;
}
. {  printf("Error type A at Line %d: Mysterious characters\'%s\'\n", yylineno, yytext);}
\n { yycolumn = 1; 
}
%%
