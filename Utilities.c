/*
 * Utilities.c
 *
 *  Created on: Mar 1, 2016
 *      Author: Alex Cherniak & Genady Mager
 */

/*======================= Included Files ======================*/
#include "Utilities.h"
#include "models.h"
#include "firstPass.h"

/*======================== Included Libraries ===================*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>



char errName[MAX_LINE_LEN] = "err_log ";

/*
 * a function that will get an array of chars that holds a command line
 * and cut the spaces out of it
 * will return pointer to an array that holds the command line without spaces
 */
char* removeSpaces(char* string)
{

	char* output = string;
	unsigned int i=0, j=0;
	if(string == NULL)/*check if string is empty*/
	{
		return NULL;
	}
	for(i=0 , j=0 ; i<strlen(string) ; i++ , j++)
	{
		if(string[i] != ' ')
			output[j] = string[i];
		else
			j--;
	}
	output[j] = 0; /*end string with NULL terminator*/
	return output;
}

/*
 * a void function that will skip spaces and tabs in line when activated
 */
char* skipSpaces(char* line)
{
	while(*line)
	{
		if((line[0] != ' ') && (line[0] != TAB))
			break;
		else
			line++;
	}
	return line;
}

/*
 * a function that looks for string and return a pointer to an array
 * containing that string if viable string is in the command line
 * else return null
 */
char* lookForString(char* line)
{
	FILE* fp;
	int i=0;
	char* result;
	line = skipSpaces(line);
	if(line[0] == QUOTE_MARK)
	{
		if(line[1] == QUOTE_MARK)/*means string is empty*/
			return NULL;
		line++;
		while(*line != QUOTE_MARK)
		{
			string[i] = *line;
			i++;
			line++;
		}
		result = string;
		return result;
	}
	else/*if the first char is not '"'
	it means the string is incorrectly defined*/
	{
		fp = fopen(errName , "a");
		if(fp != NULL)
		{
			fprintf(fp , "%s %d %s" , "line: ", lineNumber , "string incorrectly defined \n");
			/*generate error message to err_log*/
			fclose(fp);
		}
		return NULL;
	}
}

/*
 * function return 1 if from where its called and to the end of the line
 * there are only spaces left
 * if a char other than space is found , return 0
 */
int checkEndLine(char* line)
{
	line = skipSpaces(line); /*skip all the spaces*/
	if(*(line) == NEW_LINE)/*the \n comes from fgets and must be there before the line finishes*/
		return 1;
	else return 0;
}

/*
 * a function that will skip to end of line
 * - will return pointer to the end of the line
 */
char* skipToEnd(char* line)
{
	while(*line != NEW_LINE) /* '\n' comes from fgets*/
	{
		line++;
	}
	return line;
}

/*
 * a function that will return an array of number that should be store as data
 * useful when found that command is .data type and have to store numbers
 * into the data table
 */
int* getData(char* line , int* data , int* dataAmount)
{
	int* output = data;
	int i=0;
	int temp;
	FILE* fp;
	char* token;
	char* endptr;
	line = skipSpaces(line); /*skip all starting spaces to get to the data*/
	line = removeSpaces(line);/*remove all the rest of spaces between data
	makes it easier to work with*/

	token = strtok(line , ",\n");
	while(token != NULL)
	{

		temp = strtol(&token[0] , &endptr , DECIMAL); /*temp gets the converted number*/
		if(*endptr != 0) /*means the number tha token points to is not defined correctly*/
		{
			fp = fopen(errName , "a");
			fprintf(fp , "%s %d %s" , "line: " , lineNumber , "data incorrectly define \n");
			/*generate error message to error log*/
			fclose(fp);
		}
		output[i] = temp;
		i++; /*advance the output array*/
		token = strtok(NULL , ",\n"); /*advance to the next token */
	}
	*dataAmount = i; /*pass the number of data elements that are in the array*/
	return output;
}



nodeptr getRandomNode(nodeptr head)
{
	nodeptr result;
	nodeptr temp;
	int i=0, j=0;
	int random;
	if(head == NULL) /*empty list*/
	{
		return NULL;
	}
	temp = head;
	while (temp->next != NULL) /*count the number of nodes in list*/
	{
		temp = temp->next;
		i++;
	}
	i++; /*i will hold the number of nodes in list*/
	random = rand() / (RAND_MAX / i+1); /*generate random int between 0 and i*/
	result = head;
	for(j=0 ; j<random ; j++)
	{
		result = result->next;
	}
	return result;
}
/*
 * a fucntion that will create the error log file name for each input file that
 * is read by the assembler
 */
void makeErrorFileName(char* fname)
{
	clrErrFileName();
	strcat(errName , fname); /*create the new file name according to the input file*/
}
/*
 * a function to clear the error log name array and leave only the "err_log "
 * at the begining before adding the file name that error log refers to
 */
void clrErrFileName()
{
	int i;
	for(i=ERR_NAME_LEN ; i<MAX_LINE_LEN ; i++)
	{
		errName[i] = 0;
	}
}
/*
 * a function that will free a linked list
 */
void freeList(nodeptr* head)
{
	nodeptr current;
	nodeptr temp;
	if(*head == NULL) /*empty list case*/
	{
		return;
	}
	temp = *head; /*assign temp to be head of list*/
	while(temp != NULL)
	{
		current = temp;
		temp = temp->next;
		free(current);
	}
}
