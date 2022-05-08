#include "table.h"


struct Table{
	tblEntry *hashtab[HASHSIZE];
	DataType dtype;
};
/* hash: form hash value for string s */
unsigned hash(char *s)
{
	unsigned hashval;
	for (hashval = 0; *s != '\0'; s++)
		hashval = *s + 31 * hashval;
	return hashval % HASHSIZE;
}

Table* table_new(DataType dtype){
	int i =0;
	Table* newTable = (Table*)malloc(sizeof(Table));
	if(newTable==NULL)
		exit(3);
	for(i=0;i<HASHSIZE;i++)
		newTable->hashtab[i]=NULL;
	newTable->dtype = dtype;
	return newTable;
}

/* lookup: look for s in hashtab */
tblEntry* table_lookup(Table* t,char *s)
{
	tblEntry *np;
	np = t->hashtab[hash(s)];

	for (np = t->hashtab[hash(s)]; np != NULL; np = np->next)
		if (strcmp(s, np->name) == 0)
			return np; /* found */
	return NULL; /* not found */
}

/* install: put (name, value) in hashtab */
void table_add(Table *t,char *name, void* value)
{
	tblEntry *np;
	unsigned hashval;
	np = (struct tblEntry *) malloc(sizeof(tblEntry));
	if (np == NULL || (np->name = strdup(name)) == NULL)
		exit(3);
	hashval = hash(name);
	np->next = t->hashtab[hashval];
	t->hashtab[hashval] = np;
	switch(t->dtype){
		case INSTRCT_NFO:
			np->value = instrctinfo_copy((InstrctInfo*)value);
		break;
		case SYMBOL:
			np->value = symbol_copy((Symbol*)value);
		break;
	}
}

void freeEntry(Table* t,tblEntry *np){
	tblEntry *ptr;
	tblEntry *next;

	if (np!=NULL){
		ptr = np;
		while (ptr!=NULL){
			next = ptr->next;
			if (t->dtype == INSTRCT_NFO)
				free(ptr->value);
			if (t->dtype == SYMBOL)
				free(ptr->value);
			free(ptr->name);
			free(ptr);
			ptr = next;
		}
	}	
}

void table_free(Table* t){
	int i;
	for(i=0;i<HASHSIZE;i++){
		freeEntry(t,t->hashtab[i]);
	}
	free(t);
}

/******************************************************************/
/******************************************************************/





InstrctInfo* instrctinfo_copy(InstrctInfo* info){
	InstrctInfo* new_info = instrctinfo_new(info->opcode,info->funct,info->paranum,info->allowed);
	return new_info;
}

InstrctInfo* instrctinfo_new(uint opcode,uint funct,uint paranum
								,uint allowed_para){
	InstrctInfo* info;
	info = (InstrctInfo*)malloc(sizeof(InstrctInfo));
	if (info==NULL){
		printf("Memory allocation failed in Instrct_new");
		exit(3);
	}
	info->funct = funct;
	info->opcode = opcode;
	info->paranum = paranum;
	info->allowed = allowed_para;
	return info;
}

/**********************************************************
 *
 */

Symbol* symbol_new(uint extf,uint codf,uint entf,uint datf,uint address){
	Symbol* newsym;
	newsym = (Symbol*)malloc(sizeof(Symbol));
	newsym->external = extf;
	newsym->data = datf;
	newsym->code = codf;
	newsym->entry = entf;
	newsym->address = address;
	return newsym;
}

Symbol* symbol_copy(Symbol* s){
	Symbol* newSymbol = symbol_new(s->external,s->code,s->entry,s->data,s->address);
	return newSymbol;
}


