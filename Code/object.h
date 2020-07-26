#ifndef _OBJECT_H_
#define _OBJECT_H_
#include "list.h"
#include <stdio.h>

extern struct InterCodes* codes;


#define NUM_REG 32
#define USE_NUM_REG 18

enum RegEnum{ 
    A0 = 4, A1, A2, A3, T0, T1, T2, T3, T4, T5, T6, T7, S0, S1, S2, S3, S4, S5, S6, S7, T8, T9
};

typedef struct VarDescripter{
    enum {PARAMVAR, COMMONVAR} kind;//common variable and arg variable
    Operand op;
    int reg;
    int offset;
} VarDescripter, VD;


typedef struct VarDescripters{
    VarDescripter vd;
    struct VarDescripters *prev;
    struct VarDescripters *next;
} VDs, VarDescripters;

typedef struct RegDescripter {
    int flag;
    struct VarDescripters* vds;
    int offset;
} RD, RegDescripter;


extern int Offset;
extern int curReg;
extern int num_reg_available;
extern int param_index;


/*Register Descripter Table*/
extern struct RegDescripter RDT[NUM_REG];
void initRDT();
void refreshRDT(FILE *stream);
void clearVDT();
void objTranslate(FILE *stream);
VarDescripters* InsertVD(struct VarDescripter vd); //insert vd into VDT
VarDescripters* InsertOptoVDs(Operand op); //insert OP into VDT

void makeVD(VarDescripter* vd, int kind, Operand op, int reg, int offset);
VarDescripters* findVDs(Operand op);

int getReg(Operand op, FILE* stream);

struct VarDescripters * newVDsHead();
void allocSpace(FILE *stream, struct InterCode* code);
void prePrint(FILE *stream);//output prefix, read and write function.
void printLabel(FILE *stream, struct InterCode* code);
void printAssign(FILE *stream, struct InterCode* code);
void printFunc(FILE *stream, struct InterCode *code);
void printReturn(FILE *stream, struct InterCode *code);
void printDec(FILE *stream, struct InterCode *code);
void printRead(FILE *stream, struct InterCode *code);
void printParam(FILE *stream, struct InterCode *code);
void printArg(FILE *stream, struct InterCode *code);
void printCall(FILE *stream, struct InterCode *code);
void printRead(FILE *stream, struct InterCode *code);
void printWrite(FILE *stream, struct InterCode *code);
void printAdd(FILE *stream, struct InterCode *code);
void printSub(FILE *stream, struct InterCode *code);
void printMul(FILE *stream, struct InterCode *code);
void printDiv(FILE *stream, struct InterCode *code);
void printGoto(FILE *stream, struct InterCode *code);
void printCond(FILE *stream, struct InterCode *code);
#endif
