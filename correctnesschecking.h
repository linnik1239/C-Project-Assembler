/* This header file for checking correctness, there 2 scanning correctness for file, first is
to check basis correcness per line,and the second especially for label if no exist more than one label
with the same name as can be correct for example label cannot be defined as absolute and externak*/





#ifndef CORRECTNESSCHECKING_H
#define CORRECTNESSCHECKING_H
#include "buildlabeltable.h"
#include "ConstructsAndDefinitions.h"

#define YES "YES"
#define NOT "NOT"








/* check correctness for File whom 'fp' is pointing, getting also pointer to the beginning of label table,
and file name for printing error comments from which file errors were found.*/
int correctnessChecking(FILE *fp, labelInfo *head, char *fileName);
/*Cheching the correctness pr line*/
int isLineCorrect(labelInfo *head, char *token, int lineNum, char *fileNAme);
/* return 1 if token is asm num like "#-7", else this function return 0.*/
int isAsmNum(char *token);
/*return 1 if token is second addressind mode with index operand like "r4[r8]" */
int isAdressWithOperandAtAll(char *token);
/*return 1 if token is correct second addressind mode with index operand like "r4[r5]" 
in case of errors, 0 is returned and errors are printed for error file. */
int isCorrectAdressWithOperand(char *token, int lineNum, char *fileNAme);
/*return 1 if token is in label table and is in code type else return 0*/
int isLabelData(labelInfo *head, char *token);
/*return 1 if token is in label table and is in data type else return 0*/
int isLabelCode(labelInfo *head, char *token);
/*return 1 if token is exist in label table else return 0*/
int isLabelExist(labelInfo *head, char *token);
/*return 1 if token is register else return 0*/
int isReg(char *token);
/*return 1 if token  register has any addressing option (0,1,2 or 3 addressing mode)for choisen opcode(like "lea" opcode for source operand)
in case of errors, 0 is returned and errors are printed for error file. 
optionNum is number fo options. for example '2' is for option 1 and 2 for 'lea' opcode. '3' is for option 1,2,3. '4' is for options 0,1,2,3*/
int isAdressOption(labelInfo *head, char *token, int lineNum, char *fileNAme, char *numOperand, int optionNum);
/*When categorry is choisen like CAT_****_**** this function check correctnrss for this categorry.*/
int checkForChoisenCategory(labelInfo *head,  char *currentToken, int category, int lineNum, char *fileNAme);
/*Check if token is opcode or not*/
int isOpcode(char *token);
/*Check if token is numeric string*/
int isStrNum(char *token);
/*Cheching correctness after  ".string" */
int isStringCorrect(char *token, int lineNum, char *fileNAme);
/*Cheching the correctness pr line*/
int isFullLineCorrect(labelInfo *head, char *token, int lineNum, char *fileNAme);
/* Cheching correctness after  ".data" */
int checkCommaCorrectness(char *token, int lineNum, char *fileNAme);
/*Check if name of label, tokem has Unknown coding type (not know if it for code or data).*/
int isLabelUnkown(labelInfo *head, char *token);
/*This if for one more scanning for cheching correctnes espicially for labels for example that the same labels 
will not be defined twice*/
labelInfo *checkingLabeles(labelInfo *head, char *token, int lineNum, char *fileNAme, int *correct);
/*Searching the exact label with exact needed type for the label.*/
labelInfo * findExactNeededElement(labelInfo *element, char *labelName,char type);
/*For creating new label table for exact new label elements which exist*/
void gettingCorrectnessInfoToElement(labelInfo *element, char *aName, int aAdress, char aType, char aCoddingType);





#endif /* CORRECTNESSCHECKING_H  */

