#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "translate.h"
int var_no = 1;
int label_no = 1;
int const_no = 1;
struct InterCodes *codes;
enum Relop { GREATER, LESS, GEQ, LEQ, EQ, NEQ};

void translate_Program(Node *root)
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
	translate_VarDec(node->child->next);
}

void translate_VarDec(Node *node) {
	if (node->npro == 1) {	// ID
		InterCode code;
		code.kind = PARAM;
		Operand x = new_var(node->child->val);
		makeCode(&code, PARAM, x);
		InsertCode(codes, code);
	}
	else {	//VarDec LB INT RB
		translate_VarDec(node->child);
	}
}
void translate_CompSt(Node* node)
{
	/*No need to translate DefList*/
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
			Operand t = new_temp(&var_no);
			translate_Exp(node->child->next, t);
			InterCode code;
			makeCode(&code, RETURN, t);
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
			translate_Cond(node, label1, label2);
			//[LABEL label1]
			makeCode(&code1, LABEL, label1);
			InsertCode(codes, code1);
			
			Node *stmt1 = node->child->next->next->next->next;
			Node *stmt2 = stmt1->next->next;
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
			translate_Cond(node, label2, label3);
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
	if (node->npro == 1) { //ASSIGN
		InterCode code;
		code.kind = ASSIGN;
		if (node->child->npro == 16) { //	Exp1->ID
			Operand var = new_var(node->child->child->val);
			Operand t1 = new_temp(&var_no);
			translate_Exp(node->child->next->next, t1);
			InterCode code1, code2;
			makeCode(&code1, ASSIGN, var, t1);
			InsertCode(codes, code1);
			if (place != NULL) {
				makeCode(&code2, ASSIGN, place, var);
				InsertCode(codes, code2);
			}		
		}
	}
	else if (node->npro >= 5 && node->npro <= 8) { // ADD SUB(PLUS, MINUS in tree) MUL(STAR) DIV
		Operand t1 = new_temp(&var_no);
		Operand t2 = new_temp(&var_no);
		translate_Exp(node->child, t1);
		translate_Exp(node->child->next->next, t2);
		if (place != NULL) {
			InterCode code;
			int kind = node->npro - 5 + ADD; //sao caozuo
			makeCode(&code, kind, place, t1, t2);
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
		Operand t1 = new_temp(&var_no);
		translate_Exp(node->child->next, t1);
		InterCode code2;
		Operand constant = new_const(0);
		makeCode(&code2, SUB, place, constant, t1);
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
		makeCode(&code, CALL, node->child->val);
		InsertCode(codes, code);
	}
	else if (node->npro == 12) { //ID LP Args RP
		ArgNode *arg_list = NULL;
		translate_Args(node->child->next->next, &arg_list);
		if (!strcmp(node->child->val, "write")) {
			InterCode code;
			makeCode(&code, WRITE, arg_list->op);
			InsertCode(codes, code);
			return;
		}
		else {
			ArgNode *cur = arg_list;
			while (cur != NULL) {
				InterCode code;
				makeCode(&code, ARG, cur->op);
				InsertCode(codes, code);
				cur = cur->next;
			}
		}
		InterCode code;
		makeCode(&code, CALL, place, node->child->val);
		InsertCode(codes, code);
	}
	else if (node->npro == 16) { //ID
		if (place == NULL) return;
		Operand var = new_var(node->child->val);
		InterCode code;
		makeCode(&code, ASSIGN, place, var);
		InsertCode(codes, code);
	}
	else if (node->npro == 17) { //INT
		InterCode code;
		Operand t1 = new_const(atoi(node->child->val));
		makeCode(&code, ASSIGN, place, t1);
		InsertCode(codes, code);
	}
}

void translate_Cond(Node *node, Operand label_true, Operand label_false) {
	if (node->npro == 4) { //Exp RELOP Exp
		Operand t1 = new_temp(&var_no);
		Operand t2 = new_temp(&var_no);
		translate_Exp(node->child, t1);//code1
		translate_Exp(node->child->next->next, t2);//code2
		InterCode code;
		makeCode(&code, CONDOP, label_true, t1, t2, node->child->next->val);
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
		Operand t1 = new_temp(&var_no);
		translate_Exp(node, t1);
		InterCode code1, code2;
		Operand constant0 = new_const(0);
		char*tmp = (char *)malloc(4);
		strcpy(tmp, "!=");
		makeCode(&code1, CONDOP, label_true, t1, constant0, tmp);
		makeCode(&code2, GOTOOP, label_false);
		InsertCode(codes, code1);
		InsertCode(codes, code2);
	}
}
void translate_Args(Node *node, struct ArgNode **arg_list) {
	if (node->npro == 2) { //Exp
		Operand t1 = new_temp(&var_no);
		translate_Exp(node->child, t1);
		InsertArg(arg_list, t1);
	}
	else { // Exp COMMA Args1
		Operand t1 = new_temp(&var_no);
		translate_Exp(node->child, t1);
		InsertArg(arg_list, t1);
		translate_Args(node->child->next->next, arg_list);
	}
}

