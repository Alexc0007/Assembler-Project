/*
 * assembler.h
 *
 *  Created on: Mar 11, 2016
 *      Author: Alex Cherniak & Genady Mager
 */

#ifndef ASSEMBLER_H_
#define ASSEMBLER_H_
#include<stdio.h>


#define ERR_LINE_LEN 1000 /*maximum error line length*/



/*============= Prototypes =========================*/

int assemble(FILE*);
void second_run(FILE* , char[]);

#endif /* ASSEMBLER_H_ */
