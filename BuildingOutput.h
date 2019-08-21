

#ifndef BUILDINGOUTPUT_H
#define BUILDINGOUTPUT_H


#include "buildlabeltable.h"
#include "correctnesschecking.h"
#include "ConstructsAndDefinitions.h"




/* Building an output, as input pointer to input file, pointer to initial element of label table (head), the beginning address,
 name of file for creating file with ending word like .as, .ent, .ext, total memory that contain code and data address (totalAddressCode,
 totalAddressData).*/
void outputBuilder(FILE *fp, labelInfo *head, int address, char *fileName, int totalAddressCode,int totalAddressData);
/* For creating output for data addresses*/
int gettingAddressData(char *line, labelInfo *head,int currentAdress, char *entryFileName);
/* For creating output for code addresses*/
int gettingAddressCode(char *line, labelInfo *head);
/* in case than line is blank or contain commens sign at the beginning, ';'.*/
int needSkiping(char *runningStr);
/* Getting as input line of input file, initial pointer for the the label table, current running address, the extern and entry file name,
if they will be needed to be created and return and output how many steps of adrdress per linewas proceeded*/
int outputCommandAnilizer(char *runningStr, labelInfo *head,int addressCouner, char *externFileName, char *entryFileName);
/* Getting as input line of input file, initial pointer for the the label table,short commandOutput which has 16 bits to real
symbolize the outpus, current running address, the extern and entry file name,
, it really print output in output file and also return for opcode in needed to proceed 0 or 1 steps.*/
int OutputoperandAnalizer(labelInfo *head, char *token, short commandOutput, int addressCouner, char *externFileName, char *entryFileName);
/* Getting the pointer to label table, label name for giving the coddingType (Absolute/External/Relocatable)
  and as output getting the label address.*/
int getLabelAddressAndCoddingType(labelInfo *head, char *labelName, char *coddingType);
/* Getting as output the asm sum (#-90) will be -90 as output.*/
int getAsmNum(char *token);
/* Getting the pointer to label table, label name for giving the coddingType (Absolute/External/Relocatable) and methodtype
 (Coding\Data\Unknown). */
void getLabelCoddingType(labelInfo *head, char *labelName, char *methodType, char *coddingType);
/* 1 if the token like r6[r1], o else*/
int isAdressWithOperand(char *token);
#endif /* BUILDLABELTABLE_H */
