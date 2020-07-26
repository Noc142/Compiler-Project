#include "object.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
struct VarDescripters *VDshead = NULL;
struct RegDescripter RDT[NUM_REG];
int Offset = -4;// the first offset should be -4
int curReg = T0;
int num_reg_available = 18;
int param_index = 0;
int paramOffset = 8;
int argOffset = 8;



char* Regs[] = {
    "$zero",
    "$at",
    "$v0", "$v1",
    "$a0", "$a1", "$a2", "$a3",
    "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", 
    "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", 
    "$t8", "$t9", 
    "$k0", "$k1",
    "$gp", "$sp",
    "$fp",
    "$ra" 
};

void initRDT() {
    int i;
    for (i = 0; i < NUM_REG; ++i) {
        RDT[i].flag = 0;// store no variable
        RDT[i].vds = NULL;
        RDT[i].offset = -1;
    }
}

void refreshRDT(FILE *stream) {
    int i;
    for (i = 0; i < NUM_REG; ++i) {
        if (RDT[i].offset != -1) { // only when s0-s7 are used
            fprintf(stream, "  lw %s, %d($fp)\n", Regs[i], RDT[i].offset);
            RDT[i].offset = -1;
        }
        RDT[i].flag = 0;
        RDT[i].vds = NULL;
        RDT[i].offset = -1;
    }
}

struct VarDescripters * newVDsHead() {
	struct VarDescripters * head = (struct VarDescripters *) malloc(sizeof(struct VarDescripters));
	head->prev = head;
	head->next = head;
	return head;
}

void makeVD(VarDescripter* vd, int kind, Operand op, int reg, int offset) {
    vd->kind = kind;
    vd->op = op;
    vd->reg = reg;
    vd->offset = offset;
}

VarDescripters* InsertVD(struct VarDescripter vd) {
    struct VarDescripters *newvds = (struct VarDescripters*) malloc(sizeof(struct VarDescripters));
	newvds->next = VDshead;
	newvds->prev = VDshead->prev;
	newvds->prev->next = newvds;
	VDshead->prev = newvds;
	newvds->vd = vd;
	return newvds;
}

void clearVDT() {
    VDs *cur = VDshead->next;
    while (cur != VDshead) {
        VDs* tmp = cur;
        cur = cur->next;
        free(tmp);
    }
    VDshead->next = VDshead;
    VDshead->prev = VDshead;
}

VarDescripters* findVDs(Operand op) {
    VarDescripters *cur = VDshead->next;
    while (cur != VDshead) {
        Operand curop = cur->vd.op;
        if ((op->kind == TMPVAR || op->kind == ARRADDRESS) && (curop->kind == TMPVAR || curop->kind == ARRADDRESS)) {
            //fprintf(stderr, "op->u.varno: %d %d\n", op->u.var_no, curop->u.var_no);
            if (op->u.var_no == curop->u.var_no)
                return cur;
        }
        else if ((op->kind == VARIABLE || op->kind == ARRAYNAME) && (curop->kind == VARIABLE || curop->kind == ARRAYNAME))
        {
            if (!strcmp(op->u.name, curop->u.name)) {
                return cur;
            }
        }
        cur = cur->next;
    }
    return NULL;
}

VarDescripters* InsertOptoVDs(Operand op) {
    if (op->kind == CONSTANT || op->kind == MEMORY) return NULL;
    VarDescripters* find_res = findVDs(op);
    if (find_res == NULL) {
        VarDescripter vd1;
        makeVD(&vd1, COMMONVAR, op, -1, Offset);
        InsertVD(vd1);
        Offset -= 4;
    }
}

void allocSpace(FILE *stream, struct InterCode *code) {
    if (code->kind == RETURN) {
        InsertOptoVDs(code->u.x);
    }
    else if (code->kind == ASSIGN) {
        InsertOptoVDs(code->u.assign.left);
        InsertOptoVDs(code->u.assign.right);
    }
    else if (code->kind == DEC) {
        printDec(stream, code);
    }
    else if (code->kind == PARAM) {
        printParam(stream, code);
    }
    else if (code->kind == CALL) {
        if (code->u.callop.result == NULL) return;
        InsertOptoVDs(code->u.callop.result);
    }
    else if (code->kind == READ) {
        if (code->u.x == NULL) return;
        InsertOptoVDs(code->u.x);
    }
    else if (code->kind == WRITE) {
        InsertOptoVDs(code->u.x);
    }
    else if (code->kind == ADD) {
        InsertOptoVDs(code->u.binop.result);
        InsertOptoVDs(code->u.binop.op1);
        InsertOptoVDs(code->u.binop.op2);
    }
    else if (code->kind == SUB) {
        InsertOptoVDs(code->u.binop.result);
        InsertOptoVDs(code->u.binop.op1);
        InsertOptoVDs(code->u.binop.op2);
    }
    else if (code->kind == MUL) {
        InsertOptoVDs(code->u.binop.result);
        InsertOptoVDs(code->u.binop.op1);
        InsertOptoVDs(code->u.binop.op2);
    }
    else if (code->kind == DIV) {
        InsertOptoVDs(code->u.binop.result);
        InsertOptoVDs(code->u.binop.op1);
        InsertOptoVDs(code->u.binop.op2);
    }
    else if (code->kind == CONDOP) {
        InsertOptoVDs(code->u.condop.op1);
        InsertOptoVDs(code->u.condop.op2);
    }
}

int getReg(Operand op, FILE *stream) {
    int retReg;
    if (op->kind == CONSTANT) {
        fprintf(stream, "  li %s, %d\n", Regs[curReg], op->u.value);
    }
    else {
        VarDescripters* find_res = findVDs(op);     
        if (find_res == NULL) {
            VarDescripter vd1;
            makeVD(&vd1, COMMONVAR, op, curReg, Offset);
            find_res = InsertVD(vd1);
            fprintf(stream, "  addi $sp, $sp, -4\n");
            Offset -= 4;
        }
        if (op->kind == TMPVAR || op->kind == ARRADDRESS) {
            fprintf(stream, "  lw %s, %d($fp)\n", Regs[curReg], find_res->vd.offset);
        }
        else if (op->kind == VARIABLE) {
            if (find_res->vd.kind == COMMONVAR)
                fprintf(stream, "  lw %s, %d($fp)\n", Regs[curReg], find_res->vd.offset);
            else if (find_res->vd.kind == PARAMVAR) {
                    fprintf(stream, "  lw %s, %d($fp)\n", Regs[curReg], find_res->vd.offset);
      
            }
        }
        else if (op->kind == ARRAYNAME) {
            fprintf(stream, "  addi %s, $fp, %d\n", Regs[curReg], find_res->vd.offset);
        }
    }
    retReg = curReg;
    curReg = (((curReg + 1) - T0) % USE_NUM_REG) + T0;
    return retReg;
}


void prePrint(FILE *stream) {
    fprintf(stream, ".data\n");
    fprintf(stream, "_prompt: .asciiz \"Enter an integer:\"\n");
    fprintf(stream, "_ret: .asciiz \"\\n\"\n");
    fprintf(stream, ".globl main\n");
    fprintf(stream, ".text\n");
    fprintf(stream, "read:\n");
    fprintf(stream, "  li $v0, 4\n");
    fprintf(stream, "  la $a0, _prompt\n");
    fprintf(stream, "  syscall\n");
    fprintf(stream, "  li $v0, 5\n");
    fprintf(stream, "  syscall\n");
    fprintf(stream, "  jr $ra\n");
    fprintf(stream, "\n");
    fprintf(stream, "write:\n");
    fprintf(stream, "  li $v0, 1\n");
    fprintf(stream, "  syscall\n");
    fprintf(stream, "  li $v0, 4\n");
    fprintf(stream, "  la $a0, _ret\n");
    fprintf(stream, "  syscall\n");
    fprintf(stream, "  move $v0, $0\n");
    fprintf(stream, "  jr $ra\n");
}

void objTranslate(FILE *stream) {
    prePrint(stream);
    VDshead = newVDsHead();
    struct InterCodes* curcodes = codes->next;
    while (curcodes != codes) {
        //printInterCode(&curcodes->code, stdout);
        if (curcodes->code.kind == FUNC) {
            clearVDT();
            Offset = -4;// the first offset should be -4
            num_reg_available = 18;
            param_index = 0;
            paramOffset = 8;
            printFunc(stream, &curcodes->code);
            //fprintf(stderr, "FUNC\n");
            struct InterCodes* tmpcur = curcodes->next;
            while (tmpcur != codes && tmpcur->code.kind != FUNC) {
                allocSpace(stream, &tmpcur->code);
                tmpcur = tmpcur->next;
            }
            fprintf(stream, "  addi $sp, $sp, %d\n", Offset + 4);
        }
        else if (curcodes->code.kind == RETURN) {
            //fprintf(stderr, "RETURN\n");
            printReturn(stream, &curcodes->code);
        }
        else if (curcodes->code.kind == ASSIGN) {
            //fprintf(stderr, "ASSIGN\n");
            if (curcodes->code.u.assign.left == NULL) {
                curcodes = curcodes->next;
                continue;
            }
            printAssign(stream, &curcodes->code);
        }
        else if (curcodes->code.kind == DEC) {
            curcodes = curcodes->next;
            continue;
        }
        else if (curcodes->code.kind == PARAM) {
            curcodes = curcodes->next;
            continue;
        }
        else if (curcodes->code.kind == ARG) {
            printArg(stream, &curcodes->code);
        }
        else if (curcodes->code.kind == CALL) {
            printCall(stream, &curcodes->code);
        }
        else if (curcodes->code.kind == READ) {
            printRead(stream, &curcodes->code);
        }
        else if (curcodes->code.kind == WRITE) {
            printWrite(stream, &curcodes->code);
        }
        else if (curcodes->code.kind == ADD) {
            printAdd(stream, &curcodes->code);
        }
        else if (curcodes->code.kind == SUB) {
            printSub(stream, &curcodes->code);
        }
        else if (curcodes->code.kind == MUL) {
            printMul(stream, &curcodes->code);
        }
        else if (curcodes->code.kind == DIV) {
            printDiv(stream, &curcodes->code);
        }
        else if (curcodes->code.kind == GOTOOP) {
            printGoto(stream, &curcodes->code);
        }
        else if (curcodes->code.kind == CONDOP) {
            printCond(stream, &curcodes->code);
        }
        else if (curcodes->code.kind == LABEL) {
            printLabel(stream, &curcodes->code);
        }
        curcodes = curcodes->next;
    }
}

void printLabel(FILE *stream, struct InterCode* code) {
    fprintf(stream, "label%d:\n", code->u.x->u.var_no);
}

void printAssign(FILE *stream, struct InterCode *code) {
    int regx = getReg(code->u.assign.left, stream);
    Operand left = code->u.assign.left;
    Operand right = code->u.assign.right;
    if (left->kind == TMPVAR || left->kind == VARIABLE || (left->kind == ARRADDRESS && left->star == 0)) {
        if (right->kind == CONSTANT) {
            fprintf(stream, "  li %s, %d\n", Regs[regx], code->u.assign.right->u.value);
        }
        else if (right->kind == TMPVAR || right->kind == VARIABLE) {
            int regy = getReg(right, stream);
            fprintf(stream, "  move %s, %s\n", Regs[regx], Regs[regy]);
        }
        else if (right->kind == ARRADDRESS) {
            int regy = getReg(right, stream);
            if (right->star == 0) {
                fprintf(stream, "  move %s, %s\n", Regs[regx], Regs[regy]);
            }
            else {
                fprintf(stream, "  lw %s, 0(%s)\n", Regs[regx], Regs[regy]);
            }
        }
        VDs* find_res = findVDs(code->u.assign.left);
        fprintf(stream, "  sw %s, %d($fp)\n", Regs[regx], find_res->vd.offset);
    }
    else if (left->kind == ARRADDRESS && left->star == 1) {
        if (right->kind == CONSTANT) {
            int regy = getReg(right, stream);
            fprintf(stream, "  sw %s, 0(%s)\n", Regs[regy], Regs[regx]);
        }
        else if (right->kind == TMPVAR) {
            int regy = getReg(right, stream);
            fprintf(stream, "  sw %s, 0(%s)\n", Regs[regy], Regs[regx]);
        }
        else if (right->kind == ARRADDRESS) {
            int regy = getReg(right, stream);
            if (right->star == 0) {
                fprintf(stream, "  sw %s, 0(%s)\n", Regs[regy], Regs[regx]);
            }
            else if (right->star == 1) {
                fprintf(stream, "  lw %s, 0(%s)\n", Regs[regy], Regs[regy]);
                fprintf(stream, "  sw %s, 0(%s)\n", Regs[regy], Regs[regx]);
            }
        }
    }   
}

void printAdd(FILE *stream, struct InterCode *code) {
    Operand result, op1, op2;
    result = code->u.binop.result;
    op1 = code->u.binop.op1;
    op2 = code->u.binop.op2;
    int regresult = getReg(result, stream);
    int regop1 = getReg(op1, stream);
    int regop2 = getReg(op2, stream);
    if (op1->kind == ARRADDRESS && op1->star == 1) {
        fprintf(stream, "  lw %s, 0(%s)\n", Regs[regop1], Regs[regop1]);
    }
    if (op2->kind == ARRADDRESS && op2->star == 1) {
        fprintf(stream, "  lw %s, 0(%s)\n", Regs[regop2], Regs[regop2]);
    }
    if (result->kind != ARRADDRESS || (result->kind == ARRADDRESS && result->star == 0)) {
        fprintf(stream, "  add %s, %s, %s\n", Regs[regresult], Regs[regop1], Regs[regop2]);
    }
    else if (result->star == 1) {
        fprintf(stream, "  add %s, %s, %s\n", Regs[regop1], Regs[regop1], Regs[regop2]);
        fprintf(stream, "  sw %s, 0(%s)\n", Regs[regop1], Regs[regresult]);
        return;
    }
    VDs* find_res = findVDs(result);
    fprintf(stream, "  sw %s, %d($fp)\n", Regs[regresult], find_res->vd.offset);
}

void printMul(FILE *stream, struct InterCode *code) {
    Operand result, op1, op2;
    result = code->u.binop.result;
    op1 = code->u.binop.op1;
    op2 = code->u.binop.op2;
    int regresult = getReg(result, stream);
    int regop1 = getReg(op1, stream);
    int regop2 = getReg(op2, stream);
    if (op1->kind == ARRADDRESS && op1->star == 1) {
        fprintf(stream, "  lw %s, 0(%s)\n", Regs[regop1], Regs[regop1]);
    }
    if (op2->kind == ARRADDRESS && op2->star == 1) {
        fprintf(stream, "  lw %s, 0(%s)\n", Regs[regop2], Regs[regop2]);
    }
    if (result->kind != ARRADDRESS || (result->kind == ARRADDRESS && result->star == 0)) {
        fprintf(stream, "  mul %s, %s, %s\n", Regs[regresult], Regs[regop1], Regs[regop2]);
    }
    else if (result->star == 1) {
        fprintf(stream, "  mul %s, %s, %s\n", Regs[regop1], Regs[regop1], Regs[regop2]);
        fprintf(stream, "  sw %s, 0(%s)\n", Regs[regop1], Regs[regresult]);
        return;
    }
    VDs* find_res = findVDs(result);
    fprintf(stream, "  sw %s, %d($fp)\n", Regs[regresult], find_res->vd.offset);
}

void printDiv(FILE *stream, struct InterCode *code) {
    Operand result, op1, op2;
    result = code->u.binop.result;
    op1 = code->u.binop.op1;
    op2 = code->u.binop.op2;
    int regresult = getReg(result, stream);
    int regop1 = getReg(op1, stream);
    int regop2 = getReg(op2, stream);
    if (op1->kind == ARRADDRESS && op1->star == 1) {
        fprintf(stream, "  lw %s, 0(%s)\n", Regs[regop1], Regs[regop1]);
    }
    if (op2->kind == ARRADDRESS && op2->star == 1) {
        fprintf(stream, "  lw %s, 0(%s)\n", Regs[regop2], Regs[regop2]);
    }
    if (result->kind != ARRADDRESS || (result->kind == ARRADDRESS && result->star == 0)) {
        fprintf(stream, "  div %s, %s\n", Regs[regop1], Regs[regop2]);
        fprintf(stream, "  mflo %s\n", Regs[regresult]);
    }
    else if (result->star == 1) {
        fprintf(stream, "  div %s, %s\n", Regs[regop1], Regs[regop2]);
        fprintf(stream, "  mflo %s\n", Regs[regop1]);
        fprintf(stream, "  sw %s, 0(%s)\n", Regs[regop1], Regs[regresult]);
        return;
    }
    VDs* find_res = findVDs(result);
    fprintf(stream, "  sw %s, %d($fp)\n", Regs[regresult], find_res->vd.offset);
}


void printSub(FILE *stream, struct InterCode *code) {
    Operand result, op1, op2;
    result = code->u.binop.result;
    op1 = code->u.binop.op1;
    op2 = code->u.binop.op2;
    int regresult = getReg(result, stream);
    int regop1 = getReg(op1, stream);
    int regop2 = getReg(op2, stream);
    if (op1->kind == ARRADDRESS && op1->star == 1) {
        fprintf(stream, "  lw %s, 0(%s)\n", Regs[regop1], Regs[regop1]);
    }
    if (op2->kind == ARRADDRESS && op2->star == 1) {
        fprintf(stream, "  lw %s, 0(%s)\n", Regs[regop2], Regs[regop2]);
    }
    if (result->kind != ARRADDRESS || (result->kind == ARRADDRESS && result->star == 0)) {
        fprintf(stream, "  sub %s, %s, %s\n", Regs[regresult], Regs[regop1], Regs[regop2]);
    }
    else if (result->star == 1) {
        fprintf(stream, "  sub %s, %s, %s\n", Regs[regop1], Regs[regop1], Regs[regop2]);
        fprintf(stream, "  sw %s, 0(%s)\n", Regs[regop1], Regs[regresult]);
        return;
    }
    VDs* find_res = findVDs(result);
    fprintf(stream, "  sw %s, %d($fp)\n", Regs[regresult], find_res->vd.offset);
}

void printFunc(FILE *stream, struct InterCode *code) {
    fprintf(stream, "\n%s:\n", code->u.name);
    fprintf(stream, "  addi $sp, $sp, -4\n");
    fprintf(stream, "  sw $fp, 0($sp)\n");
    fprintf(stream, "  move $fp, $sp\n");
}

void printDec(FILE *stream, struct InterCode *code) {
    Offset -= code->u.dec.memory->u.value;
    VD vd1;
    makeVD(&vd1, COMMONVAR, code->u.dec.op, -1, Offset + 4);
    InsertVD(vd1);
}

void printReturn(FILE *stream, struct InterCode *code) {
    int xreg = getReg(code->u.x, stream);
    if (code->u.x->kind == TMPVAR) {
        fprintf(stream, "  move $v0, %s\n", Regs[xreg]);
    }
    else if (code->u.x->kind == ARRADDRESS) {
        if (code->u.x->star == 1) {
            fprintf(stream, "  lw $v0, 0(%s)\n", Regs[xreg]);
        }
        else {
            fprintf(stream, "  move $v0, %s\n", Regs[xreg]);
        }
    }
    fprintf(stream, "  move $sp, $fp\n");
    fprintf(stream, "  lw $fp, 0($sp)\n");
    fprintf(stream, "  addi $sp, $sp, 4\n");
    fprintf(stream, "  jr $ra\n");
}

void printParam(FILE *stream, struct InterCode *code) {
    //struct InterCode *code
    if (paramOffset < 8) {
        VD vd1;
        makeVD(&vd1, PARAMVAR, code->u.x, A0 + (paramOffset + 8) / 4, -1);
        InsertVD(vd1);
    }
    else {
        VD vd1;
        makeVD(&vd1, PARAMVAR, code->u.x, -1, paramOffset);
        InsertVD(vd1);
    }
    paramOffset += 4;
}

void printArg(FILE *stream, struct InterCode *code) {
    VDs* find_res = findVDs(code->u.x);
    if (code->u.x->kind == TMPVAR){
        //fprintf(stderr, "code->u.x.var_no = %d\n", code->u.x->u.var_no);
    }
    if (code->u.x->kind == TMPVAR) {
        if (argOffset < 8) {
            fprintf(stream, "  lw %s, %d($fp)\n", Regs[(argOffset + 8) / 4 + A0], find_res->vd.offset);
        }
        else {
            fprintf(stream, "  addi $sp, $sp, -4\n");
            fprintf(stream, "  lw %s, %d($fp)\n", Regs[curReg], find_res->vd.offset);
            fprintf(stream, "  sw %s, 0($sp)\n", Regs[curReg]);
        }
    }
    else if (code->u.x->kind == ARRADDRESS && code->u.x->star == 1) {
        if (argOffset < 8) {
            fprintf(stream, "  lw %s, %d($fp)\n", Regs[(argOffset + 8) / 4 + A0], find_res->vd.offset);
            fprintf(stream, "  lw %s, 0(%s)\n", Regs[(argOffset + 8) / 4 + A0], Regs[(argOffset + 8) / 4]);
        }
        else {
            fprintf(stream, "  addi $sp, $sp, -4\n");
            fprintf(stream, "  lw %s, %d($fp)\n", Regs[curReg], find_res->vd.offset);
            fprintf(stream, "  lw %s, 0(%s)\n", Regs[curReg], Regs[curReg]);
            fprintf(stream, "  sw %s, 0($sp)\n", Regs[curReg]);
        }
    }
    argOffset += 4;
    //fprintf(stderr, "???\n");
}


void printCall(FILE *stream, struct InterCode *code) {
    int regx;
    VDs* find_res;
    
    fprintf(stream, "  addi $sp, $sp, -4\n");
    fprintf(stream, "  sw $ra, 0($sp)\n");
    fprintf(stream, "  jal %s\n", code->u.callop.funcname);
    fprintf(stream, "  lw $ra, 0($sp)\n");
    fprintf(stream, "  addi $sp, $sp, 4\n");
    if (argOffset > 8) {
        fprintf(stream, "  addi $sp, $sp, %d\n", argOffset - 8);
    }
    if (code->u.callop.result != NULL) {
        regx = getReg(code->u.callop.result, stream);
        find_res = findVDs(code->u.callop.result);
    }
    if (code->u.callop.result != NULL) {
        fprintf(stream, "  sw $v0, %d($fp)\n", find_res->vd.offset);
    }
    argOffset = 8;
}

void printWrite(FILE *stream, struct InterCode *code) {
    int regx;
    VDs* find_res;
    if (code->u.x != NULL) {
        //regx = getReg(code->u.x, stream);
        find_res = findVDs(code->u.x);      
    } 
    fprintf(stream, "  lw $a0, %d($fp)\n", find_res->vd.offset);
    if (code->u.x->kind == ARRADDRESS && code->u.x->star == 1) {
        fprintf(stream, "  lw $a0, 0($a0)\n");       
    }   
    fprintf(stream, "  addi $sp, $sp, -4\n");
    fprintf(stream, "  sw $ra, 0($sp)\n");
    fprintf(stream, "  jal write\n");
    fprintf(stream, "  lw $ra, 0($sp)\n");
    fprintf(stream, "  addi $sp, $sp, 4\n");
}

void printRead(FILE *stream, struct InterCode *code) {
    int regx;
    VDs* find_res;
    if (code->u.x != NULL) {
        regx = getReg(code->u.x, stream);
        find_res = findVDs(code->u.x);
    }
    fprintf(stream, "  addi $sp, $sp, -4\n");
    fprintf(stream, "  sw $ra, 0($sp)\n");
    fprintf(stream, "  jal read\n");
    fprintf(stream, "  lw $ra, 0($sp)\n");
    fprintf(stream, "  addi $sp, $sp, 4\n");
    if (code->u.x != NULL) {
        fprintf(stream, "  sw $v0, %d($fp)\n", find_res->vd.offset);
    }
}

void printGoto(FILE *stream, struct InterCode *code) {
    fprintf(stream, "  j label%d\n", code->u.gotoop.L->u.var_no);
}

void printCond(FILE *stream, struct InterCode *code) {
    Operand L, op1, op2;
    L = code->u.condop.L;
    op1 = code->u.condop.op1;
    op2 = code->u.condop.op2;
    int regop1 = getReg(op1, stream);
    int regop2 = getReg(op2, stream);
    if (op1->kind == ARRADDRESS && op1->star == 1) {
        fprintf(stream, "  lw %s, 0(%s)\n", Regs[regop1], Regs[regop1]);
    }
    if (op2->kind == ARRADDRESS && op2->star == 1) {
        fprintf(stream, "  lw %s, 0(%s)\n", Regs[regop2], Regs[regop2]);
    }
    if (!strcmp(code->u.condop.op, "==")) {
        fprintf(stream, "  beq %s, %s, label%d\n", Regs[regop1], Regs[regop2], L->u.var_no);
    }
    if (!strcmp(code->u.condop.op, "!=")) {
        fprintf(stream, "  bne %s, %s, label%d\n", Regs[regop1], Regs[regop2], L->u.var_no);
    }
    if (!strcmp(code->u.condop.op, ">")) {
        fprintf(stream, "  bgt %s, %s, label%d\n", Regs[regop1], Regs[regop2], L->u.var_no);
    }
    if (!strcmp(code->u.condop.op, "<")) {
        fprintf(stream, "  blt %s, %s, label%d\n", Regs[regop1], Regs[regop2], L->u.var_no);
    }
    if (!strcmp(code->u.condop.op, ">=")) {
        fprintf(stream, "  bge %s, %s, label%d\n", Regs[regop1], Regs[regop2], L->u.var_no);
    }
    if (!strcmp(code->u.condop.op, "<=")) {
        fprintf(stream, "  ble %s, %s, label%d\n", Regs[regop1], Regs[regop2], L->u.var_no);
    }
}















