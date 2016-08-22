/*

 * firstPass.c
 *
 *  Created on: Mar 2, 2016
 *      Author: alexc007
 */

/*==================== Included Files ========================*/
#include "Utilities.h"
#include "models.h"
#include "firstPass.h"
#include "second.h"

/*==================== Included Libraries =====================*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int validFlag=0; /*a flag to validate operands*/
int lineNumber; /*line number counter*/
int IC; /*instruction counter*/
int DC; /*data counter*/


int data[MAX_LINE_LEN];/*array to hold data that will be inserted to dataTable*/
int* dataPointer;
int dataAmount; /*amount of data to be inserted to dataTable*/
char string[MAX_LINE_LEN]; /*an array to use in functions*/
char* newPos; /*command line pointer that will get new position when needed - used in functions*/


/*
 * a function that will start the first run
 * Param: a file pointer
 * will return: 0 if successfull without errors
 * 			   -1 if finished with errors
 */
int assemble(FILE* fp)
{
	int counter =0; /*label length counter*/
	int symbolFlag =0; /*flag that indicates that there is a label*/
	command cmd;
	FILE* pointer;
	char cmdLine[MAX_LINE_LEN];/*array that will hold the command line after its read*/
	char* line; /*pointer that will run on command line*/
	char* secOPpos; /*pointer to point to second operand - on step 13*/
	newPos = cmdLine;
	/*step1 - initialize IC and DC*/
	IC = 100; /*initialize Instruction Counter*/
	DC = 0; /*initialize Data Counter*/
	lineNumber = 0; /*initialize Line Number Counter*/
	FOREVER
	{
		symbolFlag = 0; /*reset symbol flag for next line*/
		counter = 0; /*reset label length counter for next line*/
		validFlag = 0; /*reset flag that validates the operands for the next line*/
		/*step2 - read a line*/
		if(fgets(cmdLine , MAX_LINE_LEN , fp) == NULL)/*reached end of file*/
		{
			break;/*end loop*/
		}
		line = cmdLine;
		line = skipSpaces(line);
		if((*line != COMMENT_CHAR) && (*line != NEW_LINE))/*if first char is comment char or empty line - skip whole codeline*/
		{

			/*check line for not viable label existance*/
			if(checkLabel(line) < 0)
			{
				pointer = fopen(errName , "a"); /*open error log*/
				switch(checkLabel(line))
				{
				case -1: /*label is too long*/

					fprintf(fp , "%s %d %s" , "line: ", lineNumber , "label is too long\n");
					/*generate error message to error log*/
					break;

				case -2: /*found space in the middle of label*/

					fprintf(fp , "%s %d %s" , "line: ", lineNumber , "label incorrectly defined\n");
					break;

				case -3: /*label has the same name as command*/

					fprintf(fp , "%s %d %s" , "line: ", lineNumber , "label has command name\n");
					break;

				case -4: /*label has the same name as register*/

					fprintf(fp , "%s %d %s" , "line: ", lineNumber , "label has register name\n");
					break;

				case -5: /*label is already in symbol table*/

					fprintf(fp , "%s %d %s" , "line: ", lineNumber , "label has already been used\n");
					break;
				}
				fclose(pointer);
			}
			/*checkLabel was ok and can continue*/
			if(checkLabel(line) == 1)
			/*means there is a viable symbol in line - step 3 */
			{
				symbolFlag = 1; /*set flag that there is a symbol - step 4*/
				while(line[counter] != LABEL_END_CHAR)
				{
					counter++;
				}
				counter++;
			}
			if(checkInstruction(line+counter , &newPos) == (DATA )|| checkInstruction(line+counter , &newPos) == (STRING))/*step 5*/
			{
				if(symbolFlag == 1)/*there is a symbol - step 6*/

					/* symbol should be now inserted into the symbol table
					 * and its address is DC
					 */
				{
					getLabel(line,&newPos); /*get label and insert it to symbol table*/
					setSymbolLastItemAddress(&symbolHead , DC); /*insert address to label*/
					setSymbolLastItemLabelType(&symbolHead , NOT_EXTERNAL , HAS_NO_FUNC);
					line = newPos;
					counter = 0;/*reset the counter for maybe future purpose of step 7*/
					line = skipSpaces(line);
				}
				/*step 7 - identify type of data , insert to data table , update DC*/
				if(checkInstruction(line+counter , &newPos) == DATA) /*instruction is DATA*/
				{
					line = newPos;
					dataPointer = getData(line , data , &dataAmount); /*get the data numbers
					into the data array*/
					storeNumsToData(dataPointer , dataAmount);/*store the data to dataTable*/
					line = skipToEnd(newPos); /*skip to the end of the line*/
				}
				else/*instruction is STRING*/
				{
					line = newPos;
					clrStringArray(); /*clear the string array in case its not empty*/
					strcpy(string , lookForString(line));
					if(string != NULL)/*make sure string is not empty*/
					{
						line = storeStringToData(string , line);/*after this function
						line will point 1 char after the string in the command line*/
						if(checkEndLine(line) == 1) /*means the rest of the command line is clear*/
						{
							line = skipToEnd(line);
						}
						else
						{
							pointer = fopen(errName , "a");
							fprintf(fp , "%s %d %s" , "line: ", lineNumber , "string incorrectly defined\n");
							/*generate error message to error log*/
							fclose(pointer);
						}
					}
				}

			}
			else/*check if instruction is entry or extern - step 8*/
			{
				if(checkInstruction(line , &newPos) == (ENTRY) || checkInstruction(line , &newPos)==(EXTERN))
				{
					if(checkInstruction(line , &newPos) == EXTERN)/*step 9*/
					{
						/*instruction is extern and now need to find symbol as param*/
						clrStringArray();
						strcpy(string , getExternLabel(newPos));/*string gets label*/
						if(string != NULL)/*means all ok*/
						{
							storeExternLabel(string);
							/*store extern label without address - step 9 comlete*/
						}
					}
				}
				else/* instruction is not ENTRY or EXTERN - step 11
				so instruction can be a command - not a directive or be an error*/
				{
					if(symbolFlag == 1)/*means there is a symbol*/
					{
						getLabel(line , &newPos); /*get the label and store it in the dataTable*/
						setSymbolLastItemAddress(&symbolHead , IC);/*set the label's address to be IC*/
						/*now need to search the commands array to see if
						 * instruction is a command - step 12*/
						line = newPos;
					}
					line = skipSpaces(line);
					cmd = getCommand(line , &newPos);
					if(strcmp(cmd.name , "inv") == 0)/*means command is invalid*/
					{
						pointer = fopen(errName , "a");
						fprintf(fp , "%s %d %s" , "line: ", lineNumber , "invalid command\n");
						/*generate error message to error log*/
						fclose(pointer);
					}
					else/*its a valid command and we need to calculate
					how many words of memory it requires - step 13*/
					{
						if(symbolFlag ==1)/*only if has label*/
						{
							setSymbolLastItemLabelType(&symbolHead , NOT_EXTERNAL , HAS_FUNC);
						}
						/* a switch case to handle the
						 * number of operands of a command
						 * and will determine how to increase IC
						 */
						switch(cmd.operands)
						{
						case 0:
							line = newPos;
							secOPpos = line;
							OPERAND_VALIDATION(operandValidation(line , secOPpos , cmd), validFlag);
							if(validFlag == 1)
							{
								IC++;
							}
							break;
						case 1:
							line = newPos;
							line = skipSpaces(line);
							secOPpos = line;
							OPERAND_VALIDATION(operandValidation(line , secOPpos , cmd), validFlag);
							if(validFlag == 1)
							{
								IC+=2;
							}
							break;
						case 2:
							line = newPos;
							/*in this case we need to test operands*/
							line = skipSpaces(line);
							/*line will point on the first char of the first operand*/
							secOPpos = line;
							while(*secOPpos != PARAM_SEPARATOR && *secOPpos != '\n')
								secOPpos++;
							/*could be that there should be a second operand but there is not
							 * that case would generate an error - lets check
							 */
							if(*secOPpos == '\n')/*no param separator found*/
							{
								pointer = fopen(errName , "a");
								fprintf(fp , "%s %d %s" , "line: ", lineNumber , "incorrect number of operands\n");
								/*generate error message to error log*/
								fclose(pointer);
							}
							else/*secOPpos will point to the param separator*/
							{
								secOPpos++; /*advance secOPpos by 1*/
								secOPpos = skipSpaces(secOPpos); /*skip spaces*/
								/*now secOPpos should definitly point to the second operand*/
								if(operandValidation(line , secOPpos , cmd) ==1) /*means operands are ok but not both registers*/
								{
									OPERAND_VALIDATION(operandValidation(line , secOPpos , cmd), validFlag);
									/*OPERAND_VALIDATION will ensure that operands are valid and dont have an error*/
									if(validFlag == 1)
									{
										/*means that both operands are not registers*/
										IC +=3;/*advance IC*/
									}

								}
								else
								{
									OPERAND_VALIDATION(operandValidation(line , secOPpos , cmd), validFlag);
									/*OPERAND_VALIDATION will ensure that operands are valid and dont have an error*/
									if(validFlag == 1)
									{
										/*means that both operands are registers*/
										IC+=2;/*advance IC*/
									}
								}
							}


						}
					}
				}
			}
		}

		/*check for not exceeding memory size*/
		if(IC + DC >IC+MEM_SIZE)
		{
			fprintf(stderr , "assembler exceeded memory size \n");
			exit(0); /*stop program*/
		}
		lineNumber++; /*advance lineNumber counter*/
	}
	pointer = fopen(errName , "r"); /*try to open error log to check
	if errors were written , since file must exist , if its not there
	means that there are no errors*/

	if(pointer != NULL) /*means errors were written*/
	{
		return -1;
	}
	return 0;
}

