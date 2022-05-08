project: word.o assembler.o main.o instrct.o syntax.o 
	gcc -Wall -ansi -pedantic -o project main.o word.o assembler.o table.o syntax.o instrct.o helperFunctions.o 
main.o: assembler.o word.o main.c instrct.o table.o
	gcc -Wall -ansi -pedantic -c main.c
assembler.o: assembler.c word.o syntax.o
	gcc -Wall -ansi -pedantic -c assembler.c
word.o: word.c 
	gcc -Wall -ansi -pedantic -c word.c
instrct.o: instrct.c word.o
	gcc -Wall -ansi -pedantic -c instrct.c
table.o: table.c helperFunctions.o
	gcc -Wall -ansi -pedantic -c table.c
helperFunctions.o: helperFunctions.c 
	gcc -Wall -ansi -pedantic -c helperFunctions.c
syntax.o: syntax.c helperFunctions.o
	gcc -Wall -ansi -pedantic -c syntax.c


