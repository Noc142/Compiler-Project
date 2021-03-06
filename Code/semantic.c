#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantic.h"
#define IN_STRUCT 1
#define IN_PARAM 2
#define IN_FUNC 3
#define IN_EXTERN 4
#define TYPE_INT 0
#define TYPE_FLOAT 1
#define FUN_DEFINE 1
#define FUN_DECLARATION 0
int have_struct = 0;
int have_error = 0;

enum nonterminal {
	num_Program = 1001, num_ExtDefList, num_ExtDef, num_ExtDecList,
	num_Specifier, num_StructSpecifier, num_OptTag, num_Tag,
	num_VarDec, num_FunDec, num_VarList, num_ParamDec,
	num_CompSt, num_StmtList, num_Stmt,
	num_DefList, num_Def, num_DecList, num_Dec,
	num_Exp, num_Args
};


int hidNameInt = 1;
int hidNameFunc = 1;
Type type_int;//for return type of int constant;
Type type_float;
int checkType(Type tp1, Type tp2)
{
	//fprintf(stderr, "begin checkType\n");
	if (tp1 == NULL && tp2 == NULL) { return 1;}
	//else if (tp1 == NULL || tp2 == NULL) return 0; ????????????????????????????????????????
	//&&
	else if (tp1 == NULL || tp2 == NULL) { if ((tp1 != NULL && tp1->kind == ARRAY) || (tp2 != NULL && tp2->kind == ARRAY)) return 0; else return 1; }
	if (tp1->kind != tp2->kind) { return 0;}
	if (tp1->kind == BASIC)
	{
		return (int)(tp1->u.basic == tp2->u.basic);
	}
	else if (tp1->kind == ARRAY)
	{
		//if (tp1->u.array.size != tp2->u.array.size)return 0; int a[10][3] equals to int b[5][2];
		return checkType(tp1->u.array.elem, tp2->u.array.elem);
	}
	else
	{
		//return checkField(tp1->u.structure, tp2->u.structure);//or just check whether the names are same is ok?
		return (!strcmp(tp1->u.structure->name, tp2->u.structure->name));//????????????????????????????????
	}
	return 1;
}
int checkField(FieldList fl1, FieldList fl2)
{
	//fprintf(stderr, "begin checkfield\n");
	if (fl1 == NULL && fl2 == NULL) {return 1;}
	else if (fl1 == NULL || fl2 == NULL) {return 0;}
	if(checkType(fl1->type, fl2->type)==0){return 0;}
	if (fl1->tail != NULL && fl2->tail != NULL)
		return checkField(fl1->tail, fl2->tail);
	else if (fl1->tail == NULL && fl2->tail == NULL)
		return 1;
	else return 0;
	return 1;
}
void checkDefine(Symbol sym)
{
	if (sym == NULL) return;
	else {
		if (sym->isdefined == 0)
		{
			fprintf(stderr, "Error type 18 at Line %d: Undefined function \"%s\".\n", sym->line, sym->name);
		}
		checkDefine(sym->left);
		checkDefine(sym->right);
	}
}
void Program(Node *root)
{
	symtable = (SymTable*)malloc(sizeof(struct SymTable));
	type_int=  (Type)malloc(sizeof(struct Type_));
	type_float=(Type)malloc(sizeof(struct Type_));
	type_int->kind = BASIC; type_int->u.basic = 0;
	type_float->kind = BASIC; type_float->u.basic = 1;
	SymInsert_func(symtable, "read", type_int, NULL, 1, 0);
	SymInsert_func(symtable, "write", type_int, newSymbol_func_var("#write", type_int), 1, 0);
	ExtDefList(root->child);
	for (int i = 0; i < DEFAULT_HASH_SIZE; i++)
	{
		checkDefine(symtable->table_func[i]);
	}
}

void ExtDefList(Node* node)
{
	if (node->isEmpty)
		return;
	ExtDef(node->child);
	ExtDefList(node->child->next);
}
void ExtDef(Node* node)
{
	//fprintf(stderr, "ExtDef: %d\n", node->npro);
	if (node->npro == 2 && ((node->child->npro == 1) || (node->child->npro == 2 && node->child->child->npro == 2))) {
		return;
	}
	Type type = Specifier(node->child);
	switch (node->npro) {
		case 1:ExtDecList(node->child->next,type); break;
		case 2:/*if (type != NULL && type != type_int && type != type_float) free(type);*/ break;
		case 3:FunDec(node->child->next, type,FUN_DEFINE); CompSt(node->child->next->next, type); break;
		case 4:FunDec(node->child->next, type, FUN_DECLARATION); break;
	}
	return;
}
void FunDec(Node* node, Type type,int isDefine)
{
	FieldList field = NULL;
	if (isDefine)//this is a defination
	{	
		if (SymContains(symtable, node->child->val) != 0)
		{
			Symbol s = Find(symtable->table_func, node->child->val);
			if(s!=NULL&&s->isdefined==1)
				fprintf(stderr, "Error type 4 at Line %d: Redefined function: \"%s\".\n", node->child->lineno, node->child->val);
			else if (s != NULL) {//check if this defination is consistant with the former declaration
				if(node->npro==1) {
					field = VarList(node->child->next->next);
				}
				if (checkType(s->type, type) == 0 || checkField(s->tail, field) == 0)
				{
					
					fprintf(stderr, "Error type 19 at Line %d: Inconsistent declaration and defination of function \"%s\".\n", node->child->lineno, node->child->val);
				}
				s->isdefined = 1;
				s->tail = field;
			}
			
		}
		
		else { 
			if (node->npro == 1) field = VarList(node->child->next->next);
			//fprintf(stderr, "%d, %s, %s\n", type->kind, node->child->val, field->name);
		 	SymInsert_func(symtable, node->child->val, type, field, isDefine, node->child->lineno);
		}

	}
	else {//this is a declaration
		if (SymContains(symtable, node->child->val) != 0)
		{
			Symbol s = Find(symtable->table_func, node->child->val);
			if (s != NULL) {//check if this declaration is consistant with the former declaration or defination
				FieldList field1 = NULL;
				if(node->npro==1) {
					field1 = VarList_CheckFuncDec(node->child->next->next);
				}
				if (checkType(s->type, type) == 0 || checkField(s->tail, field1) == 0)
				{
					fprintf(stderr, "Error type 19 at Line %d: Inconsistent declaration and defination of function \"%s\".\n", node->child->lineno, node->child->val);
					return;
				}
			}
			else {
				if (node->npro == 1) {
					VarList_CheckFuncDec(node->child->next->next);//just check the var's type if illegal, dont do other things.
				}
			fprintf(stderr, "Error type 4 at Line %d: Redefined function \"%s\".\n", node->child->lineno, node->child->val);
			}
			
		}
		else {
			if(node->npro==1)
				field = VarList_CheckFuncDec(node->child->next->next);
			if (SymContains(symtable, node->child->val) == 0)//not contain the declaration
			{
				SymInsert_func(symtable, node->child->val, type, field, isDefine,node->child->lineno);
				//fprintf(stderr, "%s\n", field->name);
				//Symbol tmp = Find(symtable->table_func, node->child->val);
				//fprintf(stderr, "Hello, %s, %s, %s, %d, %s\n", tmp->name, tmp->tail->name, tmp->tail->tail->name, tmp->tail->tail->type->kind, tmp->tail->tail->type->u.structure->name);
			}
			else {//contains,check whether the declaration is consistant
				Symbol s = Find(symtable->table_func, node->child->val);
				if (checkType(s->type, type) == 0 || checkField(s->tail, field) == 0)
					fprintf(stderr, "Error type 19 at Line %d: Inconsistent declaration of function \"%s\".\n", node->child->lineno, node->child->val);
			}
		}
	}
}
FieldList VarList(Node* node)
{
	FieldList result = NULL;
	if (node->npro == 2)
	{
		result = ParamDec(node->child);
	}
	else {
		result = ParamDec(node->child);
		if(result!=NULL)
			result->tail = VarList(node->child->next->next);
	}
	return result;
}

FieldList ParamDec(Node* node)
{
	Type type=Specifier(node->child);
	return VarDec(node->child->next, type, IN_PARAM);
}
void CompSt(Node* node, Type type)
{
	DefList(node->child->next,IN_FUNC);
	StmtList(node->child->next->next,type);
}
void StmtList(Node* node,Type type)//this type is return type when calls a func
{
	if (node->isEmpty) return;
	Stmt(node->child, type);
	StmtList(node->child->next,type);
	
}
void Stmt(Node* node,Type type)
{
	//fprintf(stderr, "stmt: %d\n", node->npro);
	switch (node->npro)
	{
	case 1:Exp(node->child); break;
	case 2:CompSt(node->child,type); break;
	case 3: if(checkType(Exp(node->child->next), type)==0)
			{ fprintf(stderr, "Error type 8 at Line %d: Type mismatched for return.\n", node->child->next->lineno); }
			break;
	case 4:Exp(node->child->next->next); Stmt(node->child->next->next->next->next,type); break;
	case 5:Exp(node->child->next->next); Stmt(node->child->next->next->next->next,type); Stmt(node->child->next->next->next->next->next->next,type); break;
	case 6: Exp(node->child->next->next); Stmt(node->child->next->next->next->next,type); break;
	}
}
Type Exp(Node* node)
{
	//fprintf(stderr, "Exp: %d\n", node->npro);
	switch (node->npro)
	{
	case 1: { 

		Type tp1 = Exp(node->child); Type tp2 = Exp(node->child->next->next); 
		int hasfault = 0;
		//&&
		if (node->child->npro != 14 && node->child->npro != 15 && node->child->npro != 16) {
			fprintf(stderr, "Error type 6 at Line %d: The left-hand side of an assignment must be a variable.\n", node->child->lineno);
			hasfault = 1;
		}
		if (checkType(tp1, tp2) == 0) {
			fprintf(stderr, "Error type 5 at Line %d: Type mismatched for assignment.\n", node->child->lineno);
			hasfault = 1;
		}
		if (hasfault) return NULL;
		else return tp1;
			}break;	
	case 2: {Type tp1 = Exp(node->child); Type tp2 = Exp(node->child->next->next); 
		if (checkType(tp1, type_int) == 0 || checkType(tp2, type_int) == 0) { fprintf(stderr, "Error type 7 at Line %d: Type mismatched for operands.\n", node->child->lineno); return NULL; }
		else return tp1;
			}break;
	case 3: {Type tp1 = Exp(node->child); Type tp2 = Exp(node->child->next->next);
		if (checkType(tp1, type_int) == 0 || checkType(tp2, type_int) == 0) { fprintf(stderr, "Error type 7 at Line %d: Type mismatched for operands.\n", node->child->lineno); return NULL; }
		else return tp1;
	}break;
	case 4: {Type tp1 = Exp(node->child); Type tp2 = Exp(node->child->next->next);
		if ((checkType(tp1, type_int) == 0 && checkType(tp1, type_float) == 0) || (checkType(tp2, type_int) == 0 && checkType(tp2, type_float) == 0))
		{
			fprintf(stderr, "Error type 7 at Line %d: Type mismatched for operands.\n", node->child->lineno); return NULL;
		}
		else return type_int;
			}break;

	case 9:return Exp(node->child->next); break;
	case 10: {Type tp = Exp(node->child->next); if (checkType(tp, type_int) == 0 && checkType(tp, type_float) == 0)
			{
				fprintf(stderr, "Error type 7 at Line %d: Type mismatched for operands.\n", node->child->lineno); return NULL;
			}
		   else return tp; }break;
	case 11: {Type tp1 = Exp(node->child->next); 
		if (checkType(tp1, type_int) == 0) { fprintf(stderr, "Error type 7 at Line %d: Type mismatched for operands.\n", node->child->lineno); return NULL; }
		else return tp1;
	}break;
	case 12: {
		if (SymContains(symtable, node->child->val) == 0) { fprintf(stderr, "Error type 2 at Line %d: Undefined function \"%s\".\n", node->child->lineno, node->child->val); return NULL; }
		Symbol s = Find(symtable->table_func, node->child->val);
		if (s == NULL) { fprintf(stderr, "Error type 11 at Line %d: \"%s\" is not a function.\n", node->child->lineno, node->child->val); return NULL; }
		if (checkField(s->tail, Args(node->child->next->next)) == 0)
		{
			fprintf(stderr, "Error type 9 at Line %d: Function \"%s\" is not applicable for arguments that you write.\n", node->child->lineno, node->child->val); 
			return NULL;//ange better if have time 
		}
		else {return s->type;}
			}break;
	case 13: {
		if(SymContains(symtable,node->child->val)==0) { fprintf(stderr, "Error type 2 at Line %d: Undefined function \"%s\".\n", node->child->lineno, node->child->val); return NULL; }
		Symbol s = Find(symtable->table_func, node->child->val);
		if (s == NULL) { fprintf(stderr, "Error type 11 at Line %d: \"%s\" is not a function.\n", node->child->lineno, node->child->val); return NULL; }
		if(s->tail!=NULL) {
			fprintf(stderr, "Error type 9 at Line %d: Function \"%s\" is not applicable for arguments that you write.\n", node->child->lineno, node->child->val); return NULL;//ange better if have time 
		}
		else return s->type;
			}break;

		//*************************************************************************************************************************
	case 14: {Type tp1 = Exp(node->child); Type tp2 = Exp(node->child->next->next); 
		
		if (tp1 != NULL && tp1->kind != ARRAY) { fprintf(stderr, "Error type 10 at Line %d: It is not an array.\n", node->child->lineno);/*output can be better*/ return NULL; }
		if (checkType(tp2, type_int) == 0) {
			fprintf(stderr, "Error type 12 at Line %d: Index is not an integer.\n", node->child->next->next->lineno);/*output can be better*/ return NULL;
		}
		else { if (tp1 == NULL) return NULL; else return tp1->u.array.elem; }//is this corrent??????????
	}
		break;
		//*****************************************************************************************************************************
	case 15: {Type tp1 = Exp(node->child); if (tp1 == NULL) return NULL; if (tp1->kind != STRUCTURE) { fprintf(stderr, "Error type 13 at Line %d: Illegal use of \".\".\n", node->child->lineno); return NULL; }
		   if (fieldContain(tp1->u.structure, node->child->next->next->val) == 0) {fprintf(stderr, "Error type 14 at Line %d: Non-existent field \"%s\".\n", node->child->next->next->lineno, node->child->next->next->val); return NULL; };
		   Symbol s = Find(symtable->table_struct_var, node->child->next->next->val);
		   if (s == NULL) { fprintf(stderr, "Error type 14 at Line %d: Non-existent field \"%s\".\n", node->child->next->next->lineno, node->child->next->next->val); return NULL; }
		   else return s->type; }break;



	case 16: {Symbol s1 = Find(symtable->table_var, node->child->val), s2 = Find(symtable->table_func_var, node->child->val);
		if (s1 == NULL && s2 == NULL) { fprintf(stderr, "Error type 1 at Line %d: Undefined variable \"%s\".\n", node->child->lineno, node->child->val); return NULL; }
		else if (s1 != NULL) return s1->type;
		else return s2->type;
	break;}
	case 17: return type_int; break;
	case 18:return type_float; break;
	default: {Type tp1 = Exp(node->child); Type tp2 = Exp(node->child->next->next);
		if (checkType(tp1, tp2) == 0||(checkType(tp1,type_int)==0&&checkType(tp1,type_float)==0)){
			fprintf(stderr, "Error type 7 at Line %d: Type mismatched for operands.\n", node->child->lineno);
			return NULL;
		}
		else return tp1; }break;
	}
}
FieldList Args(Node* node)
{
	FieldList result=(FieldList)malloc(sizeof(struct FieldList_));;
	if (node->npro == 2) {
		result->type= Exp(node->child);
	}
	else {
		result->type=Exp(node->child);
		result->tail = Args(node->child->next->next);
	}
	return result;
}
Type Specifier(Node* node)
{
	Type type=NULL;
	if (node->npro == 1)
	{
		
		if (strcmp(node->child->val, "int") == 0)
		{
			type = type_int;
		}
		else type = type_float;
	}
	else {
		type = StructSpecifier(node->child);
	}
	return type;
}
Type StructSpecifier(Node* node)
{
	have_struct = 1;
	if (node->child->next->type==num_OptTag)
	{
		Type type = (Type)malloc(sizeof(struct Type_));
		type->kind = STRUCTURE;
		type->u.structure= (FieldList)malloc(sizeof(struct FieldList_));
		if (node->child->next->isEmpty) {
			sprintf(type->u.structure->name, "%d", hidNameInt);
			hidNameInt++;
		}
		else strcpy(type->u.structure->name , node->child->next->child->val);
		type->u.structure->tail = DefList(node->child->next->next->next,IN_STRUCT);
		if (!node->child->next->isEmpty)
		{
			if(SymContains(symtable, type->u.structure->name)!=0)
				fprintf(stderr, "Error type 16 at Line %d: Duplicated name \"%s\".\n", node->child->next->lineno, type->u.structure->name);
			else {
				SymInsert_struct(symtable, type->u.structure->name, type->u.structure->tail);
			}
		}
		return type;
	}
	else {//STRUCT TAG
		Symbol sym = Find(symtable->table_struct, node->child->next->child->val);
		if (sym == NULL)
		{
			fprintf(stderr, "Error type 17 at Line %d: Undefined structure \"%s\".\n",node->child->next->lineno, node->child->next->child->val);
			return NULL;
		}
		Type type= (Type)malloc(sizeof(struct Type_));
		type->kind = STRUCTURE;
		type->u.structure = sym;
		return type;
	}
	
}

void ExtDecList(Node* node,Type type)
{
	VarDec(node->child,type,IN_EXTERN);
	if (node->npro == 2)
		ExtDecList(node->child->next->next, type);
}
FieldList DefList(Node* node, int inwhere)
{
	if (node->isEmpty)
		return NULL;
	FieldList result = NULL;
	if (inwhere == IN_STRUCT)
	{
		result=Def(node->child, inwhere);
		//fprintf(stderr, "End DefList\n");
		FieldList cur = result;
		FieldList ret2 = DefList(node->child->next, inwhere);
		if (cur != NULL) {
			while (cur->tail != NULL)
				cur = cur->tail;
			cur->tail=ret2;
		}
	}
	else {//in func
		Def(node->child, inwhere);
		DefList(node->child->next, inwhere);
	}
	/*if(result->tail!=NULL)
		if(result->tail->tail!=NULL)
			if(result->tail->tail->tail!=NULL)
			if(result->tail->tail->tail->tail!=NULL)
				fprintf(stderr, "%d %d %d %d %d",result->type->u.basic,result->tail->type->u.basic,result->tail->tail->type->u.basic,result->tail->tail->tail->type->u.basic,result->tail->tail->tail->tail->type->u.basic);*/
	return result;
}
FieldList Def(Node* node,  int inwhere)
{
	Type type = Specifier(node->child);
	FieldList result = NULL;
	if (inwhere == IN_STRUCT)
	{
		result= DecList(node->child->next, type, inwhere);
		//fprintf(stderr, "End Def %d\n", (result == NULL));
	}
	else {//in func
		DecList(node->child->next, type, inwhere);
	}
	return result;
}
FieldList DecList(Node* node, Type type, int inwhere)
{
	FieldList result = NULL;
	if (node->npro == 1)
	{
		if (inwhere == IN_STRUCT)
			result = Dec(node->child, type, inwhere);
		else {
			Dec(node->child, type, inwhere);
		}
		//fprintf(stderr, "End DecList\n");
	}
	else {
		if (inwhere == IN_STRUCT)
		{
			result = Dec(node->child, type, inwhere);
			if(result!=NULL)
				result->tail = DecList(node->child->next->next, type, inwhere);
		}
		else {
			Dec(node->child, type, inwhere);
			DecList(node->child->next->next, type, inwhere);
		}
	}
	return result;
}

FieldList Dec(Node* node, Type type, int inwhere)
{
	
	if (node->npro == 1)
	{
		return VarDec(node->child, type, inwhere);
	}
	else {
		//VarDec ASSIGNOP Exp 
		if(inwhere==IN_STRUCT)
			fprintf(stderr, "Error type 15 at Line %d:initializing in struct defination is not allowed.\n", node->child->lineno);
		//fprintf(stderr, "FUCKYOU");
		FieldList fl1 = VarDec(node->child, type, inwhere);
		Type tp2 = Exp(node->child->next->next);
		if (fl1 == NULL) return NULL;
		if (!checkType(fl1->type, tp2)) {
			fprintf(stderr, "Error type 5 at Line %d: Type mismatched for assignment.\n", node->child->lineno);
		}
		return fl1;
	}
}


FieldList VarDec(Node* node, Type type,int inwhere)
{
	FieldList field= (FieldList)malloc(sizeof(struct FieldList_));
	if (inwhere == IN_EXTERN || inwhere == IN_FUNC)
	{
		if (node->npro == 1)//ID
		{
			if (SymInsert_var(symtable, node->child->val, type) == 0)//insert failed
			{
				fprintf(stderr, "Error type 3 at Line %d: Redefined variable \"%s\".\n", node->child->lineno, node->child->val);
				return NULL;
			}
			else {
				return (FieldList)Find(symtable->table_var, node->child->val);
			}
		}
		else {//VarDec LB INT RB :a[10]
			//&&*********************************************************************************************************
			Type ntp=  (Type)malloc(sizeof(struct Type_));
			ntp->kind = ARRAY;
			ntp->u.array.elem = type; ntp->u.array.size = atoi(node->child->next->next->val);
			return VarDec(node->child, ntp, inwhere);
			//**************************************************************************************************************
		}
	}
	else if (inwhere == IN_STRUCT)
	{
		if (node->npro == 1)//ID
		{
			if (SymInsert_struct_var(symtable, node->child->val, type) == 0)//insert failed
			{
				fprintf(stderr, "Error type 15 at Line %d: Redefined field \"%s\".\n", node->child->lineno, node->child->val);
				return NULL;
			}
			else {
				return (FieldList)Find(symtable->table_struct_var, node->child->val);
			}
		}
		else {//VarDec LB INT RB
			Type ntp=  (Type)malloc(sizeof(struct Type_));
			ntp->kind = ARRAY;
			ntp->u.array.elem = type; ntp->u.array.size = atoi(node->child->next->next->val);		
			return VarDec(node->child, ntp, inwhere);
		}
	}
	else if (inwhere == IN_PARAM)
	{
		if (node->npro == 1)//ID
		{
			
			if (SymInsert_func_var(symtable, node->child->val, type) == 0)//insert failed
			{
				fprintf(stderr, "Error type 3 at Line %d: Redefined variable \"%s\".\n", node->child->lineno, node->child->val);
				return NULL;
			}
			else {
				return (FieldList)Find(symtable->table_func_var, node->child->val);
			}
		}
		else //VarDec LB INT RB//&&
		{
			Type ntp=  (Type)malloc(sizeof(struct Type_));
			ntp->kind = ARRAY;
			ntp->u.array.elem = type; ntp->u.array.size = atoi(node->child->next->next->val);		
			return VarDec(node->child, ntp, inwhere);
		}
	}
}

FieldList VarList_CheckFuncDec(Node* node)
{
	FieldList result = NULL;
	if (node->npro == 2)
	{
		result = ParamDec_CheckFuncDec(node->child);
	}
	else {
		result = ParamDec_CheckFuncDec(node->child);
		if(result!=NULL)
			result->tail = VarList_CheckFuncDec(node->child->next->next);
	}
	return result;
}

FieldList ParamDec_CheckFuncDec(Node* node)
{
	Type type=Specifier(node->child);
	return VarDec_CheckFuncDec(node->child->next, type, IN_PARAM);
}
FieldList VarDec_CheckFuncDec(Node* node, Type type,int inwhere)
{
	if (inwhere == IN_PARAM)
	{
		if (node->npro == 1)//ID
		{
			/*char tmp[48] = "\0";
			sprintf(tmp, "%d", hidNameFunc);
			hidNameFunc++;
			strcpy(tmp+strlen(tmp), node->child->val);*/
			//fprintf(stderr, "SAD");
			Symbol newNode = newSymbol_func_var(node->child->val, type);
			return newNode;
		}
		else //VarDec LB INT RB//&&
		{
			Type ntp=  (Type)malloc(sizeof(struct Type_));
			ntp->kind = ARRAY;
			ntp->u.array.elem = type; ntp->u.array.size = atoi(node->child->next->next->val);		
			return VarDec_CheckFuncDec(node->child, ntp, inwhere);
		}
	}
}
int fieldContain(FieldList fl, const char* name)
{
	if (fl == NULL) return 0;
	
	if (strcmp(fl->name, name) == 0) { return 1; }
	else return fieldContain(fl->tail, name);
}



