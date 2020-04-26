#include "list.h"
#include <stdlib.h>
#include <stdarg.h>

struct InterCodes * newCodeHead() {
	struct InterCodes* head = (struct InterCodes *) malloc(sizeof(struct InterCodes));
	head->prev = head;
	head->next = head;
	return head;
}
/*codes is the head of the InterCode List, and code is a code*/
void InsertCode(struct InterCodes *codes, struct InterCode code) {
	struct InterCodes *newcode = (struct InterCodes*) malloc(sizeof(struct InterCodes));
	newcode->next = codes;
	newcode->prev = codes->prev;
	newcode->prev->next = newcode;
	codes->prev = newcode;
	newcode->code = code;
}
void makeCode(struct InterCode *code, int kind, ...) {
	va_list ap;
	va_start(ap, kind);
	code->kind = kind;
	switch (kind) {
		case ASSIGN: {
			code->u.assign.left = (Operand) va_arg(ap, Operand);
			code->u.assign.right = (Operand) va_arg(ap, Operand);
		} break;
		case ADD: 
		case SUB:
		case MUL:
		case DIV:
		{
			code->u.binop.result = (Operand) va_arg(ap, Operand);
			code->u.binop.op1 = (Operand) va_arg(ap, Operand);
			code->u.binop.op2 = (Operand) va_arg(ap, Operand);
		} break;
		case GOTOOP:
		{
			code->u.gotoop.L = (Operand) va_arg(ap, Operand);
		} break;
		case CONDOP:
		{
			code->u.condop.L = (Operand) va_arg(ap, Operand);
			code->u.condop.op1 = (Operand) va_arg(ap, Operand);
			code->u.condop.op2 = (Operand) va_arg(ap, Operand);
		} break;
		default:
		{
			code->u.x = (Operand) va_arg(ap, Operand); 
		} break;
	}
	va_end(ap);
}
Operand new_var(char *name) {
	Operand ret = (Operand) malloc(sizeof(struct Operand_));
	ret->kind = VARIABLE;
	ret->u.name = name;
	return ret;
}
Operand new_label(int *no) {
	Operand ret = (Operand) malloc(sizeof(struct Operand_));
	ret->kind = ADDRESS;
	ret->u.name = (char *) malloc(40);
	ret->u.var_no = *no;
	(*no)++;
	return ret;
}
Operand new_temp(int *no) {
	Operand ret = (Operand) malloc(sizeof(struct Operand_));
	ret->kind = TMPVAR;
	ret->u.name = (char *) malloc(40);
	ret->u.var_no = *no;
	(*no)++;
	return ret;
}
Operand new_func(char *name) {
	Operand ret = (Operand) malloc(sizeof(struct Operand_));
	ret->kind = FUNCTION;
	ret->u.name = name;
	return ret;
}
Operand new_const(int value) {
	Operand ret = (Operand) malloc(sizeof(struct Operand_));
	ret->kind = CONSTANT;
	ret->u.value = value;
	//printf("value: %d", value);
	return ret;
}
void printCodes(struct InterCodes *codes, FILE *stream) {
	struct InterCodes *cur = codes->next;
	while (cur != codes) {
		printInterCode(&cur->code, stream);
		cur = cur->next;
	}
}

void printInterCode(struct InterCode *code, FILE *stream) { //attention!! InterCode* but not InterCodes*
	if (code->kind == ASSIGN) {
		printOperand(code->u.assign.left, stream);
		fprintf(stream, " := ");
		printOperand(code->u.assign.right, stream);
		fprintf(stream, "\n");		
	}
	else if (code->kind >= ADD && code->kind <= DIV) {
		printOperand(code->u.binop.result, stream);
		fprintf(stream, " := ");
		printOperand(code->u.binop.op1, stream);
		switch(code->kind) {
			case ADD: fprintf(stream, " + "); break;
			case SUB: fprintf(stream, " - "); break;
			case MUL: fprintf(stream, " * "); break;
			case DIV: fprintf(stream, " / "); break;
		}
		printOperand(code->u.binop.op2, stream);
		fprintf(stream, "\n");		
	}
	else if (code->kind == FUNC) {
		fprintf(stream, "FUNCTION ");
		printOperand(code->u.x, stream);
		fprintf(stream, ":\n");
	}
	else if (code->kind == LABEL) {
		fprintf(stream, "LABEL ");
		printOperand(code->u.x, stream);
		fprintf(stream, ":\n");
	}
	else if (code->kind == GOTOOP) {
		fprintf(stream, "GOTO ");
		printOperand(code->u.x, stream);
		fprintf(stream, "\n");
	}
}

void printOperand(Operand op, FILE *stream) {
	switch (op->kind) {
		case VARIABLE: fprintf(stream, "%s", op->u.name); break;
		case CONSTANT: fprintf(stream, "#%d", op->u.value); break;
		case TMPVAR: fprintf(stream, "t%d", op->u.var_no); break;
		case FUNCTION: fprintf(stream, "%s", op->u.name); break;
		case ADDRESS: fprintf(stream, "lable%d", op->u.var_no); break;
	}
}
