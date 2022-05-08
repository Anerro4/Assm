#include "word.h"
#include <stdlib.h>
#include <stdio.h>


int word_toInt(Word* w){
	return w->bits;
}

void word_set(Word *w,unsigned int val){
	w->bits = val;		
}

void word_setc(Word *w,char c){
	w->bits = (int)c;		
}

void word_clear(Word *w){
	w->bits = 0;
}

void word_copy(Word* src,Word *dst){
	dst->bits = src->bits;		
}

void word_print(Word* w){
	printf("%0" STR(HEX_SIZE) "x\n", w->bits);
}

