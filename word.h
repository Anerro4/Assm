#ifndef __word_h_
#define __word_h_

#include <stdbool.h>
#define SIZE 12
#define HEX_SIZE 3
#define STR_(X) #X
#define STR(X) STR_(X)

typedef struct Word{
	unsigned int bits:SIZE;
} Word;

void word_setc(Word *w,char c);
void word_toHex(Word w);
int word_toInt(Word* w);
void word_set(Word *w,unsigned int val);
int word_create(Word *w);
void word_clear(Word *w);
void word_copy(Word* src,Word *dst);
int word_add(Word *w,Word toAdd);
int word_sub(Word *w,Word toAdd);
void word_free(Word *w);
void word_print(Word* w);

#endif
