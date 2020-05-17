#ifndef LIST_H
#define LIST_H
#include <stdio.h>

typedef struct Operand_* Operand;
struct Operand_ {
	enum { VARIABLE, CONSTANT, ADDRESS, FUNCTION, TMPVAR, MEMORY, ARRADDRESS, ARRAYNAME } kind;
	union {
		int var_no;//addr tmp
		int value;//const memory
		char *name;//var func
	} u;
	int star;
};
typedef struct InterCode {
	enum { ASSIGN, ADD, SUB, MUL, DIV, AND, OR, GOTOOP, CONDOP, FUNC, PARAM, READ, WRITE, RETURN, LABEL, CALL, ARG, DEC} kind;
	union {
		struct { Operand right, left; } assign;
		struct { Operand result, op1, op2; } binop; // shuang mu yun suan
		struct { Operand result, op; } unaop;//dan mu yun suan
		struct { Operand L; } gotoop;
		struct { Operand L, op1, op2; char *op; } condop;
		struct { Operand result; char *funcname; } callop;
		struct { Operand op,memory; }dec;
		Operand x;
		char *name;
	} u;
} InterCode;
typedef struct InterCodes { InterCode code; struct InterCodes *prev, *next; } InterCodes;
typedef struct ArgNode {
	Operand op;
	struct ArgNode *next;
} ArgNode;
void InsertArg(struct ArgNode **head, Operand op);
void deleteArgList(struct ArgNode *head);
struct InterCodes * newCodeHead();
Operand new_var(char *name);
/*codes is the head of the InterCode List, and code is a code*/
void InsertCode(struct InterCodes *codes, struct InterCode code);
Operand new_temp(int *no);
Operand new_const(int value);
Operand new_memory(int value);
Operand new_func(char *name);
Operand new_label(int *no);
Operand new_array(char* name);
Operand new_arrayaddress(int* no,int star);
void printCodes(struct InterCodes *codes, FILE *stream);
void printInterCode(struct InterCode *code, FILE *stream);
void printOperand(Operand op, FILE *stream);
void makeCode(struct InterCode *code, int kind, ...);
Operand copyop(Operand op);

#endif
