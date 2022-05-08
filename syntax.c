#include "syntax.h"

int is_legal_instrct(Table* instrc_table,char* inst_tag,char* op1_tag,char* op2_tag
						){

	tblEntry* entry = table_lookup(instrc_table,inst_tag);
	TokenType t1;
	TokenType t2;
	InstrctInfo* value;
	uint flags=0;

	if (entry == NULL)
		return 0;
	value = (InstrctInfo*) entry->value;
	t1 = check_tok_type(op1_tag);
	t2 = check_tok_type(op2_tag);

	/* here we define the flags by 10 bits*/

	flags |= (t1 == IMMIDIATE)|( (t1 == LABEL_OR_INSTRUCTION)<< 1)
				|((t1 == RELATIVE) << 2)
				|((t1 == REGISTER) << 3)
				|((t1 == NULL_P) << 4);

	flags |= ((t2 == IMMIDIATE) << 5)|((t2 == LABEL_OR_INSTRUCTION) << 6)
				|((t2 == RELATIVE) << 7)
				|((t2 == REGISTER) << 8)
				|((t2 == NULL_P) << 9);

	if (!((flags & value->allowed) == flags))
		return 0;
	return 1;
}
int is_label(char* s)
{
	int i;
	if (isalpha(s[0])){
		for (i=0;i<strlen(s)-1;i++){
			if (isalnum(s[i])==0)
				return 0;	
		}
		if (get_last(s) == ':'){
			if (strlen(s)!=1)
				return 1;
			else
				return 0;
		}
		else{
			if(isalnum(s[i])==0)
				return 0;
			else
				return 1;
		}	
	}
	return 0;	
}

TokenType check_tok_type(char* tag){
	char first;
	char last;

	if (tag == NULL)
		return NULL_P;

	first = tag[0];
	last = get_last(tag);
	switch(first){
		case '.':
			if (strcmp(&tag[1], "data")==0)
				return D_DIRECTIVE;
			if (strcmp(&tag[1], "string")==0)
				return S_DIRECTIVE;
			if (strcmp(&tag[1], "entry")==0)
				return EN_DIRECTIVE;
			if (strcmp(&tag[1], "extern")==0)
				return EX_DIRECTIVE;
			else
				return ERROR;
			break;
		case '#':
			if (isNum(&tag[1]))
				return IMMIDIATE;
			break;
		case '%':
			if (is_label(&tag[1]))
				return RELATIVE;				
			break;
		case 'r':
			if (strlen(tag) == 2){
				if (tag[1] >= '0' && tag[1] <= '7')
					return REGISTER;
			}
			break;
		case '"':
			if (strlen(tag) >1)
				if (first == last)
					return STRING;
			break;
		case ';':
			return COMMENT;
	}
	
	if(isNum(tag))
		return NUMBER;
	
	if(is_label(tag))
		if (last==':')
			return LABELDEF;
		else
			return LABEL_OR_INSTRUCTION;
	else
		return ERROR;		

}

void token_free(Token* t)
{
	free(t->tag);
	free(t);
}

Token* token_new(char* tag, TokenType type){
	Token* newtok = (Token*)malloc(sizeof(Token));
	newtok->type = type;
	newtok->tag = strdup(tag);
	return newtok;		
}

Token** analyze_tokens(char* tags[],int size){
	int i;
	TokenType type;
	Token** result = (Token**)malloc(sizeof(Token*)*size);
	for (i =0;i<size;i++){
		type = check_tok_type(tags[i]);
		if (type == ERROR){
			dynamic_array_free((void**)result,i,(void (*)(void*))token_free);
			return NULL;
		}
		else{
			result[i] = token_new(tags[i],type);
		}
	}
	return result;
}

int split_onfirst_comma(char* line,Token** toks[],uint bfrOrAftr){
	int size_stok=-1;
	char** tokenTags;
	char** subtokenTags=NULL;
	char* before_after;
	int size;

	before_after = NULL;

	size = str_to_tags(line,",",&tokenTags);
	if (!(bfrOrAftr)){
		if (size==-1)
		{
			before_after = strdup(line);
		}
		else
			before_after = strdup(tokenTags[0]);
		size_stok=str_to_tags(before_after," \t",&subtokenTags);

	}
	else{
		if (size==1){
			size_stok = 0;
		}
		else if (size>1){
			before_after = joinstr((tokenTags+1),",",size-1);
		}
		if (before_after!= NULL)
			size_stok=str_to_tags(before_after,",",&subtokenTags);
	}


	if((size_stok!=-1) && (size_stok!=0))
		*toks = analyze_tokens(subtokenTags,size_stok);
	else
		*toks=NULL;

	free(before_after);
	dynamic_array_free((void**)tokenTags,size,free);
	dynamic_array_free((void**)subtokenTags,size_stok,free);

	if (*toks != NULL)
		return size_stok;
	else if(size_stok == 0)
		return 0;
	else
		return -1;
}
	
int check_syntax(char* line,Table* instrcTable,Token** toks[]){
		int i;
		int size_tok_before;
		int size_tok_after;
		int lead_tok_indx;
		unsigned err_flag = 0;
		char** token_tags;

		TokenType lead_type;
		Token** tks_befr;
		Token** tks_aftr;

		if ((get_last(line)==',')||(line[0]==','))
			return -1;

 		size_tok_before = split_onfirst_comma(line,&tks_befr,0);
		size_tok_after = split_onfirst_comma(line,&tks_aftr,1);

		if (tks_befr != NULL){
			lead_tok_indx= (tks_befr[0]->type == LABELDEF);
			if ((lead_tok_indx == 1) &&
					((size_tok_before == 1)||(table_lookup(instrcTable,tks_befr[0]))))
				err_flag = 1;
			else{
				lead_type = tks_befr[lead_tok_indx]->type;

				switch(lead_type){
					case EN_DIRECTIVE:
					case EX_DIRECTIVE:
						if (size_tok_before ==(2+lead_tok_indx) && (tks_aftr == NULL)){
							if(tks_befr[lead_tok_indx+1]->type != LABEL_OR_INSTRUCTION)
								err_flag=1;
						}	
						else
							err_flag=1;
						break;
					case S_DIRECTIVE:
						if (size_tok_before ==(2+lead_tok_indx) && (tks_aftr == NULL)){
							if(tks_befr[lead_tok_indx+1]->type != STRING)
								err_flag=1;
						}	
						else
							err_flag=1;
						break;
					case D_DIRECTIVE:
						if (size_tok_before ==(2+lead_tok_indx) && tks_befr[lead_tok_indx+1]->type == NUMBER){ /* if part before comma is legal*/
							if (size_tok_after != -1){/*if part after comma is legal and not empty*/
								for(i=0;i<size_tok_after;i++){
									if (tks_aftr[i]->type!=NUMBER)
										err_flag=1;
								}
							}
							else
								err_flag=1;
						}
						else
							err_flag = 1;
						break;
					case LABEL_OR_INSTRUCTION:
						if (size_tok_before == (1+lead_tok_indx)){
							if (size_tok_after == 0){ /* if format is : (LABEL:) X */

								/*											tag(X)                 		*/
								if (!(is_legal_instrct(instrcTable,tks_befr[lead_tok_indx]->tag,NULL,NULL)))
									err_flag=1;
							}
							else if (size_tok_after == 1){ /*-""- (LABEL:) X,Y */
								err_flag = 1;
							}
						}
						else if (size_tok_before == (2+lead_tok_indx)){
							if (size_tok_after == 1){  /* -""- (LABEL:) X Y ,Z */

								/*											tag(X)                 		         tag(Y)               tag(Z)      */
								if (!(is_legal_instrct(instrcTable,tks_befr[lead_tok_indx]->tag,tks_befr[size_tok_before-1]->tag,tks_aftr[0]->tag)))
									err_flag=1;
							}
							else if (size_tok_after == 0){  /*-""- (LABEL:) X Y  */

								/*											tag(X)                  tag(Y)		     */
								if (!(is_legal_instrct(instrcTable,tks_befr[lead_tok_indx]->tag,tks_befr[size_tok_before-1]->tag,NULL)))
									err_flag=1;
							}
							else
							{
								err_flag = 1;
							}
						}
						else
								err_flag =1;
						break;
						default:
						break;
					}

				}
			dynamic_array_free((void**)tks_befr,size_tok_before,(void (*)(void*))token_free);
			dynamic_array_free((void**)tks_aftr,size_tok_after,(void (*)(void*))token_free);
		}
		else
			err_flag=1;
		
		if (err_flag)
			return -1;

		size_tok_before=str_to_tags(line,", \t",&token_tags);
		*toks = analyze_tokens(token_tags,size_tok_before);

		dynamic_array_free((void**)token_tags,size_tok_before,free);

		return size_tok_before;
		
}

