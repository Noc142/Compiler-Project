#include "syntaxtree.h"
#include "symtab.h"

void Program(Node* node);
void ExtDefList(Node* node);
void ExtDef(Node* node);

Type Specifier(Node* node);
Type StructSpecifier(Node* node);
void ExtDecList(Node* node, Type type);
FieldList VarDec(Node* node, Type type, int inwhere);
FieldList Def(Node* node,  int inwhere);
FieldList DefList(Node* node, int inwhere);
FieldList Dec(Node* node, Type type, int inwhere);
FieldList DecList(Node* node, Type type, int inwhere);
void FunDec(Node* node, Type type, int isDefine);
int checkType(Type tp1, Type tp2);
int checkField(FieldList fl1, FieldList fl2);
void CompSt(Node* node, Type type);
FieldList VarList(Node* node);
FieldList ParamDec(Node* node);
void StmtList(Node* node, Type type);
void Stmt(Node* node, Type type);
Type Exp(Node* node);
FieldList Args(Node* node);
FieldList VarList_CheckFuncDec(Node* node, int* hasFoundInCons);
FieldList ParamDec_CheckFuncDec(Node* node, int* hasFoundInCons);
FieldList VarDec_CheckFuncDec(Node* node, Type type,int inwhere, int* hasFoundInCons);
