assembler: assembler.o firstPass.o models.o second.o Utilities.o assembler.h firstPass.h models.h second.h Utilities.h
	gcc -g -Wall -ansi -pedantic assembler.o firstPass.o models.o second.o Utilities.o -o assembler -lm

assembler.o: assembler.c assembler.h models.h firstPass.h Utilities.h
	gcc -c -Wall -ansi -pedantic assembler.c -o assembler.o

firstPass.o: firstPass.c firstPass.h models.h Utilities.h second.h
	gcc -c -Wall -ansi -pedantic firstPass.c -o firstPass.o

models.o: models.c models.h Utilities.h firstPass.h
	gcc -c -Wall -ansi -pedantic models.c -o models.o

second.o: second.c second.h models.h Utilities.h firstPass.h
	gcc -c -Wall -ansi -pedantic second.c -o second.o

Utilities.o: Utilities.c Utilities.h models.h firstPass.h
	gcc -c -Wall -ansi -pedantic Utilities.c -o Utilities.o
