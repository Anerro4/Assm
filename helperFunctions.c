#include "helperFunctions.h"
#include <stdio.h>

int pow2(int x){ /* returns 2^x*/
	int i;
	unsigned result=1;
	for (i=0;i<x;i++)
		result<<=1;
	return result;
}

int is_blank(const char *s) {
  while (*s != '\0') {
    if (!isspace((unsigned char)*s))
      return 0;
    s++;
  }
  return 1;
}

int trim_wspace(char **out, int len, char *str)
{
    int l;
    int n;
    const char * word_begins;
    const char * word_ends;
    char * copy;
    int i;

    l = strlen (str);
    n = l;
    word_begins = str;
    while (isspace (*word_begins)) {
        word_begins++;
        n--;
    }
    word_ends = str + l - 1;
    if (n == 0){
    	copy = malloc (2);
    	copy[0] = ' ';
    	copy[1] ='\0';
    	(*out) = copy;
    	return 1;
    }
    while (isspace (*word_ends)) {

        word_ends--;
        n--;
    }
    if (n == l) {
    	(*out) = strdup (str);
    }
    else {


        copy = malloc (n + 1);
        if (! copy) {
            fprintf (stderr, "Out of memory.\n");
            exit (EXIT_FAILURE);
        }
        for (i = 0; i < n; i++) {
            copy[i] = word_begins[i];
        }
        copy[n] = '\0';
        (*out) = copy;
        return n;
    }
}

void dynamic_array_free(void* arr[],int size,void (*fun_ptr)(void*))
{
	int i;
	for(i=0;i<size;i++){
		(*fun_ptr)(arr[i]);
	}
	free(arr);
} 

int isDig(char s){	
	return (s >='0' && s <='9');
}

int isNum(char* s){
	int i = 0;
	if ((((s[0] == '+') || (s[0] == '-')) && (strlen(s)!=1))
		 || (isDig(s[0]))){
		for(i=1;i<strlen(s);i++){
			if (isDig(s[i])==0)
				return 0;	
		}
		return 1;	
	}
	else{
		return 0;
	}
}

char *strdup(char *s) /* make a duplicate of s */
{
	char *p;
	p = (char *) malloc(strlen(s)+1); /* +1 for ’\0’ */
	if (p != NULL)
		strcpy(p, s);
	return p;
}

char get_last(char* str){
	return str[strlen(str)-1];
}

char *subString(char *string, int position, int length)
{
	char *p;
	int c;

	p = malloc(length+1);

	if (p == NULL)
	{
		printf("Unable to allocate memory.\n");
		exit(1);
	}

	for (c = 0; c < length; c++)
	{
		*(p+c) = *(string+position);      
		string++;  
	}

	*(p+c) = '\0';

	return p;
}

char *joinstr (char **s, char *sep,int size)
{
	int i;
    char *joined = NULL;                /* pointer to joined string w/sep */
    size_t lensep = strlen (sep),       /* length of separator */
        sz = 0;                         /* current stored size */
    int first = 1;                      /* flag whether first term */

    for(i=0;i<size;i++){                        /* for each string in s */
        size_t len = strlen (*s);
        /* allocate/reallocate joined */
        void *tmp = realloc (joined, sz + len + (first ? 0 : lensep) + 1);
        if (!tmp) {                     /* validate allocation */    /* handle error (adjust as req'd) */
            exit (EXIT_FAILURE);
        }
        joined = tmp;                   /* assign allocated block to joined */
        if (!first) {                   /* if not first string */
            strcpy (joined + sz, sep);  /* copy separator */
            sz += lensep;               /* update stored size */
        }
        strcpy (joined + sz, *s++);     /* copy string to joined */
        first = 0;                      /* unset first flag */
        sz += len;                      /* update stored size */
    }

    return joined;      /* return joined string */
}

int str_to_tags(char* str,char* delimiters,char*** tokens){
	int i = 1;
	char ** tok;
	char* trimmed;
	int size_trim;
	char* str_cpy = strdup(str); /*strtok for some reason changes input*/
				    /*so you have to work on a copy*/
	char* pch = strtok (str_cpy,delimiters);

	tok = (char**)malloc(sizeof(char*));
	if(pch != NULL){
		size_trim = trim_wspace(&trimmed,strlen(pch),pch);
		tok[0] = strdup(trimmed);
		pch = strtok (NULL, delimiters);
		free(trimmed);
		while (pch != NULL)
		{	
			tok = (char**)realloc(tok,(i+1)*sizeof(char*));
			size_trim = trim_wspace(&trimmed,strlen(pch),pch);
			tok[i] = strdup(trimmed);
			i++;	
			pch = strtok (NULL, delimiters);
			free(trimmed);
			
	  	}
		*tokens = tok;
	}

	else{
		i=-1;
		free(tok);
		*tokens = NULL;
	}

	free (str_cpy);
	return i;
		
}

