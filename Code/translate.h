#ifndef TRANSLATE_H
#define TRASNLATE_H

#include "syntaxtree.h"
#include "symtab.h"
#include "list.h"
#include "semantic.h"
extern struct SymTable* symtable;

void translate_Program(Node *root);
void translate_ExtDefList(Node* node);
void translate_ExtDef(Node* node);
void translate_FunDec(Node* node);
void translate_VarList(Node* node);
void translate_ParamDec(Node* node);
Operand translate_VarDec(Node *node,int from);
void translate_CompSt(Node* node);
void translate_StmtList(Node* node);
void translate_Stmt(Node* node);
void translate_Exp(Node* node, Operand place);
void translate_Cond(Node *node, Operand label_true, Operand label_false);
void translate_Args(Node *node, struct ArgNode **arg_list);
void translate_Dec(Node* node);
void translate_DecList(Node* node);
void translate_Def(Node* node);
void translate_DefList(Node* node);
int calc_memory(Type t);


#endif
