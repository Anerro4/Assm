#include "instrct.h"

/*************************************************************************/
/*************************************************************************/

Word* instrct_toWords(Instrct* inst){
	uint temp;
	int i;
	Word* result = (Word*)malloc(sizeof(Word)*(1+inst->paraNum));
	temp = inst->dAddr;
	temp += inst->sAddr<<2;
	temp += inst->funct<<4;
	temp += inst->opcode<<8;
	word_set(&result[0],temp);
	for (i=0;i<inst->paraNum;i++)
		word_copy(&inst->dWords[i],&result[i+1]);
	return result;	
}

int instrct_set_dataword(Instrct* inst,uint index,Word* w){
	if (index < inst->paraNum){
		if(inst->dWords)
			word_copy(w,&(inst->dWords[index]));
		else{
			printf("attempt to access null pointer in Instrct_set_dataword\n");
			exit(3);
		}
		return 1;
	}
	else
		return 0;
}


void instrct_free(Instrct* inst){
	free(inst->dWords);
	free(inst);
}

Instrct* instrct_new(uint opcode,uint funct,uint sAddr,uint dAddr,uint paraNum){
	Instrct* inst;
	int i;
	inst = (Instrct*)malloc(sizeof(Instrct));
	if (inst==NULL){
		printf("Memory allocation failed in Instrct_new");
		exit(3);
	}
	inst->funct = funct;
	inst->opcode = opcode;
	inst->sAddr = sAddr;
	inst->dAddr = dAddr;
	inst->paraNum = paraNum;
	inst->dWords = (Word*)malloc(paraNum*sizeof(Word));
	
	for (i=0;i<paraNum;i++)
		word_set(&inst->dWords[i],0);
	if (inst->dWords==NULL){
		printf("Memory allocation failed in Instrct_new");
		exit(3);
	}

	return inst;
}

Instrct* instrct_copy(Instrct* inst){
	Instrct* newInst;
	int i =0;

	newInst=instrct_new(inst->opcode,inst->funct,inst->sAddr,inst->dAddr,inst->paraNum);

	for(i=0;i<inst->paraNum;i++)
		newInst->dWords[i]=inst->dWords[i];
	return newInst;
}

void instrct_print(Instrct* inst){
	int i;
	Word* words;
	words = instrct_toWords(inst);
	for (i=0;i<inst->paraNum+1;i++){
		word_print(&words[i]);
	}
	free(words);

}

/*************************************************************************/
/*************************************************************************/



void instrctLst_init(InstrctLst *lst){
	lst->start = NULL;
	lst->last = NULL;			
}

void instrctLst_free(InstrctLst *lst){
	InstrctNode* ptr = lst->start;
	InstrctNode* temp;
	while (ptr){
		temp = ptr->next;
		instrct_free(ptr->data);
		free(ptr);
		ptr = temp;
	}
	free(lst);
}

void instrctLst_add(InstrctLst *lst,Instrct* data){
	if (lst->last){
		lst->last->next = (InstrctNode*)malloc(sizeof(InstrctNode));
		lst->last->next->data = instrct_copy(data);
		lst->last->next->next = NULL;
		lst->last = lst->last->next;
	}
	else{
		lst->last = (InstrctNode*)malloc(sizeof(InstrctNode));
		lst->last->data = instrct_copy(data);
		lst->last->next = NULL;
		lst->start = lst->last;
	}
}

void instrctLst_print(InstrctLst *lst){
	InstrctNode* itr;
	itr = lst->start;
	while (itr){
		instrct_print(itr->data);
		itr=itr->next;
	}
}


