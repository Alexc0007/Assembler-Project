/*
 * Created on: Mar 2, 2016
 *      Author: Alex Cherniak & Genady Mager
 */

#include<stdio.h>


#ifndef SECOND_H_
#define SECOND_H_
#define MAX_MIN_VAL -99999

/*=========================== PROTOTYPES =============================*/

void second_run(FILE *, char*);
void symbol_address_update();
int skip_spaces(char *);
int get_line(char*, FILE *);
int label_offset(char *);
void symbol_address_update();
char* import_32(int);
int set_operands(char *, char *, int *, int *, command);
int get_if_register(char *, int);
int get_if_number(char *, int);
nodeptr get_if_symbol(char *, int);
int get_if_stars(char *, int);
int str_to_int(char *, int);
int two_complete(int);
int print_to_extern(int, char*, int, char*);
int reg_to_int(regWord);
int data_to_int(dataWord);
int command_to_int(commandWord);
void print_to_file(FILE*);
char* set_file_name(char *, char*);
char* extern_by_address(int);
int check_space_ops(char*, int);
void print_table();
int makeInternalLabelList();



#endif /* SECOND_H_ */

