/*
 * assembler.c
 *
 *  Created on: Mar 2, 2016
 *      Author: Alex Cherniak & Genady Mager
 *
 *  ABOUT: the program(Assembler) will receive files that contain assembly langugae for
 *  a specific machine - as described in the Project Manual and will translate
 *  the commands written in the input files into Machine Code
 *
 *  the Assembler will print out the Code and Data in base 32 in the .prog file
 *  will print the entry labels in .entry file
 *  and will print the external labels in the .external file
 */
/*============== Included Libraries ======================*/
#include <stdio.h>
#include <stdlib.h>

/*================ Included Files ========================*/
#include "models.h"
#include "assembler.h"
#include "firstPass.h"
#include "Utilities.h"




int main(int argc , char* argv[])
{
	int i;
	int result;
	FILE* fp;
	if(argc == 1)/*no file names inserted as command line arguments*/
	{
		fp = fopen(errName , "a");
		if(fp != NULL)
		{
			fprintf(fp , "%s" , "no input files\n");/*write error message to log*/
			exit(1);
		}
	}

	for(i=1 ; i<argc ; i++)/*loop to read all files from command line arguments*/
	{
		fp = fopen(argv[i] , "r");
		if(fp != NULL)/*if file open is OK - start process*/
		{
			makeErrorFileName(argv[i]);
			result = assemble(fp); /*call to assemble function that will start the process*/
			if (result == 0)
			{
				second_run(fp , argv[i]);
			}
			else
			{
				fp = fopen(errName , "a");
				if(fp != NULL)
				{
					fprintf(fp , "%s %s %s" , "first pass ended with errors on file: " ,argv[i], ", second pass will not run \n");
					fclose(fp);
				}
			}
		}
		else /*couldnt open file*/
		{
			fp = fopen(errName , "a");
			if(fp != NULL)
			{
				fprintf(fp , "%s %s" , "cant open file:" ,argv[i]);
				/*write error log*/
				fclose(fp);
			}
		}
	}
	/*===============   free all linked lists ==================*/
	freeList(&symbolHead);
	freeList(&codeHead);
	freeList(&dataHead);
	freeList(&internalLabelList);
	/*============= End ========================*/
	return 0;
}
