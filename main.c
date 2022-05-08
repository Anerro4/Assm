#include "assembler.h"
#include "memory.h"
#include "instrct.h"
#include "table.h"
#include "helperFunctions.h"
#include <stdio.h>



int main(int argc, char *argv[]) {
	Assembler* s = assmbl_new();
	assmbl_start(s,argc-1,argv+1); 	/*argv+1 since argv contains the program*/	
	assmbl_free(s);			/*name*/
	return 0;		
}
