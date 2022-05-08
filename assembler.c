#include "assembler.h"

typedef enum Line_Error{
	NOERR,
	SYNERR,
	REPDEF,
	UKNOWNCOM	
} Error;

struct Assembler{
	InstrctLst* instImage;
	Word* dataImage;
	Table* symbolTable;
	Table* instrctTable;
	Word psw;
	int dc;
	int ic; 	
};

void print_error(Error type,int line_num)
{
	switch(type){
		case SYNERR:
			printf("Syntax error at line %d\n",line_num);
		break;
		case REPDEF:
			printf("Repeating label defintion at line %d\n",line_num);
		break;
		default:
		break;
	}
}

void load_instruction(Table* tbl,uint opcod,uint funct,uint paranum,uint allowed,char* label){
	InstrctInfo* toAdd;
	toAdd = instrctinfo_new(opcod,funct,paranum,allowed);
	table_add(tbl,label,toAdd);
	free(toAdd);
}

void load_instructions(Table* tbl){
	load_instruction(tbl,0,0,2,331,"mov");
	load_instruction(tbl,1,0,2,363,"cmp");
	load_instruction(tbl,2,10,2,331,"add");
	load_instruction(tbl,2,11,2,331,"sub");
	load_instruction(tbl,4,0,2,322,"lea");
	load_instruction(tbl,5,10,1,522,"clr");
	load_instruction(tbl,5,11,1,522,"not");
	load_instruction(tbl,5,12,1,522,"inc");
	load_instruction(tbl,5,13,1,522,"dec");
	load_instruction(tbl,9,10,1,518,"jmp");
	load_instruction(tbl,9,11,1,518,"bne");
	load_instruction(tbl,9,12,1,518,"jsr");
	load_instruction(tbl,12,0,1,522,"red");
	load_instruction(tbl,13,0,1,523,"prn");
	load_instruction(tbl,14,0,0,528,"rts");
	load_instruction(tbl,15,0,0,528,"stop");
}

int add_symbol(Assembler *asmblr,char* tag,Symbol* sym){
	if (table_lookup(asmblr->symbolTable,tag) == NULL)			
	{				
		table_add(asmblr->symbolTable,tag,sym);
		return 1;	
	}
	return 0;
}


Assembler* assmbl_new(){
	Assembler* assm = (Assembler*)malloc(sizeof(Assembler)); 
	if (assm == NULL){	
		printf("error allocating memory in assmbl_new");
		exit(3);
	}
	assm->dataImage=NULL;
	assm->dc = 0;
	assm->ic = IC_START;
	assm->symbolTable = table_new(SYMBOL);
	assm->instrctTable = table_new(INSTRCT_NFO);
	load_instructions(assm->instrctTable);
	return assm;
}

void addw_to_dimage(Assembler *asmblr,uint value){
	int dataSize = asmblr->dc;
	asmblr->dataImage=(Word*)realloc(asmblr->dataImage,(dataSize+1)*sizeof(Word));
	word_set(&asmblr->dataImage[dataSize],value);
	asmblr->dc++;	
}

int first_pass(Assembler *asmblr,char *fileName){
	int i;
	int j;
	int lead_tok_index;
	int err_flag=0;
	Error line_err_type = 0;
	FILE * fp;
	Symbol* newSymbol;
	char* tempStr = NULL;
	char line [80];
	int linecnt = 0;
	Token** tokens = NULL;
	int size_tok=-1;	
	Instrct* instrct_toadd;
	InstrctInfo* instrct_nfo;

	fp = fopen(fileName, "r");
	if (fp == NULL){
		printf("error opening input file %s",fileName);
		exit(3);
	}
	while (fgets(line, sizeof(line), fp)) {
		linecnt ++;
		line_err_type = NOERR;
		line[strcspn(line, "\n")] = 0;

		tokens = NULL;

		size_tok=check_syntax(line,asmblr->instrctTable,&tokens);
		if (size_tok != -1){
			lead_tok_index = (tokens[0]->type == LABELDEF);
			switch(tokens[lead_tok_index]->type){
				case D_DIRECTIVE:
				case S_DIRECTIVE:
					/* If there is a label check if you can add a symbol*/
					if(lead_tok_index == 1){ /*Copy pasted this part instead of extracting function because didnt have time*/
						newSymbol = symbol_new(0,0,0,1,asmblr->dc);
						tempStr=subString(tokens[0]->tag,0,strlen(tokens[0]->tag)-1);
							
						if (!(add_symbol(asmblr,tempStr,newSymbol))){
							line_err_type = REPDEF;
						}

						free(tempStr);
						free(newSymbol);	
					}
					if(line_err_type==NOERR){
						/*Add data to dataimage*/
						for (i=lead_tok_index+1;i<size_tok;i++){
							if(tokens[i]->type == NUMBER){
								addw_to_dimage(asmblr,atoi(tokens[i]->tag));
							}
							if(tokens[i]->type == STRING){
								tempStr = subString(tokens[i]->tag,1,strlen(tokens[i]->tag)-2);
								for (j=0;j<strlen(tempStr);j++)
									addw_to_dimage(asmblr,(int)tempStr[j]);
								addw_to_dimage(asmblr,0);
								free(tempStr);
							}	
					
		
						}
					}

				break;
				case EX_DIRECTIVE:
						newSymbol = symbol_new(1,0,0,0,0);

						if (!(add_symbol(asmblr,tokens[lead_tok_index+1]->tag,newSymbol))){
							if(!((Symbol*)(table_lookup(asmblr->symbolTable,tokens[lead_tok_index+1]->tag)->value))
									->external)
								line_err_type = REPDEF;
						}

						free(newSymbol);
				break;
				case LABEL_OR_INSTRUCTION:
					if(lead_tok_index == 1){
						newSymbol = symbol_new(0,1,0,0,asmblr->ic);
						tempStr=subString(tokens[0]->tag,0,strlen(tokens[0]->tag)-1);

						if (!(add_symbol(asmblr,tempStr,newSymbol))){
							line_err_type = REPDEF;
						}

						free(tempStr);
						free(newSymbol);
					}
					if(line_err_type==NOERR){
						instrct_nfo = (InstrctInfo*)table_lookup(asmblr->instrctTable,tokens[lead_tok_index]->tag)->value;
						instrct_toadd = instrct_new(instrct_nfo->funct,instrct_nfo->opcode,0,0,instrct_nfo->paranum);
						instrctLst_add(asmblr->instImage,instrct_toadd);
						asmblr->ic+=instrct_nfo->paranum+1;
						instrct_free(instrct_toadd);
					}
				break;
				default:
				break;

			}
			dynamic_array_free((void**)tokens,size_tok,(void (*)(void*))token_free);
		}
		else
			line_err_type = SYNERR;

		if (line_err_type != NOERR){
			print_error(line_err_type,linecnt);
			err_flag = 1;
		}

	}
	fclose(fp);
	if (!err_flag)
		return asmblr->ic;
	else
		return -1;
}

void second_pass(Assembler *asmblr,char *fileName,int icf){
	int i;
	Word* w;
	InstrctNode* itr = asmblr->instImage->start;
	int lead_tok_index;
	FILE * fp;
	char** token_tags;
	char line [80];
	int linecnt = 0;
	Token** tokens = NULL;
	int size_tok=-1;
	TokenType arg_type;
	tblEntry* entry;
	Symbol* smb;

	fp = fopen(fileName, "r");
	if (fp == NULL){
		printf("error opening input file %s",fileName);
		exit(3);
	}
	asmblr->ic = 100;
	while (fgets(line, sizeof(line), fp)) {
		linecnt ++;
		line[strcspn(line, "\n")] = 0;

		size_tok = str_to_tags(line,", \t",&token_tags);
		tokens = analyze_tokens(token_tags,size_tok);
		dynamic_array_free((void**)token_tags,size_tok,free);

		lead_tok_index = (tokens[0]->type == LABELDEF);
		switch(tokens[lead_tok_index]->type){
			case LABEL_OR_INSTRUCTION:
					for (i =1+lead_tok_index;i<size_tok;i++){
						w = (Word*)malloc(sizeof(Word));
						arg_type = tokens[i]->type;
						switch(arg_type){
							case IMMIDIATE:
								word_set(w,atoi(&tokens[i]->tag[1]));
								instrct_set_dataword(itr->data,i-1-lead_tok_index,w);

								break;
							case REGISTER:
								word_set(w,pow2(atoi(&tokens[i]->tag[1])));
								instrct_set_dataword(itr->data,i-1-lead_tok_index,w);
							break;
							case LABEL_OR_INSTRUCTION:
								entry = table_lookup(asmblr->symbolTable,tokens[i]->tag);
								if (entry == NULL)
									printf("Error unkown symbol %s at line %d\n",tokens[i]->tag,linecnt);
								else{
									smb = (Symbol*)entry->value;
									if (smb->data)
										word_set(w,((Symbol*)entry->value)->address+icf);
									else{
										word_set(w,((Symbol*)entry->value)->address);
									}

									instrct_set_dataword(itr->data,i-1-lead_tok_index,w);
								}

							break;
							case RELATIVE:
								entry = table_lookup(asmblr->symbolTable,&tokens[i]->tag[1]);
								smb = (Symbol*)entry->value;
								if (smb->data){
									word_set(w,(smb->address+icf)-asmblr->ic-1);

								}
								else{
									word_set(w,(smb->address)-asmblr->ic-1);
								}
								instrct_set_dataword(itr->data,i-1-lead_tok_index,w);
							break;
							default:
							break;
						}
						free(w);
					}
					asmblr->ic+=itr->data->paraNum+1;

					itr = itr->next;


					/*instrct_nfo = (InstrctInfo*)table_lookup(asmblr->instrctTable,tokens[lead_tok_index]->tag)->value;
					instrct_toadd = instrct_new(instrct_nfo->funct,instrct_nfo->opcode,0,0,instrct_nfo->paranum);
					instrctLst_add(asmblr->instImage,instrct_toadd);
					instrct_free(instrct_toadd);*/
			break;
			default:
			break;
		}

		dynamic_array_free((void**)tokens,size_tok,(void (*)(void*))token_free);
	}
	instrctLst_print(asmblr->instImage);
	for (i=0;i<asmblr->dc;i++)
		word_print(&asmblr->dataImage[i]);
	fclose(fp);

}
void assmbl_translate(Assembler *asmblr,char *fileName){
	int icf = first_pass(asmblr,fileName);
	if (icf != -1){
		second_pass(asmblr,fileName,icf);
	}
}

void assmbl_start(Assembler *asmblr,int fileNum,char *fileNames[]){
	int i;
	asmblr->instImage = (InstrctLst*) malloc(sizeof(InstrctLst));
	instrctLst_init(asmblr->instImage);
	for (i=0;i<fileNum;i++){
		assmbl_translate(asmblr, fileNames[i]);	
	}		
}

void assmbl_free(Assembler *asmblr){
	table_free(asmblr->symbolTable);
	table_free(asmblr->instrctTable);
	instrctLst_free(asmblr->instImage);
	free(asmblr->dataImage);
	free(asmblr);
}
