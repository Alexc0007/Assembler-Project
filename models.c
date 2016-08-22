/*
 * models.c
 *
 *  Created on: Feb 25, 2016
 *      Author: Alex Cherniak & Genady Mager
 */

/*=======================Included Files =========================*/
#include "models.h"
#include "Utilities.h"
#include "firstPass.h"
#include "second.h"

/*========================Included Libraries ======================*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

nodeptr symbolHead = NULL;
nodeptr codeHead = NULL;
nodeptr dataHead = NULL;
nodeptr internalLabelList = NULL;


/*
 * array that contain information about commands that can be used in the
 * assembly language
 */
command commands[] =
{/*name, opcode, operands , supported src operands , supported target operands*/
	{"mov" , 0 , 2 , (IMMEDIATE | DIRECT | RANDOM | REGISTER) , (DIRECT | REGISTER)},
	{"cmp" , 1 , 2 , (IMMEDIATE | DIRECT | RANDOM | REGISTER) , (IMMEDIATE | DIRECT | REGISTER)},
	{"add" , 2 , 2 , (IMMEDIATE | DIRECT | RANDOM | REGISTER) , (DIRECT | REGISTER)},
	{"sub" , 3 , 2 , (IMMEDIATE | DIRECT | RANDOM | REGISTER) , (DIRECT | REGISTER)},
	{"not" , 4 , 1 , NONE , (DIRECT | REGISTER)},
	{"clr" , 5 , 1 , NONE , (DIRECT | REGISTER)},
	{"lea" , 6 , 2 , (DIRECT | RANDOM) , (DIRECT | REGISTER)},
	{"inc" , 7 , 1 , NONE , (DIRECT | REGISTER)},
	{"dec" , 8 , 1 , NONE , (DIRECT | REGISTER)},
	{"jmp" , 9 , 1 , NONE , (DIRECT | REGISTER)},
	{"bne" , 10 , 1 , NONE , (DIRECT | REGISTER)},
	{"red" , 11 , 1 , NONE , (DIRECT | REGISTER)},
	{"prn" , 12 , 1 , NONE , (IMMEDIATE | DIRECT | REGISTER)},
	{"jsr" , 13 , 1 , NONE , (DIRECT | REGISTER)},
	{"rts" , 14 , 0 , NONE , NONE},
	{"stop" , 15 , 0 , NONE , NONE},
};

/*
 * array that will countain information about registers and their
 * addresses
 */
reg registers[] = /*registers table with register names and addresses*/
{
		{"r0" , 0},
		{"r1" , 1},
		{"r2" , 2},
		{"r3" , 3},
		{"r4" , 4},
		{"r5" , 5},
		{"r6" , 6},
		{"r7" , 7}
};


/**
 * function that create a new node
 */
nodeptr makeNewNode()
{
	nodeptr newNode;
	newNode = (nodeptr)malloc(sizeof(Node));
	CHECK_MEM_ALLOC(newNode);
	newNode->next = NULL;
	return newNode;
}

/**
 * function that adds 1 node to the linked list
 */
nodeptr addToList(nodeptr* head , nodeptr node)
{
	nodeptr temp;
	if(*head == NULL) /* if head is an empty list - return the new node as head*/
	{
		*head = node;
		return *head;
	}
	temp = *head;
	while(temp->next != NULL) /*loop to the end of the loop*/
		temp = temp->next;

	temp->next = node;
	return *head;
}



/**
 * a function that gets a string pointer and looks for a label
 * if label is ok - return 1
 * if no label - return 0
 * if label exist but is not valid(too long , or has spaces before the sign ':'
 * - return -1/-2/-3/-4/-5 depending on the type of error
 * function's param: char* pointer to command line
 */
int checkLabel(char* line)
{
	int flag = 0; /*a flag to check if ':' found in the command*/
	nodeptr temp;
	char label[MAX_LAB_LEN]; /*temp array to hold label if there is one*/
	int i,j;
	char* runner; /*runner will through the command line*/
	runner = line;
	i=0;
	while(runner[i]!='\0')
	{
		if(runner[i] == LABEL_END_CHAR)
		{
			flag = 1; /*found char ':'*/
			break;
		}
		i++; /*advance i to point to the next char*/
	}
	if(flag == 1)/*we must check if Label is viable - has no spaces ,
	length is less than 30 chars , has no register or command name*/
	{
		/*=================== check label length =======================*/

		if(i > 30) /*label is too long*/
		{
			return -1;
		}
		/*================= check spaces in label ======================*/

		for(j=0 ; j<i ; j++) /*check for spaces inside label*/
		{
			if(runner[j] == ' ')
			{
				return -2; /*found a space in the label text*/
			}
		}
		memcpy(label , line , i); /* copy i bytes into label*/
		/* now label will contain Label thats alredy been checked that has
		 * no space and is shorter than 30 bytes
		 */
		label[i] = 0; /*null terminator to label string*/

		/*========== check label name is not command's name ===========*/

		for(j=0 ; j<NUM_OF_CMD ; j++)
		{
			if(strcmp(label , commands[j].name) == 0)
				/* check that label dont have the same name as any command*/
			{
				return -3; /*means label has the same name as a command*/
			}
		}

		/*========== check label name is not register's name ===========*/

		for(j=0 ; j<NUM_OF_REGS ; j++)
		{
			if(strcmp(label , registers[j].regName) == 0)
				/*check that label dont have same name as register name*/
			{
				return -4; /*means label has the same name as a register*/
			}
		}
		/*now only need to check the symbol table and make sure that label
		 * isn't already in the symbol table
		 */

		/*========== check that label hasnt been used before ===========*/

		temp = symbolHead;
		while(temp != NULL)
		{
			if(strcmp(label , temp->item.symbolItem.label) == 0)
				/*check if label is already in symbol table*/
			{
				return -5; /*label already been used in this code*/
			}
			temp = temp->next; /*advance down the table*/
		}
		/*if all test are pass and got this far - label is viable*/
		return 1;
	}
	else
	{
		return 0; /*no label on this command line*/
	}
}

/*
 * a function that gets label from a command line
 * and store it in the symbol table.
 * function's param: char* pointer to command line.
 * function will be used after we know there is a viable label
 * and ensured it with checkLabel()
 */
void getLabel(char* p , char** newPos)
{
	char label[MAX_LAB_LEN] = {0};
	nodeptr temp;
	int i;
	char* runner;

	runner = p;
	i=0;
	while(*runner != LABEL_END_CHAR)/*a loop to check how many bytes need to be copied*/
	{
		runner++;
		i++;
	}
	strncpy(label , p , i); /*put the Label inside label[]*/
	temp = makeNewNode();/*create new node*/
	strcpy(temp->item.symbolItem.label , label); /*copy label name into the node*/
	symbolHead = addToList(&symbolHead , temp); /*add node to symbol table*/
	*newPos = ++runner; /*update newPos to point to the new position in line
	after reading the label*/
}


/*
 * a function that set the address for the last symbol item in the list
 * address is either DC or IC
 */
void setSymbolLastItemAddress(nodeptr* head , int address)
{
	nodeptr temp;
	temp = *head;
	while(temp->next != NULL)/*loop to last item in list*/
	{
		temp = temp->next;
	}
	temp->item.symbolItem.address = address;
}

/*
 * function that sets the type for the last symbolTable item in the list
 * type has 2 fields:
 * 1) external => 1 = external , 0 = not external
 * 2) function line => 1 if has function line , 0 if no function line
 */
void setSymbolLastItemLabelType(nodeptr* head , int external , int functionLine)
{
	nodeptr temp;
	temp = *head;
	while(temp->next != NULL)/*loop to the last item*/
	{
		temp = temp->next;
	}
	temp->item.symbolItem.type.external = external;
	temp->item.symbolItem.type.functionLine = functionLine;
}

/*
 * the function gets a command line and check the instruction type
 * of the command line
 *
 * received line is already without spaces so if there is a directive
 * (.data , .string , .entry or .extern) , the '.' will be the first character
 */
instructionType checkInstruction(char* line , char** newPos/*address*/)
{
	FILE* fp;
	int ins_len;
	char* runner;
	char* temp;
	char directive[MAX_DIR_LENGTH];
	runner = line;
	runner = skipSpaces(runner); /*skip the spaces*/
	temp = strchr(runner , ' '); /*temp will point to the next space*/
	ins_len = (int)(temp - runner); /*find the command length*/
	if(runner[0] == '.')/*means there is a directive*/
	{
		if(ins_len < 5)/*invalid directive*/
		{
			fp = fopen(errName , "a");
			if(fp != NULL)
			{
				fprintf(fp , "%s %d %s" , "line: ", lineNumber , "invalid directive \n");
				/*generate error message to err_log*/
				fclose(fp);
			}
			return -1;
		}
		strncpy(directive ,runner , ins_len); /*copy the directive into directive[]*/
		if(strcmp(directive , ".data") == 0)
		{
			*newPos = (line + ins_len); /*return the position in line after reading directive*/
			return 1; /* .data directive found*/
		}
		if(strcmp(directive , ".string") == 0)
		{
			*newPos =(line + ins_len);
			return 2; /* .string directive found*/
		}
		if(strcmp(directive , ".entry") == 0)
		{
			*newPos = (line +ins_len);
			return 3; /* .entry directive found*/
		}
		if(strcmp(directive , ".extern") == 0)
		{
			*newPos = (line +ins_len);
			return 4; /* .extern directive found*/
		}
		fp = fopen(errName, "a");
		if(fp != NULL)
		{
			fprintf(fp , "%s %d %s" , "line: ", IC , "invalid directive\n");
			/*generate error message to err_log*/
			fclose(fp);
		}
		return -1; /*invalid directive*/

	}
	else
	{
		*newPos = (line +ins_len); /*in this case
		return the position in line only after clearing the spaces
		on the start of the line if there are some*/
		return 0; /*no Directive found*/
	}
}

/*
 * a function that will get command from line
 * assumption is pointer line is pointing to start of command
 */
command getCommand(char* line , char** newPos)
{
	int i=0;
	FILE* fp;
	command invalid; /*definition of an invalid command that will be returned
	if command is not found in the commands array*/
	strcpy(invalid.name , "inv");
	invalid.opcode = 0;
	invalid.operands = 0;
	invalid.supportedSrcAddressing = NONE;
	invalid.supportedTarAddressing = NONE;
	while(i<NUM_OF_CMD)
	{
		if(strncmp(line , commands[i].name , 3) == 0) /*command found*/
		{
			if(line[3] != ' ')/*means there is another char after the command*/
			{
				if(i == 15)/*check stop command - only one with 4 letters*/
				{
					if(strncmp(line , commands[i].name , 4) == 0) /*check that its really stop command*/
					{
						*newPos = line + strlen(commands[i].name);
						return commands[i]; /*return the stop command*/
					}
					else
					{
						fp = fopen(errName , "a");
						if(fp != NULL)
						{
							fprintf(fp , "%s %d %s" , "line: ", lineNumber , "invalid command\n");
							/*generate error message to err_log*/
							fclose(fp);
							return invalid; /*return an invalid command*/
						}

					}
				}
				fp = fopen(errName , "a");
				if(fp != NULL)
				{
					fprintf(fp , "%s %d %s" , "line: ", lineNumber , "invalid command\n");
					/*generate error message to err_log*/
					fclose(fp);
					return invalid; /*return an invalid command*/
				}
			}
			*newPos = line + strlen(commands[i].name);
			/*new pointer position after command*/
			return commands[i];
		}
		i++;
	}
	fp = fopen(errName , "a");
	if(fp != NULL)
	{
		fprintf(fp , "%s %d %s" , "line: ", lineNumber , "invalid command\n");
		/*generate error message to err_log*/
		fclose(fp);
		return invalid; /*return an invalid command*/
	}
	return invalid;
}

/*
 * a function that will store string into the data table and will
 * update the DC if a viable string is found
 */
char* storeStringToData(char* string , char* line)
{
	nodeptr temp;
	int i=DC; /*will insert data address - the one DC points to*/
	int count=0; /*will count how many data words were inserted and
	determine how much to add to DC after the addition*/

	while(string[count] != '\0')
	{
		temp = makeNewNode(); /* create new node*/
		temp->item.dataItem.address = i; /*insert data address as value of i*/
		temp->item.dataItem.word = (string[count] & WORD_MASK);
		addToList(&dataHead , temp);
		i++;
		count++;
	}/*now we need to insert null terminator to the string*/
	temp = makeNewNode();
	temp->item.dataItem.address = i;
	temp->item.dataItem.word = 0; /*the null terminator*/
	addToList(&dataHead , temp);
	count++;

	/*once the whole string is added to data - need to update DC*/
	DC = DC+count; /*update DC*/

	/*now advance line to the end of the string -
	line had to point to '"'  or to a space before the start of the function */
	line = skipSpaces(line);
	if(line[0] == QUOTE_MARK)
		line++;
	while(line[0] != QUOTE_MARK)/*when loop done , line points to '"'*/
		line++;
	line++; /*advance line one more time to point 1 char after '"'*/

	return line;
}
/*
 * function that will get an array of numbers and store them to data table
 */
void storeNumsToData(int* data , int dataAmount)
{
	nodeptr temp;
	int i =DC;
	int j = 0;
	while(j < dataAmount)
	{
		temp = makeNewNode(); /*create new node*/
		temp->item.dataItem.address = i; /*insert address*/
		temp->item.dataItem.word = (data[j] & WORD_MASK); /*insert data word*/
		addToList(&dataHead , temp); /*add to dataTable*/
		i++; /*advance address*/
		j++; /*advance data array*/
	}
	DC = DC+dataAmount; /*update DC*/
/*once this function is done - we can skip to end of line*/
}


/*
 * function that will be called only when found out that instruction is .extern
 * and will return pointer to extern label so we can store it to the
 * symbol table
 *
 * if NULL is returned , means that during the check , found out that there
 * is an error in the extern label and error will be generated to error log
 */
char* getExternLabel(char* line)
{
	nodeptr temp;
	FILE* fp;
	int i = 0 , j=0;
	char* output;
	clrStringArray(); /*clear the string array before use*/
	output = string; /*apply output to point to string array*/
	line = skipSpaces(line); /*skip spaces*/
	/*line = removeSpaces(line);*/
	while(*line != '\n')
	{
		if((isalpha(*line) != 0) ||(isdigit(*line) != 0))/*means char is ok
		, char is digit or alphabet*/
		{
			output[i] = *line; /*copy char to string array*/
			i++;/*advance counter of characters*/
		}
		else/*extern label name incorretly defined*/
		{
			fp = fopen(errName , "a");
			fprintf(fp , "%s %d %s" , "line: ", lineNumber , "extern label incorrectly defined\n");
			/*generate error message to err_log*/
			fclose(fp);
			return NULL;
		}
		line++;
	}
	output[i+1] = 0; /*null terminator to the label string*/

	/*============== check label length ===================================*/
	if(i > MAX_LAB_LEN) /*means label is too long*/
	{
		fp = fopen(errName , "a");
		fprintf(fp , "%s %d %s" , "line: ", lineNumber , "extern label is too long\n");
		/*generate error message to err_log*/
		fclose(fp);
		return NULL;
	}

	/*========= check label doesnt have name of a register ================*/
	for(j=0 ; j<NUM_OF_REGS ; j++)
	{
		if(strcmp(output , registers[j].regName) == 0)
		/*check that label dont have same name as register name*/
		{
			fp = fopen(errName , "a");
			fprintf(fp , "%s %d %s" , "line: ", lineNumber , "extern label has register name\n");
			/*generate error message to err_log*/
			fclose(fp);
			return NULL;
		}
	}

	/*========== check label name is not command's name ==================*/

	for(j=0 ; j<NUM_OF_CMD ; j++)
	{
		if(strcmp(output ,commands[j].name) == 0)
		/* check that label dont have the same name as any command*/
		{
			fp = fopen(errName , "a");
			fprintf(fp , "%s %d %s" , "line: ", lineNumber , "extern label has command name\n");
			/*generate error message to err_log*/
			fclose(fp);
			return NULL;
		}
	}

	/*============ Check that symbol hasnt been used before ==============*/
	temp = symbolHead;
	while(temp != NULL)
	{
		if(strcmp(output , temp->item.symbolItem.label) == 0)
			/*check if label is already in symbol table*/
		{
			fp = fopen(errName , "a");
			fprintf(fp , "%s %d %s" , "line: ", lineNumber , "extern label has register name\n");
			/*generate error message to err_log*/
			fclose(fp);
			return NULL;
		}
		temp = temp->next; /*advance down the table*/
	}
	/*if all tests are pass , can return string*/
	return output;
}

/*
 * a function that will receive  viable
 * label name and will store it as external label
 * in the symbol table
 */
void storeExternLabel(char* labelName)
{
	nodeptr temp;
	temp = makeNewNode();
	strcpy(temp->item.symbolItem.label , labelName); /*copy the label name to temp node*/
	temp->item.symbolItem.type.external = EXTENAL_LABEL;/*set this label to be external*/
	temp->item.symbolItem.type.functionLine = HAS_NO_FUNC;/*set has no function line*/
	temp->item.symbolItem.address = 0; /*temp has no address*/
	addToList(&symbolHead , temp);/*add the node temp to the list*/
}

/*
 * a function that will clear the string array before its used
 */
void clrStringArray()
{
	int i;
	for(i=0 ; i < MAX_LINE_LEN ; i++)
	{
		string[i] = 0;
	}
}

/*
 * a function that will check operands validation and will write to error log
 * if one of the incoming operands is faulty
 */
int operandValidation(char* pos1 , char* pos2 , command cmd)
{
	FILE* fp;
	int nofOperands = cmd.operands;
	if(nofOperands == 2)/*there are 2 operands*/
	{
		/*============ check lea command ==============================*/
		if(strcmp(cmd.name , "lea") == 0)/*lea command*/
		{
			if(checkStars(pos1) > 0 || checkLab(pos1) ==1)
			{
				/*now check second operand - label or register*/
				if(checkLab(pos2) ==1 || checkReg(pos2)>0)
				{
					return 1; /*operands are ok */
				}
				else /*determine problem in second operand*/
				{
						fp = fopen(errName , "a");
						fprintf(fp , "%s %d %s" , "line: ", lineNumber , "second operand incorrectly defined\n");
						fclose(fp);
						return -1;
				}
			}
			else/*problem in first operand*/
			{
				fp = fopen(errName , "a");
				fprintf(fp , "%s %d %s" , "line: ", lineNumber , "first operand incorrectly defined\n");
				fclose(fp);
				return -1;
			}
		}
		/*=========== tests for all other 2 operands commands =================*/
		if(checkNum(pos1) == 1 || checkStars(pos1)>0 || checkReg(pos1)>=0 || checkLab(pos1)==1)
		{
			/*now check second operand*/
			if(checkReg(pos2)>=0 || checkLab(pos2)==1)
			{
				if((checkReg(pos1) >= 0 && checkReg(pos2) >=0) || (checkStars(pos1) == 1 && checkReg(pos2)>= 0))/*both operands are registers*/
				{
					return 2; /*operands are ok and both registers*/
				}
				return 1; /*operands are ok*/
			}
			else
			{
				fp = fopen(errName , "a");
				fprintf(fp , "%s %d %s" , "line: ", lineNumber , "second operand incorrectly defined\n");
				fclose(fp);
				return -1;
			}
		}
		else/* problem is in first operand*/
		{
			fp = fopen(errName , "a");
			fprintf(fp , "%s %d %s" , "line: ", lineNumber , "first operand incorrectly defined\n");
			fclose(fp);
			return -1;
		}
	}
	if(nofOperands == 1)/*for commands with 1 operand*/
	{
		/*=============== check prn command ===============================*/
		if(strcmp(cmd.name , "prn") == 0)/*prn command*/
		{
			if(checkNum(pos1) == 1 || checkReg(pos1) >=0 || checkLab(pos1)==1)
			{
				return 1; /*operands are ok*/
			}
			else /*incorrectly defined operand*/
			{
				fp = fopen(errName , "a");
				fprintf(fp , "%s %d %s" , "line: ", lineNumber , "first operand incorrectly defined\n");
				fclose(fp);
				return -1;
			}
		}
		/*============ check all other 1 operand commands ===================*/
		if(checkReg(pos1) >= 0 || checkLab(pos1) == 1)
		{
			return 1; /*operand is ok*/
		}
		else/*incorrectly defined operand*/
		{
			fp = fopen(errName , "a");
			fprintf(fp , "%s %d %s" , "line: ", lineNumber , "first operand incorrectly defined\n");
			fclose(fp);
			return -1;
		}
	}
	if(nofOperands == 0) /*for command with no operands - just check that rest of line is clear*/
	{
		if(checkEndLine(pos1) == 1)
		{
			return 1; /*end line is ok*/
		}
		else
		{
			fp = fopen(errName , "a");
			fprintf(fp , "%s %d %s" , "line: ", lineNumber , "command expects no operands\n");
			fclose(fp);
			return -1;
		}
	}
	return -1; /*end line is not ok*/
}

/*
 * a function that will check if operand is an immidiate number
 * return 0 if not
 * return 1 if yes
 */
int checkNum(char* pos)
{
	char* endptr; /*end pointer for strtol*/
	int result=0;
	int num;
	if(*pos == IMM_CHAR)
	{
		pos++; /*advance pos to point to start of number*/
		if(*pos != '0')
		{
			num = strtol(pos , &endptr , DECIMAL);
			if(num != 0)
			{
				if(*endptr == NEW_LINE || *endptr == PARAM_SEPARATOR || *endptr == ' ')
				{
					result = 1;
				}
			}
			else
			{
				result = 0; /*means that pos doesnt point to a number*/
			}

		}
		else /*check that after 0 there are no more garbage chars*/
		{
			pos++; /*advance pos to point after 0*/
			pos = skipSpaces(pos);/*skip all spaces*/
			if(*pos == NEW_LINE || *pos == PARAM_SEPARATOR)
			{
				result =1;
			}
			else result =0;
		}
	}
	return result;
}
/*
 * function will check if register is found as operand
 * return register address if a register - a number from 0 to 7
 * return -1 if no register found
 * return -2 if param incorrectly defined
 */
int checkReg(char *pos)
{
	int result = -1;
	int i;
	for(i=0 ; i<NUM_OF_REGS ; i++)
	{
		if(strncmp(pos , registers[i].regName , 2) == 0 &&(pos[2] == PARAM_SEPARATOR || pos[2]== NEW_LINE || pos[2]== ' '))
		{
			result = registers[i].regAddress;
			if(pos[2] == ' ')/*if there is a space after register name*/
			{
				pos+=2;
				pos = skipSpaces(pos);
				if(*pos != NEW_LINE || *pos != PARAM_SEPARATOR)
				{
					return -2; /* param incorrectly defined*/
				}
			}
		}
	}
	return result;
}

/*
 * function that will check if pos points to a string that can be used as label
 * return 1 if label is ok
 * return -1 if label too long
 * return -2 if label incorrectly defined
 */
int checkLab(char* pos)
{
	char* runner;
	int length=0;
	runner = pos;
	while(*runner != NEW_LINE && *runner != PARAM_SEPARATOR)/*get label length*/
	{
		runner++;
		length++;
	}
	if(length > MAX_LAB_LEN)/*label is too long? */
	{
		return -1;
	}
	while(pos != runner) /*run on label , check its letters or digits and nothing else*/
	{
		if(isalpha(*pos) != 0 || isdigit(*pos) != 0)/* *pos is letter or digit*/
		{
			pos++;
		}
		else/*label incorrectly defined*/
		{
			return -2;
		}
	}
	if(*runner == PARAM_SEPARATOR)/*means label is the first operand*/
	{
		runner++; /*advance 1 char forward*/
		if(*runner == NEW_LINE)/*cant be because expecting second operand*/
		{
			return -2;
		}
		while(*runner != NEW_LINE)/*run till end of line*/
		{
			if(*runner == PARAM_SEPARATOR)/*found another ',' */
			{
				return -2; /*label incorrectly defined*/
			}
			runner++;
		}
	}
	return 1; /*label is ok*/
}

/*function that checks if operand are stars - 1 , 2 or 3 */
int checkStars(char* pos)
{
	FILE* fp;
	int nofstars = 3;
	char* one = "*";
	char* two = "**";
	char* three = "***";
	if(*pos == '*')
	{
		if(strncmp(pos , three , nofstars)==0)/*check if 3 stars*/
		{
			pos+= nofstars;/*advance pos to point after the stars*/
			pos = skipSpaces(pos); /*skip the spaces*/
			if(*pos == PARAM_SEPARATOR)
			{
				return 3; /*3 stars at operand*/
			}
			else /*operand incorrectly defined*/
			{
				fp = fopen(errName , "a");
				fprintf(fp , "%s %d %s" , "line: ", lineNumber , "first operand invalid\n");
				/*generate error message to err_log*/
				fclose(fp);
				return -1;
			}
		}
		/* didnt find 3 stars - lets look for 2 stars */
		nofstars--; /*decrease number of stars*/
		if(strncmp(pos , two , nofstars)== 0)
		{
			pos+= nofstars;/*advance pos to point after the stars*/
			pos = skipSpaces(pos); /*skip the spaces*/
			if(*pos == PARAM_SEPARATOR)
			{
				return 2; /*2 stars at operand*/
			}
			else /*operand incorrectly defined*/
			{
				fp = fopen(errName , "a");
				fprintf(fp , "%s %d %s" , "line: ", lineNumber , "first operand invalid\n");
				/*generate error message to err_log*/
				fclose(fp);
				return -1;
			}
		}
		/*didnt find 2 stars - lets look for 1 star */
		nofstars--;
		if(strncmp(pos , one , nofstars)== 0)
		{
			pos+= nofstars;/*advance pos to point after the star*/
			pos = skipSpaces(pos); /*skip the spaces*/
			if(*pos == PARAM_SEPARATOR)
			{
				return 1; /*1 star at operand*/
			}
			else /*operand incorrectly defined*/
			{
				fp = fopen(errName , "a");
				fprintf(fp , "%s %d %s" , "line: ", lineNumber , "first operand invalid\n");
				/*generate error message to err_log*/
				fclose(fp);
				return -1;
			}
		}
	}
	return -1; /*no stars*/
}


