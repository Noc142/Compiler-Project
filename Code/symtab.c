#include "symtab.h"
#include <string.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

Symbol newSymbol_var(char k[], Type tp) {
	Symbol retsym = malloc(sizeof(Symbol));
	strcpy(retsym->name, k);
	retsym->which = _VAR;
	retsym->type = tp;
	retsym->tail = NULL;
	retsym->left = NULL;
	retsym->right = NULL;
	return retsym;
}
Symbol newSymbol_func(char k[], Type tp) {
	Symbol retsym = malloc(sizeof(Symbol));
	strcpy(retsym->name, k);
	retsym->which = _FUNC;
	retsym->type = tp;
	retsym->tail = NULL;
	retsym->left = NULL;
	retsym->right = NULL;
	return retsym;
}
Symbol newSymbol_struct(char k[], FieldList fl) {
	Symbol retsym = malloc(sizeof(Symbol));
	strcpy(retsym->name, k);
	retsym->which = _STRUCT;
	retsym->type = NULL;
	retsym->tail = fl;
	retsym->left = NULL;
	retsym->right = NULL;
	return retsym;
}
/*Insert a symbol to this tree*/
int BSTInsert_var(Symbol ptr, char k[], Type tp) {
	Symbol cur = NULL, p = NULL;
	if (ptr == NULL) {
		ptr = newSymbol_var(k, tp);
		return 1;
	}
	cur = ptr;
	while (cur != NULL) {
		p = cur;
		if (strcmp(k, cur->name) == 0) return 0;
		else if (strcmp(k, cur->name) < 0) cur = cur->left;
		else cur = cur->right;
	}
	cur = newSymbol_var(k, tp);
	if (strcmp(k, p->name) < 0) {
		p->left = cur;
	}
	else p->right = cur;
	return 1;
}
int BSTInsert_func(Symbol ptr, char k[], Type tp) {
	Symbol cur = NULL, p = NULL;
	if (ptr == NULL) {
		ptr = newSymbol_func(k, tp);
		return 1;
	}
	cur = ptr;
	while (cur != NULL) {
		p = cur;
		if (strcmp(k, cur->name) == 0) return 0;
		else if (strcmp(k, cur->name) < 0) cur = cur->left;
		else cur = cur->right;
	}
	cur = newSymbol_func(k, tp);
	if (strcmp(k, p->name) < 0) {
		p->left = cur;
	}
	else p->right = cur;
	return 1;
}
int BSTInsert_struct(Symbol ptr, char k[], FieldList fl) {
	Symbol cur = NULL, p = NULL;
	if (ptr == NULL) {
		ptr = newSymbol_struct(k, fl);
		return 1;
	}
	cur = ptr;
	while (cur != NULL) {
		p = cur;
		if (strcmp(k, cur->name) == 0) return 0;
		else if (strcmp(k, cur->name) < 0) cur = cur->left;
		else cur = cur->right;
	}
	cur = newSymbol_struct(k, fl);
	if (strcmp(k, p->name) < 0) {
		p->left = cur;
	}
	else p->right = cur;
	return 1;
}
/*Judge if Symbol k in the tree*/
int BSTContains(Symbol ptr, char k[]) {
	Symbol cur = ptr;
	while (cur != NULL) {
		if (strcmp(k, cur->name) == 0) return 1;
		else if (strcmp(k, cur->name) < 0) cur = cur->left;
		else cur = cur->right;
	}
	return 0;
}
void delete_BST(Symbol ptr) {
	if (ptr != NULL) {
		delete_BST(ptr->left);
		delete_BST(ptr->right);
		free(ptr);
	}
}
unsigned int hashcode(char k[]) {
	int lenk = strlen(k);
	unsigned int code = 0;
	for (int i = 0; i < lenk; ++i) {
		code *= 65599 * code + (unsigned int)k[i];
	}
	code %= DEFAULT_HASH_SIZE;
	return code;
}
int SymContains(struct SymTable table, char k[]) {
	unsigned int code = hashcode(k);
	if (BSTContains(table.table_var[code], k)) return 1;
	else if (BSTContains(table.table_func[code], k)) return 2;
	else if (BSTContains(table.table_struct[code], k)) return 3;
}
int SymInsert_var(Symbol table[], char k[], Type tp) {
	unsigned int code = hashcode(k);
	return BSTInsert_var(table[code], k, tp);
}
int SymInsert_func(Symbol table[], char k[], Type tp) {
	unsigned int code = hashcode(k);
	return BSTInsert_func(table[code], k, tp);
}
int SymInsert_struct(Symbol table[], char k[], FieldList fl) {
	unsigned int code = hashcode(k);
	return BSTInsert_struct(table[code], k, fl);
}
