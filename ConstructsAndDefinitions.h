
/* This file created for the making needed struct mainly for label tabel and importans definitions for macro*/

#ifndef CONSTRUCTSANDDEFINITIONS_H
#define CONSTRUCTSANDDEFINITIONS_H


/* mainly for initialize for finishing ot next file, mainly for file main.c */
#define INITIALIZE_ALL  /*Deleeting label table, all pointers to table, closing file poimtres and initial address*/ \
deleteList(totalList); /*Deleeting and initializing total list for next file name.*/\
totalList = NULL;\
labelCode = NULL;\
labelData = NULL;\
fclose(fp);\
countAdress = INITIAL_ADDRESS;\
puts("----------------------------------------------------------------------------------------\n");\
continue



#define INITIAL_ADDRESS 100 /*Choisen the addres from the beginning.*/
#define MAX_LINE_SIZE 180

#define REAL_LABLE(label) label[strlen(label) - 1] = '\0'  /*For transering label definition to label like: "MAIN:"==>"MAIN"*/

/*Definition of for opcodes.*/
enum opcodes { movS, cmpS, addS, subS, notS, clrS, leaS, incS, decS, jmpS, bneS, redS, prnS, jsrS, rtsS, stopS };

/*Linked list for storing information for labels */
typedef struct labelInfo {
	char *name; /*Label name*/
	int address; /*Address of this label*/
	char type;   /*The type: 'C' Coding label, 'D' data label, 'U' Unknown label for external label.*/
	char coddingType; /*The codding type: 'A' Another label like "MER: mov r1,r2" that is not entry and notextern, 'E' External label, 'R' entRy label .*/
	struct labelInfo *next;
}labelInfo;


/* Mainly for checking the correctness. */
/*This is for possible opcode, possible operands and possible addressing for choisen opcode.*/
/*   CAT(category)_****(possible addressing for source operand)_****(possibleaddressing for second operand)      .*/
/* CAT_0123_123 1: mov, add, sub*/
/* CAT_0123_0123: cmp*/
/* CAT_123: not,clr,inc,dec,jmp,bne,red,jsr*/
/* CAT_12_123: lea*/
/* CAT_0123: prn*/
/* CAT_: stop, rts*/
enum addressingCategories { CAT_0123_123, CAT_0123_0123, CAT_123, CAT_12_123, CAT_0123, CAT_ };


#define NOT_CORRECT -1 /* In case that correctness failed.*/


/* Mainly for makingOutput.c */
#define MASKING_BITS 0x7FFF /* For making possible only for 15 bits beeing set. */
#define INITIAL_INITIALIZER 0x7000 /*For making "111" at the beginning from left of 15 bits. for command.*/



#define GROUP_NO_OPERANDS 0 /*For masking needed bits from left of 15 bits. for command.*/
#define GROUP_ONE_OPERANDS 1
#define GROUP_TWO_OPERANDS 2

/*For masking needed bits from left of 15 bits. for command.*/
#define SET_INITIAL_INITIALIZER(theOutput) theOutput|=INITIAL_INITIALIZER
#define SET_OPCODE(opcode,theOutput) theOutput|=((opcode)<<6)
#define SET_NUM_OPERANDS(numOperands,theOutput) theOutput|=((numOperands)<<10)

/* Structures for pointers for output files: object, extern, entry files */
typedef struct outputFilePointers {
	FILE *externFp;
	FILE *entryFp;
	FILE *objectFp;
}outputFilePointers;


#endif /* CONSTRUCTSANDDEFINITIONS_H  */