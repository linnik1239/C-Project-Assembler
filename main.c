/* Student name: Teodor Linnik */
/* Student ID: 312728017 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "correctnesschecking.h"
#include "buildlabeltable.h"
#include "BuildingOutput.h"




int needSkip(char *runningStr); /* For skkiing line in file during the scanning if needed.*/

int main(int argc, char *argv[])
{

	FILE *fp;         /*Pointer for every file from argv*/
	int countAdress = INITIAL_ADDRESS;  /* The address counter from the defined initial address.*/
	int totalAddressCode; /*The total needed space memory for code address*/
	int totalAddressData; /*The total needed space data for code address*/
	char runningStr[MAX_LINE_SIZE];  /*For cotying to this string every line if file*/
	char asmFileName[MAX_LINE_SIZE]; /*For cotying to this string every file name from argv from command line.*/
	struct labelInfo *labelCode = NULL; /*Linked list for labels table for code*/
	struct labelInfo *labelData = NULL; /*Linked list for labels table for data*/
	struct labelInfo *totalList = NULL; /*United linked list for labels table for data and code*/
	int i; /*for counting arguments.*/


	for (i = 1; i < argc; ++i) { /*Check all files in command line.*/
		strcpy(asmFileName, argv[i]); /*Fiel name from command line*/
		strcat(asmFileName, ".as");  /*Appending .as for beeing assembly file name*/
		printf("The output of File name : %s is:\n\n", asmFileName);
		fp = fopen(asmFileName, "r"); /*Trying to open the file.*/
		if (fp == NULL) { /*In case that file is not exist.*/
			fprintf(stderr, "The file %s is not exist.\n\n\n", asmFileName);
			continue;
		}
		fseek(fp, 0, SEEK_SET); /*placing at the beginnig of file for scanning.*/
		while ((fgets(runningStr, MAX_LINE_SIZE, fp)) != 0) { /* Scanning every line in file.*/
			if (needSkip(runningStr)) {
				continue;
			}
			
			labelCode = lineCheckCode(runningStr, labelCode, &countAdress); /*Getting pointer for the updated code table every line checking.*/

		}
		labelCode = reverseLinkedList(labelCode); /*Reversing linked list for order.*/
		totalAddressCode = countAdress - INITIAL_ADDRESS;
		fseek(fp, 0, SEEK_SET);/*placing at the beginnig of file for new scanning for data.*/
		while ((fgets(runningStr, MAX_LINE_SIZE, fp)) != 0) {/* Scanning every line in file.*/
			if (*runningStr == '\n' || *runningStr == ';') {
				continue;
			}
			labelData = lineCheckData(runningStr, labelData, &countAdress);/*Getting pointer for the updated code table every line checking.*/
		}

		totalAddressData = countAdress - totalAddressCode - INITIAL_ADDRESS;

		labelData = reverseLinkedList(labelData);/*Reversing linked list for order.*/
		
		totalList = linkTwoLists(labelCode, labelData); /*Unification of code and data tables.*/
		

		fseek(fp, 0, SEEK_SET);/*placing at the beginnig of file for new scanning for updating the labels if the are Absolute/External/Relocatable.*/

		while ((fgets(runningStr, MAX_LINE_SIZE, fp)) != 0) { /* Scanning every line in file for Updating the labeks.*/
			if (*runningStr == '\n' || *runningStr == ';') {
				continue;
			}
			makeEntryChanges(totalList, runningStr); /*Function for updating the label if needed.*/
		}
		

		fseek(fp, 0, SEEK_SET);/*placing at the beginnig of file for new scanning for Checking the error. */

		if (correctnessChecking(fp, totalList, asmFileName) == NOT_CORRECT) {
			INITIALIZE_ALL;
		}
		/*In case of correctness in file the outputs files are created.*/
		outputBuilder(fp, totalList, INITIAL_ADDRESS, argv[i], totalAddressCode, totalAddressData);
		INITIALIZE_ALL;
	}

	return 0;
}


/* For skkiing line in file during the scanning if needed.*/
int needSkip(char *runningStr) {
	char *strPtr = runningStr;
	for (; *strPtr==' ' || *strPtr == '\t';++strPtr);
	if (*strPtr == '\n' || *strPtr == EOF || *strPtr == ';') {
		return 1;
	}
	else {
		return 0;
	}
}