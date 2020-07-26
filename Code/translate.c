#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "translate.h"
int var_no = 1;
int label_no = 1;
int const_no = 1;
struct InterCodes* codes;
enum Relop { GREATER, LESS, GEQ, LEQ, EQ, NEQ };

int calc_memory(Type t)
{
	if (t!=NULL&&t->kind != ARRAY)
		return 4;
	else return t->u.array.size * calc_memory(t->u.array.elem);
}
void translate_Program(Node* root)
{
	codes = newCodeHead();
	translate_ExtDefList(root->child);
}

void translate_ExtDefList(Node* node)
{
	if (node->isEmpty)
		return;
	translate_ExtDef(node->child);
	translate_ExtDefList(node->child->next);
}
void translate_ExtDef(Node* node)
{
	if (node->npro == 2 && ((node->child->npro == 1) || (node->child->npro == 2 && node->child->child->npro == 2))) {
		return;
	}
	switch (node->npro) {
	case 3:translate_FunDec(node->child->next); translate_CompSt(node->child->next->next); break;	//Specifier FunDec CompSt
	}
	return;
}
void translate_FunDec(Node* node)
{
	InterCode code;
	makeCode(&code, FUNC, node->child->val);
	InsertCode(codes, code);
	if (node->npro == 1) {
		translate_VarList(node->child->next->next);
	}
}
void translate_VarList(Node* node)
{
	FieldList result = NULL;
	translate_ParamDec(node->child);
	if (node->npro == 1) {  //ParamDec COMMA VarList
		translate_VarList(node->child->next->next);
	}
}

void translate_ParamDec(Node* node)
{
	translate_VarDec(node->child->next, 2);
}

Operand translate_VarDec(Node* node, int from) {
	if (from == 2)//ParamDec->Specifier VarDec 
	{
		if (node->npro == 1) {	// ID
			InterCode code;
			code.kind = PARAM;
			Operand x = new_var(node->child->val);
			makeCode(&code, PARAM, x);
			InsertCode(codes, code);
		}
		else {	//VarDec LB INT RB
			translate_VarDec(node->child, from);
		}
	}
	else if (from == 0)//Dec->VarDec
	{
		if (node->npro == 1)//ID
		{
			Symbol sym = Find(symtable->table_var, node->child->val);
			if (sym != NULL && sym->type->kind == ARRAY)
			{
				InterCode code1;
				Operand var = new_var(node->child->val);
				Operand memory = new_memory(calc_memory(sym->type));
				makeCode(&code1, DEC, var, memory);
				InsertCode(codes, code1);
			}
		}
		else {//VarDec LB INT RB
			translate_VarDec(node->child, from);
		}
	}
	else {//Dec->VarDec ASSIGNOP Exp
		if (node->npro == 1)//ID
		{
			Operand res = new_var(node->child->val);
			return res;
		}
		else {
			//do nothing
		}
	}

}
void translate_Dec(Node* node)
{
	if (node->npro == 1)//Dec->VarDec
	{
		translate_VarDec(node->child, 0);
	}
	else {//Dec->VarDec ASSIGNOP Exp
		Operand x = translate_VarDec(node->child, 1);
		Operand t1;
		if (node->child->next->next->npro == 14)
			t1 = new_arrayaddress(&var_no, 0);
		else t1 = new_temp(&var_no);
		translate_Exp(node->child->next->next, t1);
		Operand t2 = copyop(t1);
		t2->star = 1;
		InterCode code1;
		makeCode(&code1, ASSIGN, x, t2);
		InsertCode(codes, code1);
	}
}
void translate_DecList(Node* node)
{
	translate_Dec(node->child);
	if (node->npro == 2)
		translate_DecList(node->child->next->next);
}
void translate_Def(Node* node)
{
	translate_DecList(node->child->next);
}
void translate_DefList(Node* node)
{
	if (node->npro == 1)
	{
		translate_Def(node->child);
		translate_DefList(node->child->next);
	}
}
void translate_CompSt(Node* node)
{
	translate_DefList(node->child->next);
	translate_StmtList(node->child->next->next);
}
void translate_StmtList(Node* node)
{
	if (node->isEmpty) return;
	translate_Stmt(node->child);
	translate_StmtList(node->child->next);
}
void translate_Stmt(Node* node)
{
	switch (node->npro)
	{
	case 1:translate_Exp(node->child, NULL); break;
	case 2:translate_CompSt(node->child); break;
	case 3: {
		Operand t1;
		if (node->child->next->npro == 14) t1 = new_arrayaddress(&var_no, 0);
		else t1 = new_temp(&var_no);
		translate_Exp(node->child->next, t1);
		Operand t2 = copyop(t1); t2->star = 1;
		InterCode code;
		makeCode(&code, RETURN, t2);
		InsertCode(codes, code);
	} break;
	case 4: { //IF LP Exp RP Stmt1
		Operand label1 = new_label(&label_no);
		Operand label2 = new_label(&label_no);
		translate_Cond(node->child->next->next, label1, label2);
		InterCode code1, code2;
		makeCode(&code1, LABEL, label1);
		makeCode(&code2, LABEL, label2);
		InsertCode(codes, code1);
		translate_Stmt(node->child->next->next->next->next);
		InsertCode(codes, code2);
	} break;
	case 5: { //IF LP Exp RP Stmt1 ELSE Stmt2
		Operand label1 = new_label(&label_no);
		Operand label2 = new_label(&label_no);
		Operand label3 = new_label(&label_no);
		InterCode code1, code2, code3, code4;
		//code1
		translate_Cond(node->child->next->next, label1, label2);
		//[LABEL label1]
		makeCode(&code1, LABEL, label1);
		InsertCode(codes, code1);

		Node* stmt1 = node->child->next->next->next->next;
		Node* stmt2 = stmt1->next->next;
		//code2
		translate_Stmt(stmt1);
		//[GOTO label3]
		makeCode(&code2, GOTOOP, label3);
		InsertCode(codes, code2);
		//[LABEL label2]
		makeCode(&code3, LABEL, label2);
		InsertCode(codes, code3);
		//code3
		translate_Stmt(stmt2);
		//[LABEL label3]
		makeCode(&code4, LABEL, label3);
		InsertCode(codes, code4);
	} break;
	case 6: { //WHILE LP Exp RP Stmt1
		Operand label1 = new_label(&label_no);
		Operand label2 = new_label(&label_no);
		Operand label3 = new_label(&label_no);
		InterCode code1, code2, code3, code4;
		//[LABEL label1]
		makeCode(&code1, LABEL, label1);
		InsertCode(codes, code1);
		//code1
		translate_Cond(node->child->next->next, label2, label3);
		//[LABLE label2]
		makeCode(&code2, LABEL, label2);
		InsertCode(codes, code2);
		//code2
		translate_Stmt(node->child->next->next->next->next);
		//[GOTO label1]
		makeCode(&code3, GOTOOP, label1);
		InsertCode(codes, code3);
		//[LABEL label3]
		makeCode(&code4, LABEL, label3);
		InsertCode(codes, code4);
	} break;
	}
}

void translate_Exp(Node* node, Operand place)
{
	if (node->npro == 1) { //Exp->Exp ASSIGNOP Exp
		if (node->child->npro == 16) { //	Exp1->ID
			Operand var = new_var(node->child->child->val);
			Operand t1;
			if (node->child->next->next->npro == 14)
				t1 = new_arrayaddress(&var_no, 0);
			else t1 = new_temp(&var_no);
			translate_Exp(node->child->next->next, t1);
			Operand t2 = copyop(t1);
			t2->star = 1;
			InterCode code1, code2;
			makeCode(&code1, ASSIGN, var, t2);
			InsertCode(codes, code1);
			if (place != NULL) {
				makeCode(&code2, ASSIGN, place, var);
				InsertCode(codes, code2);
			}
		}
		else if (node->child->npro == 14 && node->child->next->next->npro != 14) {//Exp1->Exp LB Exp RB

			Operand arrvalue = new_arrayaddress(&var_no, 0);
			Operand t1 = new_temp(&var_no);
			translate_Exp(node->child->next->next, t1);
			translate_Exp(node->child, arrvalue);
			InterCode code1, code2;
			Operand arrvalue2 = copyop(arrvalue);
			arrvalue2->star = 1;
			makeCode(&code1, ASSIGN, arrvalue2, t1);
			InsertCode(codes, code1);
			if (place != NULL) {
				makeCode(&code2, ASSIGN, place, t1);
				InsertCode(codes, code2);
			}
		}
		else if (node->child->npro == 14 && node->child->next->next->npro == 14) {//Exp1->Exp LB Exp RB

			Operand arrvalue = new_arrayaddress(&var_no, 0);
			Operand arrvalue2 = new_arrayaddress(&var_no, 0);
			translate_Exp(node->child->next->next, arrvalue2);
			translate_Exp(node->child, arrvalue);
			InterCode code1, code2, code3;
			Operand arrvalue3 = copyop(arrvalue);
			Operand arrvalue4 = copyop(arrvalue2);
			Operand t1 = new_temp(&var_no);
			arrvalue3->star = 1;
			arrvalue4->star = 1;
			makeCode(&code3, ASSIGN, t1, arrvalue4);
			InsertCode(codes, code3);
			makeCode(&code1, ASSIGN, arrvalue3, arrvalue4);
			InsertCode(codes, code1);
			if (place != NULL) {
				makeCode(&code2, ASSIGN, place, t1);
				InsertCode(codes, code2);
			}
		}

	}
	else if (node->npro >= 5 && node->npro <= 8) { // ADD SUB(PLUS, MINUS in tree) MUL(STAR) DIV Exp->Exp PLUS Exp
		Operand t1; Operand t2;
		if (node->child->npro == 14) t1 = new_arrayaddress(&var_no, 0);
		else t1 = new_temp(&var_no);
		if (node->child->next->next->npro == 14) t2 = new_arrayaddress(&var_no, 0);
		else t2 = new_temp(&var_no);
		translate_Exp(node->child, t1);
		translate_Exp(node->child->next->next, t2);
		Operand t3 = copyop(t1); t3->star = 1;
		Operand t4 = copyop(t2); t4->star = 1;
		if (place != NULL) {
			InterCode code;
			int kind = node->npro - 5 + ADD; //sao caozuo
			makeCode(&code, kind, place, t3, t4);
			InsertCode(codes, code);
		}
	}
	else if (node->npro == 9) { //LP Exp RP
		//Operand t1 = new_temp(&var_no);
		translate_Exp(node->child->next, place);
		/*if (place != NULL) {
			InterCode code;
			makeCode(&code, ASSIGN, place, t1);
			InsertCode(codes, code);
		}*/
	}
	else if (node->npro == 10) {  //MINUS Exp
		Operand t1;
		if (node->child->next->npro == 14)
			t1 = new_arrayaddress(&var_no, 0);
		else
			t1 = new_temp(&var_no);
		translate_Exp(node->child->next, t1);
		InterCode code2;
		Operand constant = new_const(0);
		Operand t2 = copyop(t1);
		t2->star = 1;
		makeCode(&code2, SUB, place, constant, t2);
		InsertCode(codes, code2);
	}
	else if ((node->npro >= 2 && node->npro <= 4) || node->npro == 11) { // RELOP AND OR NOT
		Operand label1 = new_label(&label_no);
		Operand label2 = new_label(&label_no);
		if (place == NULL) return;
		InterCode code0, code1, code2, code3;
		//code0
		Operand constant0 = new_const(0);
		makeCode(&code0, ASSIGN, place, constant0);
		InsertCode(codes, code0);
		//code1
		translate_Cond(node, label1, label2);
		//code2
		makeCode(&code1, LABEL, label1);
		InsertCode(codes, code1);
		Operand constant1 = new_const(1);
		makeCode(&code2, ASSIGN, place, constant1);
		InsertCode(codes, code2);
		//[LABEL label2]
		makeCode(&code3, LABEL, label2);
		InsertCode(codes, code3);
	}
	else if (node->npro == 13) { //ID LP RP
		if (!strcmp(node->child->val, "read")) {
			InterCode code;
			makeCode(&code, READ, place);
			InsertCode(codes, code);
			return;
		}
		InterCode code;
		makeCode(&code, CALL, place, node->child->val);
		InsertCode(codes, code);
	}
	else if (node->npro == 12) { //ID LP Args RP
		ArgNode* arg_list = NULL;
		translate_Args(node->child->next->next, &arg_list);
		if (!strcmp(node->child->val, "write")) {
			InterCode code;
			makeCode(&code, WRITE, arg_list->op);
			InsertCode(codes, code);
			if (place != NULL) {
			    InterCode code1;
			    Operand constant = new_const(0);
			    makeCode(&code1, ASSIGN, place, constant);
			    InsertCode(codes, code1);
			}
			return;
		}
		else {
			ArgNode* cur = arg_list;
			while (cur != NULL) {
				InterCode code;
				makeCode(&code, ARG, cur->op);
				InsertCode(codes, code);
				cur = cur->next;
			}
			deleteArgList(arg_list);
		}
		InterCode code;
		makeCode(&code, CALL, place, node->child->val);
		InsertCode(codes, code);
	}
	else if (node->npro == 14) {//Exp LB Exp RB
		if (place == NULL)return;
		if (node->child->npro == 16)//the first Exp->ID
		{

			Symbol sym = Find(symtable->table_var, node->child->child->val);
			Symbol sym2 = Find(symtable->table_func_var, node->child->child->val);
			if (sym != NULL && sym->type->kind == ARRAY)
			{
				Operand var = new_array(node->child->child->val);
				Operand t1;
				if (node->child->next->next->npro == 14)
					t1 = new_arrayaddress(&var_no, 0);
				else t1 = new_temp(&var_no);
				translate_Exp(node->child->next->next, t1);
				Operand c1 = new_const(calc_memory(sym->type->u.array.elem));
				Operand t2 = new_temp(&var_no);
				InterCode code;
				Operand t3 = copyop(t1);
				t3->star = 1;
				makeCode(&code, MUL, t2, t3, c1);
				InsertCode(codes, code);
				InterCode code2;
				makeCode(&code2, ADD, place, var, t2);
				InsertCode(codes, code2);
			}
			else if (sym2 != NULL && sym2->type->kind == ARRAY)
			{
				Operand var = new_var(node->child->child->val);
				Operand t1;
				if (node->child->next->next->npro == 14)
					t1 = new_arrayaddress(&var_no, 0);
				else t1 = new_temp(&var_no);
				translate_Exp(node->child->next->next, t1);
				Operand c1 = new_const(calc_memory(sym2->type->u.array.elem));
				Operand t2 = new_temp(&var_no);
				InterCode code;
				Operand t3 = copyop(t1);
				t3->star = 1;
				makeCode(&code, MUL, t2, t3, c1);
				InsertCode(codes, code);
				InterCode code2;
				makeCode(&code2, ADD, place, var, t2);
				InsertCode(codes, code2);
			}

		}
		else {
			Operand t3 = new_temp(&var_no);
			Operand t5 = new_temp(&var_no);
			translate_Exp(node->child, t3);
			Type t2 = Exp(node->child)->u.array.elem;
			Operand c2 = new_const(calc_memory(t2));
			Operand t4;
			if (node->child->next->next->npro == 14)
				t4 = new_arrayaddress(&var_no, 0);
			else t4 = new_temp(&var_no);
			translate_Exp(node->child->next->next, t4);
			InterCode code3;
			Operand t7 = copyop(t4);
			t7->star = 1;
			makeCode(&code3, MUL, t5, t7, c2);
			InsertCode(codes, code3);
			Operand t6 = new_temp(&var_no);
			InterCode code4;
			makeCode(&code4, ADD, place, t5, t3);
			InsertCode(codes, code4);
		}

	}
	else if (node->npro == 16) { //ID
		if (place == NULL) return;
		Symbol sym = Find(symtable->table_var, node->child->val);
		if (sym != NULL && sym->type->kind == ARRAY)
		{
			Operand var = new_array(node->child->val);
			InterCode code;
			makeCode(&code, ASSIGN, place, var);
			InsertCode(codes, code);
		}
		else {
			Operand var = new_var(node->child->val);
			InterCode code;
			makeCode(&code, ASSIGN, place, var);
			InsertCode(codes, code);
		}
	}
	else if (node->npro == 17) { //INT
		InterCode code;
		Operand t1 = new_const(atoi(node->child->val));
		makeCode(&code, ASSIGN, place, t1);
		InsertCode(codes, code);
	}
}

void translate_Cond(Node* node, Operand label_true, Operand label_false) {
	if (node->npro == 4) { //Exp RELOP Exp
		Operand t1;
		Operand t2;
		if (node->child->npro == 14) t1 = new_arrayaddress(&var_no, 0);
		else t1 = new_temp(&var_no);
		if (node->child->next->next->npro == 14) t2 = new_arrayaddress(&var_no, 0);
		else t2 = new_temp(&var_no);
		translate_Exp(node->child, t1);//code1
		translate_Exp(node->child->next->next, t2);//code2
		Operand t3 = copyop(t1); t3->star = 1;
		Operand t4 = copyop(t2); t4->star = 1;
		InterCode code;
		makeCode(&code, CONDOP, label_true, t3, t4, node->child->next->val);
		//fprintf(stderr, "relop: %d\n", node->type);
		InsertCode(codes, code);
		//[GOTO label_false]
		InterCode code1;
		makeCode(&code1, GOTOOP, label_false);
		InsertCode(codes, code1);
	}
	else if (node->npro == 11) { // Not Exp
		translate_Cond(node->child->next, label_false, label_true);
	}
	else if (node->npro == 2) { //Exp1 AND Exp2
		Operand label1 = new_label(&label_no);
		translate_Cond(node->child, label1, label_false);
		InterCode code;
		makeCode(&code, LABEL, label1);
		InsertCode(codes, code);
		translate_Cond(node->child->next->next, label_true, label_false);
	}
	else if (node->npro == 3) { //Exp1 OR Exp2
		Operand label1 = new_label(&label_no);
		translate_Cond(node->child, label_true, label1);
		InterCode code;
		makeCode(&code, LABEL, label1);
		InsertCode(codes, code);
		translate_Cond(node->child->next->next, label_true, label_false);
	}
	else {
		Operand t1;
		if (node->npro == 14) t1 = new_arrayaddress(&var_no, 0);
		else t1 = new_temp(&var_no);
		translate_Exp(node, t1);
		Operand t2 = copyop(t1); t2->star = 1;
		InterCode code1, code2;
		Operand constant0 = new_const(0);
		char* tmp = (char*)malloc(4);
		strcpy(tmp, "!=");
		makeCode(&code1, CONDOP, label_true, t2, constant0, tmp);
		makeCode(&code2, GOTOOP, label_false);
		InsertCode(codes, code1);
		InsertCode(codes, code2);
	}
}
void translate_Args(Node* node, struct ArgNode** arg_list) {
	if (node->npro == 2) { //Exp
		Operand t1;
		if (node->child->npro == 14)
			t1 = new_arrayaddress(&var_no, 0);
		else
			t1 = new_temp(&var_no);

		translate_Exp(node->child, t1);
		Operand t2 = copyop(t1);
		Type t = Exp(node->child);
		if (t!=NULL&&t->kind != ARRAY)
			t2->star = 1;
		InsertArg(arg_list, t2);
	}
	else { // Exp COMMA Args1

		Operand t1;
		if (node->child->npro == 14)
			t1 = new_arrayaddress(&var_no, 0);
		else
			t1 = new_temp(&var_no);
		translate_Exp(node->child, t1);
		Operand t2 = copyop(t1);
		Type t = Exp(node->child);
		if (t!=NULL&&t->kind != ARRAY)
			t2->star = 1;
		InsertArg(arg_list, t2);
		translate_Args(node->child->next->next, arg_list);
	}
}

