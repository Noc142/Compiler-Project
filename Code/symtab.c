#include "symtab.h"
#include <string.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

void initSymTable(SymTable *symtab) {
	memset(symtab->table_var, 0, DEFAULT_HASH_SIZE * sizeof(Symbol));
	memset(symtab->table_func, 0, DEFAULT_HASH_SIZE * sizeof(Symbol));
	memset(symtab->table_struct, 0, DEFAULT_HASH_SIZE * sizeof(Symbol));
	memset(symtab->table_struct_var, 0, DEFAULT_HASH_SIZE * sizeof(Symbol));
}

Symbol newSymbol_var(char k[], Type tp) {
	Symbol retsym = malloc(sizeof(struct FieldList_));
	strcpy(retsym->name, k);
	retsym->which = _VAR;
	retsym->isdefined = 1;
	retsym->type = tp;
	retsym->tail = NULL;
	retsym->left = NULL;
	retsym->right = NULL;
	return retsym;
}
Symbol newSymbol_struct_var(char k[], Type tp) {
	Symbol retsym = malloc(sizeof(struct FieldList_));
	strcpy(retsym->name, k);
	retsym->which = _STRUCT_VAR;
	retsym->isdefined = 1;
	retsym->type = tp;
	retsym->tail = NULL;
	retsym->left = NULL;
	retsym->right = NULL;
	return retsym;
}
Symbol newSymbol_func(char k[], Type tp, FieldList fl, int isdef) {
	Symbol retsym = malloc(sizeof(struct FieldList_));
	strcpy(retsym->name, k);
	retsym->which = _FUNC;
	retsym->isdefined = isdef;
	retsym->type = tp;
	retsym->tail = fl;
	retsym->left = NULL;
	retsym->right = NULL;
	return retsym;
}
Symbol newSymbol_struct(char k[], FieldList fl) {
	Symbol retsym = malloc(sizeof(struct FieldList_));
	strcpy(retsym->name, k);
	retsym->which = _STRUCT;
	retsym->isdefined = 1;
	retsym->type = NULL;
	retsym->tail = fl;
	retsym->left = NULL;
	retsym->right = NULL;
	return retsym;
}
/*Insert a symbol to this tree*/
int BSTInsert_var(Symbol *ptr, char k[], Type tp) {
	Symbol cur = NULL, p = NULL;
	if (*ptr == NULL) {
		*ptr = newSymbol_var(k, tp);
		return 1;
	}
	cur = *ptr;
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
int BSTInsert_func(Symbol *ptr, char k[], Type tp, FieldList fl, int isdef) {
	Symbol cur = NULL, p = NULL;
	if (*ptr == NULL) {
		*ptr = newSymbol_func(k, tp, fl, isdef);
		return 1;
	}
	cur = *ptr;
	while (cur != NULL) {
		p = cur;
		if (strcmp(k, cur->name) == 0) return 0;
		else if (strcmp(k, cur->name) < 0) cur = cur->left;
		else cur = cur->right;
	}
	cur = newSymbol_func(k, tp, fl, isdef);
	if (strcmp(k, p->name) < 0) {
		p->left = cur;
	}
	else p->right = cur;
	return 1;
}
int BSTInsert_struct(Symbol *ptr, char k[], FieldList fl) {
	Symbol cur = NULL, p = NULL;
	if (*ptr == NULL) {
		*ptr = newSymbol_struct(k, fl);
		return 1;
	}
	cur = *ptr;
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
	int i;
	for (i = 0; i < lenk; ++i) {
		code = 65599 * code + (unsigned int)k[i];
	}
	code %= DEFAULT_HASH_SIZE;
	return code;
}
int SymContains(struct SymTable *symtab, char k[]) {
	unsigned int code = hashcode(k);
	if (BSTContains(symtab->table_var[code], k)) return 1;
	else if (BSTContains(symtab->table_func[code], k)) return 2;
	else if (BSTContains(symtab->table_struct[code], k)) return 3;
	else if (BSTContains(symtab->table_struct_var[code], k)) return 4;
	return 0;
}
Symbol Find(Symbol table[], char k[]) {
	unsigned int code = hashcode(k);
	Symbol cur = table[code];
	while (cur != NULL) {
		if (strcmp(k, cur->name) == 0) return cur;
		else if (strcmp(k, cur->name) < 0) cur = cur->left;
		else cur = cur->right;
	}
	return NULL;
}
int SymInsert_var(SymTable *symtab, char k[], Type tp) {
	unsigned int code = hashcode(k);
	if (BSTContains(symtab->table_func[code], k)) return 0;
	else if (BSTContains(symtab->table_struct[code], k)) return 0;
	else if (BSTContains(symtab->table_struct_var[code], k)) return 0;
	return BSTInsert_var(&(symtab->table_var[code]), k, tp);
}
int SymInsert_func(SymTable *symtab, char k[], Type tp, FieldList fl, int isdef) {
	unsigned int code = hashcode(k);
	if (BSTContains(symtab->table_var[code], k)) return 0;
	else if (BSTContains(symtab->table_struct[code], k)) return 0;
	else if (BSTContains(symtab->table_struct_var[code], k)) return 0;
	return BSTInsert_func(&(symtab->table_func[code]), k, tp, fl, isdef);
}
int SymInsert_struct(SymTable *symtab, char k[], FieldList fl) {
	unsigned int code = hashcode(k);
	if (BSTContains(symtab->table_var[code], k)) return 0;
	else if (BSTContains(symtab->table_func[code], k)) return 0;
	else if (BSTContains(symtab->table_struct_var[code], k)) return 0;
	return BSTInsert_struct(&(symtab->table_struct[code]), k, fl);
}
int SymInsert_struct_var(SymTable *symtab, char k[], Type tp) {
	unsigned int code = hashcode(k);
	if (BSTContains(symtab->table_var[code], k)) return 0;
	else if (BSTContains(symtab->table_func[code], k)) return 0;
	else if (BSTContains(symtab->table_struct[code], k)) return 0;
	return BSTInsert_var(&(symtab->table_struct_var[code]), k, tp);
}
