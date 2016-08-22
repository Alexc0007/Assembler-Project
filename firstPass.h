/*
 * firstPass.h
 *
 *  Created on: Mar 10, 2016
 *      Author: Alex Cherniak & Genady Mager
 */

#ifndef FIRSTPASS_H_
#define FIRSTPASS_H_



/*============ GLOBAL VARIABLES ==================*/
extern int IC;
extern int DC;
extern int lineNumber;
extern char string[];


/*============= PROTOTYPES ======================*/
int assemble(FILE*);

/*================= MACRO =========================*/
#define OPERAND_VALIDATION(num , flag)\
{\
	flag = 0;\
	if(num != -1) /*operands are valid*/\
		flag = 1;\
}

#endif /* FIRSTPASS_H_ */
