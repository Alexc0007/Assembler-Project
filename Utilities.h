/*
 * Utilites.h
 *
 *  Created on: Feb 27, 2016
 *      Author: Alex Cherniak & Genady Mager
 */
#include<stdio.h>
#include "models.h"

#ifndef UTILITIES_H_
#define UTILITIES_H_


#define DECIMAL 10
#define FOREVER for(;;) /*endless loop*/
#define ERR_NAME_LEN 8 /*length of standard error log name*/

/*==================== Global Variables ========================*/
extern char errName[];

/*====================== Macro ==============================*/
/**
 * macro to check memory allocation
 */

 #define CHECK_MEM_ALLOC(ptr)\
{\
	if(ptr == NULL)\
	{\
		fprintf(stderr , "cannot allocate memory \n");\
		exit(0);\
	}\
\
}


/*=======================PROTOTYPES==========================*/
char* skipSpaces(char*);
char* removeSpaces(char*);
int checkEndLine(char*);
char* lookForString(char*);
int* getData(char* , int* , int*);
char* skipToEnd(char*);
nodeptr getRandomNode(nodeptr);
void makeErrorFileName(char*);
void clrErrFileName();
void freeList(nodeptr*);










#endif /* UTILITIES_H_ */
