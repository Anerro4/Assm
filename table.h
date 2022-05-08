#ifndef __table_h_
#define __table_h_

#include <stdlib.h>
#include <string.h>
#define HASHSIZE 101
#include "helperFunctions.h"
#include "instrct.h"

typedef enum DataTypes {
	INSTRCT_NFO,
	SYMBOL
} DataType;

typedef struct Table Table;

typedef struct tblEntry { /* table entry: */
	struct tblEntry *next; /* next entry in chain */
	char *name; /* defined name */
	void* value;
}tblEntry;

void table_free(Table* t);
Table* table_new(DataType dtype);
tblEntry* table_lookup(Table* t,char *s);
void table_add(Table *d,char *name, void* value);

/************************************************************
*
*	InstrctInfo contains relevant info of instruction.
*	Decided to show the implementation to prevent code
*	from being even bigger and more complex.
*
*	Allowed is a bit mask that decides later whether or not the
*	correct operators types are given to the instruction
*	Paranum contains the number of parameters that are legal
*	for the instruction
*
/************************************************************/

typedef struct InstrctInfo{
	uint opcode:4;
	uint funct:4;
	uint paranum;
	uint allowed;
}InstrctInfo;

InstrctInfo* instrctinfo_new(uint opcode,uint funct,uint paranum
								,uint allowed_para);
InstrctInfo* instrctinfo_copy(InstrctInfo* info);

/************************************************************/
/************************************************************/

typedef struct Symbol{
	uint address;
	uint external:1;
	uint data:1;
	uint code:1;
	uint entry:1;
}Symbol;

Symbol* symbol_new(uint addf,uint codf,uint entf,uint datf,uint address);
Symbol* symbol_copy(Symbol* s);

/************************************************************/
/************************************************************/
#endif
