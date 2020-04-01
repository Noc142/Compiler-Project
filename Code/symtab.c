#include "symtab.h"
#include <string.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

void initBSTNode(BSTNode *node, char k[], int tp, struct BSTNode *l, struct BSTNode *r) {
	memset(node->key, 0, STRLEN);
	strncpy(node->key, k, strlen(k));
	node->type = tp;
	node->left = l;
	node->right = r;
}
/*Insert a symbol to this tree*/
int Insert(BSTNode *ptr, char k[], int tp) {
	BSTNode *cur = NULL, *p = NULL;
	if (ptr == NULL) {
		ptr = malloc(sizeof(struct BSTNode));
		initBSTNode(ptr, k, tp, NULL, NULL);
		return 1;
	}
	cur = ptr;
	while (cur != NULL) {
		p = cur;
		if (strcmp(k, cur->key) == 0) return 0;
		else if (strcmp(k, cur->key) < 0) cur = cur->left;
		else cur = cur->right;
	}
	cur = malloc(sizeof(struct BSTNode));
	if (strcmp(k, p->key) < 0) {
		p->left = cur;
	}
	else p->right = cur;
	return 1;
}
/*Judge if Symbol k in the tree*/
int Contains(BSTNode *ptr, char k[]) {
	BSTNode *cur = ptr;
	while (cur != NULL) {
		if (strcmp(k, cur->key) == 0) return 1;
		else if (strcmp(k, cur->key) < 0) cur = cur->left;
		else cur = cur->right;
	}
	return 0;
}
void delete_BST(BSTNode *ptr) {
	if (ptr != NULL) {
		delete_BST(ptr->left);
		delete_BST(ptr->right);
		free(ptr);
	}
}
int HashContains(struct HashTable table, char k[]) {
	int lenk = strlen(k);
	unsigned int code = 0;
	for (int i = 0; i < lenk; ++i) {
		code *= 65599 * code + (unsigned int)k[i];
	}
	code %= DEFAULT_HASH_SIZE;
	return Contains(table.table[code], k);
}
int HashInsert(struct HashTable table, char k[], int tp) {
	int lenk = strlen(k);
	unsigned int code = 0;
	for (int i = 0; i < lenk; ++i) {
		code *= 65599 * code + (unsigned int)k[i];
	}
	code %= DEFAULT_HASH_SIZE;
	return Insert(table.table[code], k, tp);
}
