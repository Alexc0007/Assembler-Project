/*
 * models.h
 *
 *  Created on: Feb 25, 2016
 *      Author: Alex Cherniak & Genady Mager
 */

#ifndef MODELS_H_
#define MODELS_H_


#define MAX_LAB_LEN 30 /*the max label length*/
#define MIN_LAB_LEN 1 /*the minimum label length*/
#define MAX_CMD_NAME 5 /*maximum command name length*/
#define MAX_DIR_LENGTH 7 /*maximum directive name length*/

#define MAX_LINE_LEN 80 /*the maximum line length*/
#define MEM_SIZE 1000 /*memory size*/
#define WORD_LEN /* 1 memory word length*/
#define NUM_OF_CMD 16 /*the number of commands*/

#define REG_NAME_SIZE 3 /*register name size*/
#define NUM_OF_REGS 8 /*number of registers 0-7*/

#define DATA_INST ".data" /*data instruction*/
#define STRING_INST ".string" /*string instruction*/
#define ENTRY_INST ".entry" /*entry instruction*/
#define EXTERN_INST ".extern" /*extern instruction*/

#define IMM_CHAR '#' /*immediate character*/
#define PARAM_SEPARATOR ',' /*parameters separator character*/
#define COMMENT_CHAR ';' /*comment character*/
#define LABEL_END_CHAR ':' /*label end character*/
#define QUOTE_MARK '\"'/*quote mark char*/
#define NEW_LINE '\n' /*new line char*/
#define TAB '\t' /*tab char*/
#define WORD_MASK 0x7fff /* used to cut 15 bits in a data word thats added to memory*/
#define EXTENAL_LABEL 1
#define NOT_EXTERNAL 0
#define HAS_FUNC 1
#define HAS_NO_FUNC 0




/**
 * build a struct that will represent a command word - using bitfields
 */
typedef struct
{
	unsigned int ARE	:2; /* bits 0-1*/
	unsigned int targetAdressing	:2; /*bits 2-3*/
	unsigned int sourceAdressing	:2; /*bits 4-5*/
	unsigned int opcode	:4; /*bits 6-9*/
	unsigned int group	:2; /*bits 10-11*/
	unsigned int rnd	:2; /* bits 12-13*/
	unsigned int notUsed:1; /* bit 14*/

}commandWord;

/**
 * build a struct that will represent a data word - immediate
 */
typedef struct
{
	unsigned int ARE :2; /*bits 0-1*/
	unsigned int value :13; /*bits 2-14*/
}dataWord;


/**
 * build a struct that will represent a register word in the code table
 */
typedef struct
{
	unsigned int ARE :2; /*bits 0-1*/
	unsigned int targetRegAddress :6; /*bits 2-7*/
	unsigned int sourceRegAddress :6; /*bits 8-13*/
	unsigned int notUsed :1; /*bit 14*/
}regWord;

/**
 * a struct that will represent a register name and its address
 */
typedef struct
{
	char regName[REG_NAME_SIZE];
	unsigned int regAddress;
}reg;

extern reg registers[];

/**
 * a union that describes a memory word that could fit into the code table
 */
typedef union
{
	unsigned int bin; /*the binary form of the word*/
	regWord reg; /* a word that will contain info about 1 or 2 registers*/
	dataWord data; /*a word that will contain data + ARE*/
	commandWord command; /*a word that will contain info about a command*/
}memWord;

/**
 * a struct that will hold info about the label type, if its external or has function line attached to it
 */
typedef struct
{
	int external; /* if 0 - not external , if 1 - external*/
	int functionLine; /* if 0 - has no function line , if 1 - has function line*/
}labelType;


/**
 * a struct that holds info about a line in the code table
 */
typedef struct
{
	int address;
	memWord word;
}codeTableItem;

/**
 * a struct that will hold info about a line in the symbol table
 */
typedef struct
{
	char label[MAX_LAB_LEN];
	int address;
	labelType type; /*will hold info about the the label type - externel? or has function line?*/
}symbolTableItem;

/*
 * a struct that will hold info about a line in the data table
 */
typedef struct
{
	int address; /*data address*/
	int word;/*data word - should be added & WORD_MASK*/
}dataTableItem;


/**
 * a union that will hold either a code table item , symbol table item or
 * data table item
 * the 3 of them can be placed in a linked list that will represent
 * a table for each item
 */
typedef union
{
	codeTableItem codeItem;
	symbolTableItem symbolItem;
	dataTableItem dataItem;
}item;

/**
 * a linked list that will represent the code table / symbol table / data table
 * it will work for the three of them
 */
typedef struct Node* nodeptr; /*define a pointer to a node in a list*/

typedef struct Node
{
	item item;
	nodeptr next;
}Node;







/**
 * defining the addressing methods - 0,1,2,3 as defined in the booklet
 */
typedef enum
{
	NONE = -1,
	IMMEDIATE = 0,
	DIRECT,   /* 1*/
	RANDOM,	  /* 2 */
	REGISTER  /* 3 */
}addressingMethods;

/**
 * a struct that hold information about each command , its opcode , number of operands
 * and the supported addressing methods for source and target operands
 */
typedef struct
{
	char name[MAX_CMD_NAME]; /*command name*/
	unsigned short opcode;   /*command operation code*/
	unsigned short operands; /*number of operands in command*/
	unsigned int supportedSrcAddressing; /*bit mask that shows source addressing method*/
	unsigned int supportedTarAddressing; /*bit mast that shows target addressing method*/
}command;


extern command commands[];

/*
 * set numbers to define the instruction type in order to determine
 * the algorithm to proceed with the actions
 */
typedef enum
{
	INVALID_INSTRUCTION = -1,
	NOT_DIRECTIVE, /* 0 */
	DATA,   /* 1 */
	STRING, /* 2 */
	ENTRY,  /* 3 */
	EXTERN  /* 4 */
}instructionType;

/*============= EXTERN VARIABLES ===========================*/
extern nodeptr symbolHead;
extern nodeptr codeHead;
extern nodeptr dataHead;
extern nodeptr internalLabelList;
extern char* newPos;


/*===========PROTOTYPES=====================================*/
nodeptr makeNewNode();
nodeptr addToList(nodeptr* , nodeptr);
int checkLabel(char*);
void getLabel(char* , char**);
void setSymbolLastItemAddress(nodeptr* , int);
void setSymbolLastItemLabelType(nodeptr* , int , int);
instructionType checkInstruction(char* , char**);
command getCommand(char* , char**);
char* storeStringToData(char* , char*);
void storeNumsToData(int* , int);
char* getExternLabel(char*);
void storeExternLabel(char*);
void clrStringArray();
int operandValidation(char* , char* , command);
int checkNum(char* pos);
int checkReg(char *pos);
int checkLab(char* pos);
int checkStars(char* pos);



#endif




