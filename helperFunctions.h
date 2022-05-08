#include <stdlib.h>
#include <string.h>

int pow2(int x);
int isDig(char s);
int isNum(char* s);
char *strdup(char *s);
char get_last(char* s);
int str_to_tags(char* str,char* delimiters, char*** tokens);
char* subString(char *string, int position, int length);
void dynamic_array_free(void* arr[],int size,void (*fun_ptr)(void*));
int is_blank(const char *s);
int trim_wspace(char **out, int len, char *str);
