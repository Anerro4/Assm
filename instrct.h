#ifndef __instrct_h_
#define __instrct_h_
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "word.h"
#include <string.h> 
#include "global_defines.h"

/*************************************************************************/
/*************************************************************************/

typedef struct Instrct{
	uint opcode:4;
	uint funct:4;
	uint sAddr:2;
	uint dAddr:2;
	uint paraNum;
	Word* dWords;
} Instrct;

void instrct_free(Instrct* inst);
Instrct* instrct_copy(Instrct* inst);
int instrct_set_dataword(Instrct* inst,uint index,Word* w);
Word* instrct_toWords(Instrct* inst);
void instrct_print(Instrct* inst);
void instrct_set(Instrct* inst);
Instrct* instrct_new(uint opcode,uint funct,uint sAddr,uint dAddr,uint paranum);

/*************************************************************************/
/*************************************************************************/


typedef struct InstrctNode {
    Instrct* data;
    struct InstrctNode *next;
}InstrctNode;

typedef struct InstrctLst {
    InstrctNode *start;
    InstrctNode *last;   
}InstrctLst;

void instrctLst_free(InstrctLst *lst);
void instrctLst_init(InstrctLst *lst);
void instrctLst_add(InstrctLst *lst,Instrct *inst);
void instrctLst_print(InstrctLst *lst);
Instrct instrctLst_get(InstrctLst lst);

/*************************************************************************/
/*************************************************************************/

#endif


