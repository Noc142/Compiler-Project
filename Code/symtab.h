#ifndef SYMTAB_H
#define SYMTAB_H

#define DEFAULT_HASH_SIZE 512
#define STRLEN 40
#define _VAR 1
#define _FUNC 2
#define _STRUCT 3
#define _STRUCT_VAR 4


typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct FieldList_* Symbol;

struct Type_
{
	enum { BASIC, ARRAY, STRUCTURE } kind;
	union 
	{
		//basic type
		int basic;
		//array
		struct { Type elem; int size; } array;
		FieldList structure;
	} u;
};

struct FieldList_
{
	int which; //Variable?Function?Struct?
	char name[STRLEN];
	Type type;
	FieldList tail;
	struct FieldList_ *left, *right; //used to build binary search tree
};


/*Binary Search Tree Node*/

typedef struct SymTable {
	Symbol table_var[DEFAULT_HASH_SIZE];//var
	Symbol table_func[DEFAULT_HASH_SIZE];//func
	Symbol table_struct[DEFAULT_HASH_SIZE];//struct
	Symbol table_struct_var[DEFAULT_HASH_SIZE];//var in struct
} SymTable;

unsigned int hashcode(char k[]);
Symbol newSymbol_var(char k[], Type tp);
Symbol newSymbol_struct_var(char k[], Type tp);
Symbol newSymbol_func(char k[], Type tp, FieldList fl);
Symbol newSymbol_struct(char k[], FieldList fl);
/*Insert a symbol to this tree*/
int BSTInsert_var(Symbol ptr, char k[], Type tp);
int BSTInsert_func(Symbol ptr, char k[], Type tp, FieldList fl);
int BSTInsert_struct(Symbol ptr, char k[], FieldList fl);
/*Judge if Symbol k in the tree*/
int BSTContains(Symbol ptr, char k[]);
void delete_BST(Symbol ptr);

int SymContains(struct SymTable table, char k[]);
int SymInsert_var(Symbol table[], char k[], Type tp);
int SymInsert_func(Symbol table[], char k[], Type tp, FieldList fl);
int SymInsert_struct(Symbol table[], char k[], FieldList fl);


#endif
