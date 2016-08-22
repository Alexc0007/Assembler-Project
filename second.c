/*
 * Created on: Mar 2, 2016
 *      Author: Alex Cherniak & Genady Mager
 */


/*====================== Included Libraries ==========================*/
#include <string.h>
#include <math.h>
#include <stdlib.h>
/*====================== Included Files ===============================*/
#include "models.h"
#include "Utilities.h"
#include "second.h"
#include "firstPass.h"

int instC = 100;
/*This is the main function for the second pass  */
void second_run(FILE *f1, char* fname)
{
	int one=1;
   	 char *line = (char*) malloc(sizeof(char)*MAX_LINE_LEN);
	/*................................................... */
	/*Declerations of files, and naming them */
	FILE *entF , *progF;
	fseek(f1, 0, SEEK_SET);
	entF = fopen(set_file_name(fname, ".entry"), "a");
	progF = fopen(set_file_name(fname, ".prog") , "a");
	CHECK_MEM_ALLOC(entF);
	CHECK_MEM_ALLOC(progF);
	/*....................................................*/

    symbol_address_update(); /*Update the symbol addresses */
    makeInternalLabelList(); /*Creating a symbol table without exteranl labels */
    


	while (one)
	{
		command cmd;
		commandWord word;
		one = get_line(line, f1);

		line = line + label_offset(line); /*now the pointer is set to the start of the command.*/
		line =skipSpaces(line);
		if(checkInstruction(line, &newPos)!= 0)
		{
			/* now we know its entry*/
			if(checkInstruction(line, &newPos) == 3)
			{
                int len;
                nodeptr temp;
				newPos = skipSpaces(newPos);
				temp = symbolHead;

				while(temp != NULL)
				{
                    len =(int) strlen(temp->item.symbolItem.label);
					if(strncmp(newPos,temp->item.symbolItem.label, len) ==0)
					{
						int address= temp->item.symbolItem.address;
						fprintf(entF,"%s\t%s \n",import_32(address), temp->item.symbolItem.label );
					}
					temp = temp->next;
				}
			}
		}
		/*This is not an instruction line- means it's a command line */
		else
		{
            memWord m1, m2, mReg;
			nodeptr firstIn = makeNewNode();
			nodeptr secondIn = makeNewNode();
			nodeptr thirdIn = makeNewNode();
			int k=0;
			int result, fop , sop; /*Those are the addresses/ values of the operands  */
			int i=0;
			cmd=getCommand(line, &newPos); /*Now cmd contains the command of the type */
			while(strcmp(cmd.name, commands[i].name)!=0) i++;
			/*.............................................................................................*/
			word.opcode = commands[i].opcode; /*Now word contains the opcode of it's command*/
			word.group= cmd.operands; /* the number of operands for this opcode*/
			word.ARE =0; /*ARE bits for an instruction word is 0 */
			word.notUsed = 0;
			/*.............................................................................................*/
			newPos = skipSpaces(newPos); /*Now it's on the first operand */
			while(*(newPos+k) != ',' && *(newPos+k) != '\n') k++;
			k++;
			while(*(newPos+k) == ' ') k++;
			/*If there is only 1 operand, it's checked like operand num 2 */
			result = set_operands(newPos, newPos+k, &fop, &sop, commands[i]);

			switch(result)
			{
			case 0: {
                        word.sourceAdressing = 0;
                        word.targetAdressing = 0;
                        break;
                    }
			case 1:{

						/*.....................................................................................*/
						mReg.reg.ARE=0;
						mReg.reg.sourceRegAddress = fop;
                        mReg.reg.targetRegAddress = sop;
                        mReg.reg.notUsed = 0;
                        secondIn->item.codeItem.word.bin = reg_to_int(mReg.reg);

						/*.................................................................................... */
						word.sourceAdressing = REGISTER;
						word.targetAdressing = REGISTER;
						word.rnd = 0;
						break;
				   }
			case 2:{
                        /*.....................................................................................*/
						mReg.reg.ARE=0;
						mReg.reg.sourceRegAddress = fop;
						mReg.reg.targetRegAddress = 0;
						mReg.reg.notUsed = 0;
						secondIn->item.codeItem.word.bin = reg_to_int(mReg.reg);
						/*.....................................................................................*/
						m2.data.value = sop;
						if(print_to_extern(instC, newPos + k, 2 ,fname)) m2.data.ARE =1;
						else m2.data.ARE = 2;
						thirdIn->item.codeItem.word.bin = data_to_int(m2.data);
						/*.................................................................................... */
						word.sourceAdressing = REGISTER;
						word.targetAdressing = DIRECT;
						word.rnd = 0;

						break;
					}
			case 3:{
                        /*.....................................................................................*/
						m2.data.value = sop;
						if(print_to_extern(instC, newPos+k,2 , fname )) m2.data.ARE =1;
						else m2.data.ARE = 0;
						secondIn->item.codeItem.word.bin = data_to_int(m2.data);
						/*.....................................................................................*/
						m1.data.value = fop;
						if(print_to_extern(instC, newPos, 1, fname)) m1.data.ARE =1;
						else m1.data.ARE = 0;
						secondIn->item.codeItem.word.bin = data_to_int(m1.data);
						/*.....................................................................................*/
						word.sourceAdressing = DIRECT;
						word.targetAdressing = DIRECT;
						word.rnd = 0;
						break;

					}
			case 4: {
                        /*.....................................................................................*/
						m1.data.value = fop;
						if(print_to_extern(instC, newPos, 1 , fname)) m2.data.ARE =1;
						else m2.data.ARE = 0;
						secondIn->item.codeItem.word.bin = data_to_int(m2.data);
						/*.....................................................................................*/
						mReg.reg.ARE = 0;
						mReg.reg.sourceRegAddress = 0;
						mReg.reg.targetRegAddress = sop;
						thirdIn->item.codeItem.word.bin = reg_to_int(mReg.reg);
						/*.....................................................................................*/
						word.sourceAdressing = DIRECT;
						word.targetAdressing = REGISTER;
						word.rnd = 0;
						break;
					}
			case 5: {
                        /*.....................................................................................*/
                        m1.data.ARE = fop;
                        m1.data.ARE = 0;
                        secondIn->item.codeItem.word.bin = data_to_int(m1.data);
                        /*.....................................................................................*/
						mReg.reg.ARE = 0;
						mReg.reg.sourceRegAddress = 0;
						mReg.reg.targetRegAddress = sop;
						thirdIn->item.codeItem.word.bin = reg_to_int(mReg.reg);
						/*.....................................................................................*/
						word.sourceAdressing = IMMEDIATE;
						word.targetAdressing = REGISTER;
						word.rnd =0;
						break;
					}
			case 6: {
                        /*.....................................................................................*/
                        m1.data.value = fop;
                        m1.data.ARE = 0;
                        secondIn->item.codeItem.word.bin = data_to_int(m1.data);
                        /*.....................................................................................*/
                        m2.data.value = sop;
						if(print_to_extern(instC, newPos, 2 , fname)) m2.data.ARE =1;
						else m2.data.ARE = 0;
						thirdIn->item.codeItem.word.bin = data_to_int(m2.data);
						/*.....................................................................................*/
						word.sourceAdressing = IMMEDIATE;
						word.targetAdressing = DIRECT;
						word.rnd = 0;
						break;
					}
			case 7: {

						/*.....................................................................................*/
						mReg.reg.ARE=0;
						mReg.reg.sourceRegAddress = fop;
                        mReg.reg.targetRegAddress = sop;
                        mReg.reg.notUsed = 0;
                        secondIn->item.codeItem.word.bin = reg_to_int(mReg.reg);
                        /*.....................................................................................*/
						word.sourceAdressing = RANDOM;
						word.targetAdressing = REGISTER;
						word.rnd = 1;
						break;
					}
			case 8: {

                        /*.....................................................................................*/
                        m1.data.value = fop;
                        m1.data.ARE = 0;
                        secondIn->item.codeItem.word.bin = data_to_int(m1.data);
                        /*.....................................................................................*/
						mReg.reg.ARE = 0;
						mReg.reg.sourceRegAddress = 0;
						mReg.reg.targetRegAddress = sop;
						thirdIn->item.codeItem.word.bin = reg_to_int(mReg.reg);
						/*.....................................................................................*/
						word.sourceAdressing = RANDOM;
						word.targetAdressing = REGISTER;
						word.rnd = 2;
						break;
					}
			case 9: {
                        char *symbol = extern_by_address(fop);
                        m1.data.value = fop;
                        if(print_to_extern(instC, symbol, 1 , fname)) m1.data.ARE =1;
						else m1.data.ARE = 2;
						secondIn->item.codeItem.word.bin = data_to_int(m2.data);
                        /*.....................................................................................*/
						mReg.reg.ARE = 0;
						mReg.reg.sourceRegAddress = 0;
						mReg.reg.targetRegAddress = sop;
						thirdIn->item.codeItem.word.bin = reg_to_int(mReg.reg);
						/*.....................................................................................*/
						word.sourceAdressing = RANDOM;
						word.targetAdressing = REGISTER;
						word.rnd = 3;
						break;
					}
			case 10: {

                        /*.....................................................................................*/
						mReg.reg.ARE=0;
						mReg.reg.sourceRegAddress = fop;
						mReg.reg.targetRegAddress = 0;
						mReg.reg.notUsed = 0;
						secondIn->item.codeItem.word.bin = reg_to_int(mReg.reg);
						/*.....................................................................................*/
						m2.data.value = sop;
						if(print_to_extern(instC, newPos + k, 2 , fname)) m2.data.ARE =1;
						else m2.data.ARE = 0;
						thirdIn->item.codeItem.word.bin = data_to_int(m2.data);
						/*.................................................................................... */
						word.sourceAdressing = RANDOM;
						word.targetAdressing = DIRECT;
						word.rnd = 1;
						break;
					 }
			case 11: {
                        /*.....................................................................................*/
                        m1.data.value = fop;
                        m1.data.ARE = 0;
                        secondIn->item.codeItem.word.bin = data_to_int(m1.data);
                        /*.....................................................................................*/
                        m2.data.value = sop;
						if(print_to_extern(instC, newPos, 2 , fname)) m2.data.ARE =1;
						else m2.data.ARE = 0;
						thirdIn->item.codeItem.word.bin = data_to_int(m2.data);
						/*.....................................................................................*/
						word.sourceAdressing = RANDOM;
						word.targetAdressing = DIRECT;
						word.rnd = 2;
						break;
					 }
			case 12: {
                        /*.....................................................................................*/
                        /*This is the proper way to print to the extern file! fix it also in
                         *first operand of case 9 */
                        char *symbol = extern_by_address(fop);
                        m1.data.value = fop;
                        if(print_to_extern(instC, symbol, 1 , fname)) m1.data.ARE =1;
						else m1.data.ARE = 2;
						secondIn->item.codeItem.word.bin = data_to_int(m1.data);
                        /*.....................................................................................*/
                        m2.data.value = sop;
						if(print_to_extern(instC, newPos+ k, 2 , fname)) m2.data.ARE =1;
						else m2.data.ARE = 2;
						thirdIn->item.codeItem.word.bin = data_to_int(m2.data);
						/*.....................................................................................*/
						word.sourceAdressing = RANDOM;
						word.targetAdressing = DIRECT;
						word.rnd = 3;
						break;
					 }
			case 13: {

                        /*.....................................................................................*/
                        m1.data.value = fop;
                        m1.data.ARE = 0;
                        secondIn->item.codeItem.word.bin = data_to_int(m1.data);
                        /*.....................................................................................*/
                        m2.data.value = sop;
                        m2.data.ARE = 0;
                        thirdIn->item.codeItem.word.bin = data_to_int(m2.data);
                        /*.....................................................................................*/
						word.sourceAdressing = IMMEDIATE;
						word.targetAdressing = IMMEDIATE;
						word.rnd = 0;
						break;
					 }
			case 14: {
                        /*.....................................................................................*/
						mReg.reg.ARE=0;
						mReg.reg.sourceRegAddress = 0;
                        mReg.reg.targetRegAddress = sop;
                        mReg.reg.notUsed = 0;
                        secondIn->item.codeItem.word.bin = reg_to_int(mReg.reg);
                        /*.....................................................................................*/
						word.sourceAdressing = 0;
						word.targetAdressing = REGISTER;
						word.rnd = 0;
						break;
					 }
			case 15: {
                        /*.....................................................................................*/
						m2.data.value = sop;
						if(print_to_extern(instC, newPos, 2 , fname)) m2.data.ARE =1;
						else m2.data.ARE = 2;
						secondIn->item.codeItem.word.bin = data_to_int(m2.data);
						/*.................................................................................... */
						word.sourceAdressing = 0;
						word.targetAdressing = DIRECT;
						word.rnd = 0;
						break;
					 }
			case 16: {
                        /*.....................................................................................*/
                        m2.data.value = sop;
                        m2.data.ARE = 0;
                        secondIn->item.codeItem.word.bin = data_to_int(m2.data);
                        /*.....................................................................................*/
						word.sourceAdressing = 0;
						word.targetAdressing = IMMEDIATE;
						word.rnd = 0;
						break;
					 }
				}
			firstIn->item.codeItem.word.command =word;
			firstIn->item.codeItem.word.bin = command_to_int(word);
			firstIn->item.codeItem.address = instC;
            addToList(&codeHead ,firstIn);

			if(word.group == 2)
				{
                    if(result == 1) /*Means there are 2 regs as poerands */
                    {

                        secondIn->item.codeItem.address = instC+1;
                        secondIn->item.codeItem.word = mReg;
                        codeHead = addToList(&codeHead ,secondIn);
                        instC+=2;
                    }
                    else
                    {
                        secondIn->item.codeItem.address = instC+1;
                        if(get_if_register(newPos , 1)>0)
                        {
                        	secondIn->item.codeItem.word = mReg;
                        }
                        else
                        {
                        	secondIn->item.codeItem.word = m1;
                        }
                        thirdIn->item.codeItem.address = instC+2;
                        if(get_if_register(newPos+k , 2)>0)
                        {
                        	thirdIn->item.codeItem.word = mReg;
                        }
                        else
                        {
                        	thirdIn->item.codeItem.word = m2;
                        }
                        codeHead = addToList(&codeHead ,secondIn);
                        codeHead = addToList(&codeHead ,thirdIn);
                        instC+=3;
                    }
				}
                if(word.group == 1)
                {
                    secondIn->item.codeItem.address = instC+1;
                    if(get_if_register(newPos, 2)>0)
                    {
                    	secondIn->item.codeItem.word = mReg;
                    }
                    else
                    {
                    	secondIn->item.codeItem.word = m2;
                    }
                    instC+=2;
                    codeHead = addToList(&codeHead , secondIn);


                }
                if(word.group == 0) instC++;

            }

		}
	print_to_file(progF);
	fclose(entF);
	fclose(progF);
	}


int get_line(char *line, FILE *f1)
{

	if (fgets(line, MAX_LINE_LEN, f1)!= NULL)  /*reads a line from a file.*/
		return 1;
	return 0;

}

/*if there is a label, this function return an integers that sets a us after the ':'*/
int label_offset(char *line)
{
	int i = 0;
	if (checkLabel(line) == 0) return 0;
	else
		while (*(line + i) != ':' ) i++;
	return i+1;
}

/*This function updates the addresses if the variables (.data or .string)
 * in the symbol table. */

void symbol_address_update()
{
    nodeptr temp2 = dataHead;
	nodeptr temp=symbolHead;
	while(temp != NULL)
	{
		/*Here i check if the symbol is external/ function line related. if it's not both, i update the
		 addresses according to the instruction counter. */
		if(temp->item.symbolItem.type.external == 0 && temp->item.symbolItem.type.functionLine==0)
			temp->item.symbolItem.address += IC;
			temp = temp->next;
	}
	while(temp2 != NULL)
	{
			temp2->item.dataItem.address += IC;
			temp2 = temp2->next;
	}
}

/*This function imports addresses to their representation as a base-32 number.
 * it gets an integer and returns a pointer to the string that represents the number in base-32. */
char* import_32(int num_to_convert)
{
	int j=0;
	char *converted;
	int i=0;
	int converted_num[50];
	/*Digits of base-32 */
	char base_32[32]= {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
						'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V'};
	num_to_convert = num_to_convert & WORD_MASK;
    if(num_to_convert == 0) return "0";
    else
    {

	/*The converting method */
	while(num_to_convert != 0)
	{
		converted_num[i] = num_to_convert % 32;
		num_to_convert = num_to_convert / 32;
		++i;

	}

	converted= (char*) malloc(sizeof(char)*(i));
	CHECK_MEM_ALLOC(converted);
	for(; i>0; i--, j++)
	{
		converted[j] =  base_32[converted_num[i-1]];
	}

	return converted;
	}

}

/* ---------------------------------------------------------------------------------------------------------------------- */

/*This function returns number that indicates what types of operands wer'e using,
 * it receives pointers to the operands, and addresses of two integer,
 *  witch we will store the values of the operands inside , and also a number indicating how
 *  much operands are there*/
int set_operands(char *pos1, char *pos2, int *op1, int *op2, command cmd)
{
    int num;
	int q = cmd.operands;
	if(q==2) /*If there is two operands */
	{
		if(get_if_register(pos1, 1) > 0 && get_if_register(pos2, 2) > 0) /*Checks if both registers */
		{
			*op1 = get_if_register(pos1, 1); /*Sets the address of the registers */
			*op2 = get_if_register(pos2, 2); /*Sets the address of the second register */
			return 1;
		}
		if(get_if_register(pos1, 1) > 0 && get_if_symbol(pos2, 2) != NULL )
		{
			*op1 = get_if_register(pos1, 1);
			*op2 = get_if_symbol(pos2, 2)->item.symbolItem.address;
			return 2;
		}
		if(get_if_symbol(pos1, 1)!= NULL && get_if_symbol(pos2, 2) != NULL)
		{
			*op1 = get_if_symbol(pos1, 1)->item.symbolItem.address;
			*op2 = get_if_symbol(pos2, 2)->item.symbolItem.address;
			return 3;
		}
		if(get_if_symbol(pos1, 1) != NULL && get_if_register(pos2, 2) > 0)
		{
			*op1 = get_if_symbol(pos1, 1)->item.symbolItem.address;
			*op2 = get_if_register(pos2, 2);
			return 4;
		}
		if(get_if_number(pos1, 1) > MAX_MIN_VAL && get_if_register(pos2, 2) > 0)
		{
			*op1 = get_if_number(pos1, 1);
			*op2 = get_if_register(pos2, 2);
			return 5;
		}
		if(get_if_number(pos1, 1) > MAX_MIN_VAL && get_if_symbol(pos2, 2) != NULL)
		{
			*op1 = get_if_number(pos1, 1);
			*op2 = get_if_symbol(pos2, 2)->item.symbolItem.address;
			return 6;
		}
		if(get_if_stars(pos1, 1) > 0 && get_if_register(pos2, 2) > 0)
		{

			*op2 = get_if_register(pos2, 2);

			switch(get_if_stars(pos1, 1))
			{
			case 1: *op1 = rand() %6 ;
					return 7;
			case 2: *op1 = rand()%32767;
					return 8;
			case 3: *op1 = getRandomNode(internalLabelList)->item.dataItem.address;
					return 9;
			}

		}

		if(get_if_stars(pos1, 1) > 0 && get_if_symbol(pos2, 2) != NULL)
		{

			*op2 = get_if_symbol(pos2, 2)->item.symbolItem.address;

			switch(get_if_stars(pos1, 1))
			{
			case 1: *op1 = rand() %6 ;
					return 10;
			case 2: *op1 = rand()%32767;
					return 11;
			case 3: *op1 = getRandomNode(internalLabelList)->item.symbolItem.address;
					return 12;
			}
		}

		if(strcmp(cmd.name, "cmp") == 0)
					{
						if(get_if_number(pos1, 1) > MAX_MIN_VAL && get_if_number(pos2, 2) > MAX_MIN_VAL)
						{
							*op1 = get_if_number(pos1, 1);
							*op2 = get_if_number(pos2, 2);
							return 13;
						}
					}

	}
	if(q==1)
	{
		if(get_if_symbol(pos1, 2))
		{
			*op2 = get_if_symbol(pos1, 2)->item.symbolItem.address;

			return 15;
		}
		if(get_if_register(pos1, 2) > 0)
		{
			*op2 = get_if_register(pos1, 2);
			return 14;
		}

		if(strcmp(cmd.name, "prn") == 0)
		{
            num = get_if_number(pos1, 2);
			if( num > MAX_MIN_VAL)
			{
				*op2 = num;
				return 16;
			}
		}
	}
	if(q==0) return 0; /* means no operands are used to that operation */

	return -1;
}

/*This function checks if the operand is registry.
 * if it is, it returns the address of the register,
 * else returns -1. */
int get_if_register(char *pos, int opnum)
{
	int i;
	int return_value=-1;
	for(i=0; i<NUM_OF_REGS; i++)
	{
        if(opnum ==1)
        {
            if(strncmp(registers[i].regName,pos,2)==0  && (*(pos+2) == ' '  || *(pos+2)== ',') && check_space_ops(pos,1) == 1)
                return_value = registers[i].regAddress;
        }
        if(opnum == 2)
        {
        	if(check_space_ops(pos,2) == 1)
        	{
        		if(strncmp(registers[i].regName,pos,2)==0  &&( (*(pos+2) == ' '  || *(pos+2)== ',' || *(pos+2)== '\t' || *(pos+2)== '\n')  ))
        			return_value = registers[i].regAddress;
        	}
        }
	}
	return return_value;
}

/*This function checks if the operand is a direct number.
 * if so, than it return the value of the number.
 * else it returns NULL */
int get_if_number(char *pos, int opnum)
{
    int rValue;
	int i=0;
	if(*pos == IMM_CHAR && check_space_ops(pos, opnum) == 1)
	{
		pos ++ ;
		if(opnum == 1) while(pos[i] != ' ' && pos[i] != ',') i++; /*Counts to the last digit */
		else while(pos[i] != ' ' && pos[i] != ',' && pos[i] != '\n' && pos[i] != '\t') i++;
		rValue = str_to_int(pos, i);
	}
	else rValue = MAX_MIN_VAL;
	return rValue;

}
/*This function checks if the operand is a symbol (from the symbol table).
 * if so, the function returns a pointer to the right node in the node table
 * else, the function will return NULL */
nodeptr get_if_symbol(char *pos, int opnum)
{
	int state =1;
	int i=0;
	nodeptr temp = symbolHead;
	while(pos[i] != ' ' && pos[i] != ',' && pos[i] != '\n' && pos[i]!= 0) i++;
	while(state && temp!=NULL)
	{
		if(strncmp(pos, temp->item.symbolItem.label, i) == 0) state = 0;
		else temp = temp->next;

	}
	return temp;
}

/*This function checks if the operand is * type,
 * if so, it returns the number of *'s
 * else it returns -1 */
int get_if_stars(char *pos, int opnum)
{
	int nofstars = 3;
	char* one = "*";
	char* two = "**";
	char* three = "***";
	if( *pos == '*')
	{
		if(strncmp(pos , three , nofstars)==0)
		{
			pos+= nofstars;/*advance pos to point after the stars*/
			pos = skipSpaces(pos); /*skip the spaces*/
			if(opnum == 1)
			{
				if(*pos == PARAM_SEPARATOR)
				{
					return 3; /*3 stars at operand*/
				}
				else /*operand incorrectly defined*/
				{
					return -1;
				}
			}
			else /*opnum == 2*/
			{
				return -1; /*stars cant be on second operand*/
			}
		}
		/* didnt find 3 stars - lets look for 2 stars */
		nofstars--; /*decrease number of stars*/
		if(strncmp(pos , two , nofstars)== 0)
		{
			pos+= nofstars;/*advance pos to point after the stars*/
			pos = skipSpaces(pos); /*skip the spaces*/
			if(opnum == 1)
			{
				if(*pos == PARAM_SEPARATOR)
				{
					return 2; /*2 stars at operand*/
				}
				else /*operand incorrectly defined*/
				{
					return -1;
				}
			}
			else /*opnum == 2*/
			{
				return -1; /*stars cant be on second operand*/
			}
		}
		/*didnt find 2 stars - lets look for 1 star */
		nofstars--; /*decrease number of stars*/
		if(strncmp(pos , one, nofstars)== 0)
		{
			pos+= nofstars;/*advance pos to point after the stars*/
			pos = skipSpaces(pos); /*skip the spaces*/
			if(opnum == 1)
			{
				if(*pos == PARAM_SEPARATOR)
				{
					return 1; /*1 star at operand*/
				}
				else/*operand incrrectly defined*/
				{
					return -1;
				}
			}
			else /*opnum == 2*/
			{
				return -1; /*stars cant be on second operand*/
			}
		}
	}
    return -1; /*didnt find stars*/
}

/*This function converts an integer represented by a string to int type */
int str_to_int(char *strng, int length)
{
	char *number;
	number = (char*)malloc(sizeof(char)*length);
	CHECK_MEM_ALLOC(number);
	strncpy(number, strng, length);
	return atoi(number);
}


/*This is Two's complement function */
int two_complete(int num)
{
	int i=0;
	unsigned int mask = 1;
	num= (int) fabs(num); /*The absolute of the num*/
	while((num&mask) == 0)
	{
		mask = mask<<1;
		i++;
	}

	for(; i<14; i++)
	{

		mask =mask<<1;
		num ^= mask; /*We use XOR operation to "flip" the digits */
	}
	return num;
}

/*This function is used to print an external symbol and it's address in the program to the prog.ext file.
 * the function checks if the symbol is external type.
 * if so, print it. */
int print_to_extern(int address, char*  sym, int opnum , char* fname)
{
	nodeptr node = get_if_symbol(sym, opnum);
	FILE *extF=fopen(set_file_name(fname ,".ext"), "a");
	CHECK_MEM_ALLOC (extF);
	if(node->item.symbolItem.type.external == 1)
	{
        fprintf(extF, "%s\t%s \n", node->item.symbolItem.label, import_32(address));
        return 1;
	}
    fclose(extF);
	return 0;
}

/*-------------------------------------------------------------------------------------------------------*/
/* These 3 functions are used to convert bitfield struct of various types to their
 * integer representation */

int reg_to_int(regWord reg)
{
    return (reg.ARE)  | (reg.targetRegAddress<<2) | (reg.sourceRegAddress<<8)|(reg.notUsed<<14);
}

int data_to_int(dataWord data)
{
    return (data.ARE) | (data.value<<2);
}

int command_to_int(commandWord cmd)
{

    return(cmd.ARE) | (cmd.targetAdressing<<2) | (cmd.sourceAdressing<<4) | (cmd.opcode<<6) |
        (cmd.group<<10) | (cmd.rnd<<12) | (cmd.notUsed<<14);
}
/*-------------------------------------------------------------------------------------------------------*/


/*This function prints the Code table to the file */
void print_to_file(FILE *progF)
{
	nodeptr temp = codeHead;

	fprintf(progF, "Base 32 addres\tBase 32 machine code\n\t\t %s %s\n", import_32(IC-100), import_32(DC));

	while(temp != NULL)
	{
		fprintf(progF, "\t%s\t\t%s \n", import_32(temp->item.codeItem.address) , import_32(temp->item.codeItem.word.bin));
		temp = temp->next;
	}

	temp = dataHead;
	while(temp != NULL)
	{

        fprintf(progF, "\t%s\t\t%s \n", import_32(temp->item.dataItem.address), import_32(temp->item.dataItem.word));
        temp = temp->next;
	}


}

/*This function receives two char pointers to string and concatenate them into one string,
 *and than returns the reult. */
char* set_file_name(char * fname, char* type)
{
	char* runner;
	char *result;
	result = (char*) malloc(sizeof(char)*(strlen(fname) + strlen(type)));
	strcpy(result, fname);
	runner = result;
	while(*runner != '.')
	{
		runner++;
	}
	/*now result will point to dot*/
	runner[0] = 0;
	return strcat(result, type);
}

/*This function will check if a symbol is external by it's address */
char* extern_by_address(int addr)
{
    nodeptr temp = symbolHead;
        while(temp!= NULL)
    {
        if(temp->item.symbolItem.address == addr) return temp->item.symbolItem.label;
       else temp = temp->next;
    }
    return NULL;
}

/*This function checks if there are spaces in the argument.
 *If no, return 1, else return 0 */
int check_space_ops(char* op, int opnum)
{
int i=0;
while(op[i] != ' ' && op[i] != NEW_LINE && op[i] != PARAM_SEPARATOR) i++;
while(op[i] == ' ' || op[i] == '\t') i++;
if(opnum ==1)
{
	if(op[i] == ',') return 1;
	else return 0;
}
else if(opnum == 2)
{
	if(op[i] == '\n') return 1;
	else return 0;
}

return 0;
}

/*
 * function that will create a linked list with only internal labels
 * from the assembly code given by the input
 * return the number of nodes that are copied to the new lists.
 * that's used to find a random node to ** operand.
 */
int makeInternalLabelList()
{
	nodeptr temp;
	nodeptr internalItem;
	int counter = 0;
	temp = symbolHead;
	if(temp == NULL)/*empty list*/
	{
		return 0; /*no nodes to copy*/
	}
	do /*do - while loop is for the case there is only 1 node and we must check it*/
	{
		if(temp->item.symbolItem.type.external == 0)
		{
			internalItem = makeNewNode();
			internalItem->item.symbolItem.address = temp->item.symbolItem.address;
			strcpy(internalItem->item.symbolItem.label , temp->item.symbolItem.label);
			internalItem->item.symbolItem.type.external = 0;
			internalItem->item.symbolItem.type.functionLine = temp->item.symbolItem.type.functionLine;
			addToList(&internalLabelList , internalItem);
			counter ++; /*advance node counter*/
		}
		temp = temp->next; /*Advance to the second node */
	} while(temp!= NULL);
	return counter;
}















