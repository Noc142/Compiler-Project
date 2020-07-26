#include "list.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
extern int var_no;
struct InterCodes * newCodeHead() {
	struct InterCodes* head = (struct InterCodes *) malloc(sizeof(struct InterCodes));
	head->prev = head;
	head->next = head;
	return head;
}
void InsertArg(struct ArgNode **head, Operand op) {
	if (*head == NULL) {
		*head = (ArgNode*) malloc(sizeof(struct ArgNode));
		(*head)->op = op;
		(*head)->next = NULL;
	}
	else {
		struct ArgNode *newArg = (ArgNode *) malloc(sizeof(struct ArgNode));
		newArg->next = *head;
		newArg->op = op;
		(*head) = newArg;
	}
}
void deleteArgList(struct ArgNode *head) {
	struct ArgNode *cur = head;
	while (cur != NULL) {
		head = head->next;
		free(cur);
		cur = head;
	}
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
		case DEC: {
			code->u.dec.op= (Operand)va_arg(ap, Operand);
			code->u.dec.memory= (Operand)va_arg(ap, Operand);
			break;
		}
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
			code->u.condop.L = va_arg(ap, Operand);
			code->u.condop.op1 = (Operand) va_arg(ap, Operand);
			code->u.condop.op2 = (Operand) va_arg(ap, Operand);
			code->u.condop.op = va_arg(ap, char *);
		} break;
		case FUNC: {
			code->u.name = va_arg(ap, char*);
		} break;
		case CALL: {
			code->u.callop.result = va_arg(ap, Operand);
			code->u.callop.funcname = va_arg(ap, char*);
		} break;
		case ARG: {
			code->u.x = va_arg(ap, Operand);
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
	ret->u.var_no = *no;
	(*no)++;
	return ret;
}
Operand new_temp(int *no) {
	Operand ret = (Operand) malloc(sizeof(struct Operand_));
	ret->kind = TMPVAR;
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
Operand new_memory(int value) {
	Operand ret = (Operand)malloc(sizeof(struct Operand_));
	ret->kind = MEMORY;
	ret->u.value = value;
	return ret;
}
Operand new_array(char* name)
{
	Operand ret = (Operand)malloc(sizeof(struct Operand_));
	ret->kind = ARRAYNAME;
	ret->u.name = name;
	return ret;
}
Operand new_arrayaddress(int* no,int star)
{
	Operand ret = (Operand)malloc(sizeof(struct Operand_));
	ret->kind = ARRADDRESS;
	ret->u.var_no = *no;
	ret->star = star;
	(*no)++;
	return ret;
}
Operand copyop(Operand op)
{
	Operand ret = (Operand)malloc(sizeof(struct Operand_));
	ret->kind = op->kind;
	ret->u = op->u;
	ret->star = op->star;
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
		if (code->u.assign.left == NULL) return;
		printOperand(code->u.assign.left, stream);
		fprintf(stream, " := ");
		printOperand(code->u.assign.right, stream);
		fprintf(stream, "\n");		
	}
	else if (code->kind >= ADD && code->kind <= DIV) {
		if (code->u.binop.result == NULL) return;
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
		fprintf(stream, "%s", code->u.name);
		fprintf(stream, " :\n");
	}
	else if (code->kind == CALL) {
		if (code->u.callop.result == NULL)
			fprintf(stream,  "tempnouse"); // place == NULL
		else printOperand(code->u.callop.result, stream);
		fprintf(stream, " := CALL %s\n", code->u.callop.funcname);
	}
	else if (code->kind == LABEL) {
		fprintf(stream, "LABEL ");
		printOperand(code->u.x, stream);
		fprintf(stream, " :\n");
	}
	else if (code->kind == GOTOOP) {
		fprintf(stream, "GOTO ");
		printOperand(code->u.gotoop.L, stream);
		fprintf(stream, "\n");
	}
	else if (code->kind == DEC) {
		fprintf(stream, "DEC ");
		printOperand(code->u.dec.op, stream);
		printOperand(code->u.dec.memory, stream);
		fprintf(stream, "\n");
	}
	else if (code->kind == PARAM) {
		fprintf(stream, "PARAM ");
		printOperand(code->u.x, stream);
		fprintf(stream, "\n");
	}
	else if (code->kind == CONDOP) {
		fprintf(stream, "IF ");
		printOperand(code->u.condop.op1, stream);
		fprintf(stream, " %s ", code->u.condop.op);
		printOperand(code->u.condop.op2, stream);
		fprintf(stream, " GOTO ");
		printOperand(code->u.condop.L, stream);
		fprintf(stream, "\n");
	}
	else if (code->kind == READ) {
		fprintf(stream, "READ ");
		if (code->u.x == NULL) {
		    fprintf(stream, "tread");
		}
		else printOperand(code->u.x, stream);
		fprintf(stream, "\n");
	}
	else if (code->kind == WRITE) {
		fprintf(stream, "WRITE ");
		printOperand(code->u.x, stream);
		fprintf(stream, "\n");
	}
	else if (code->kind == ARG) {
		fprintf(stream, "ARG ");
		printOperand(code->u.x, stream);
		fprintf(stream, "\n");
	}
	else if (code->kind == RETURN) {
		fprintf(stream, "RETURN ");
		printOperand(code->u.x, stream);
		fprintf(stream, "\n");
	}
}

void printOperand(Operand op, FILE *stream) {
	switch (op->kind) {
		case VARIABLE: fprintf(stream, "v%s", op->u.name); break;
		case CONSTANT: fprintf(stream, "#%d", op->u.value); break;
		case TMPVAR: fprintf(stream, "t%d", op->u.var_no); break;
		case FUNCTION: fprintf(stream, "%s", op->u.name); break;
		case ADDRESS: fprintf(stream, "label%d", op->u.var_no); break;
		case MEMORY: fprintf(stream, " %d", op->u.value); break;
		case ARRAYNAME: fprintf(stream, "&v%s", op->u.name); break;
		case ARRADDRESS: {if(op->star) fprintf(stream, "*t%d", op->u.var_no);
							else fprintf(stream, "t%d", op->u.var_no); } break;
	}
}
