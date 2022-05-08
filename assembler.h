#include "word.h"
#include "table.h"
#include "instrct.h"
#include "helperFunctions.h"
#include "syntax.h"
#include "global_defines.h"
#include <stdio.h>
#include <stdlib.h>

#define IC_START 100
#define REG_NUM 8

typedef struct Assembler Assembler;

Assembler* assmbl_new();
void assmbl_free(Assembler *asmblr);
void assmbl_start(Assembler *asmblr,int fileNum,char *fileNames[]);

