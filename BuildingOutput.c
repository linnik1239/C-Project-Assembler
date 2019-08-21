
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "BuildingOutput.h"

/* Initializing the pointers for output files: object, extern, entry files */
static outputFilePointers outputFp = {NULL,NULL,NULL};

/* Building an output, as input pointer to input file, pointer to initial element of label table (head), the beginning address,
name of file for creating file with ending word like .as, .ent, .ext, total memory that contain code and data address (totalAddressCode,
totalAddressData).*/
void outputBuilder(FILE *fp, labelInfo *head, int address,char *fileName,int totalAddressCode,int totalAddressData) {
	int addressCouner = address; /*The current running address from initial.*/
	char runningStr[MAX_LINE_SIZE];
	char objectFileName[MAX_LINE_SIZE];
	char externFileName[MAX_LINE_SIZE];
	char entryFileName[MAX_LINE_SIZE];
	fseek(fp, 0, SEEK_SET); /*scanning from the beggining of file pointer*/
	
	strcpy(objectFileName, fileName);
	strcat(objectFileName, ".ob");    /*Creating name of assember output file*/

	
	strcpy(externFileName, fileName);
	strcat(externFileName, ".ext"); /*Creating name of external output file*/


	strcpy(entryFileName, fileName);
	strcat(entryFileName, ".ent");  /*Creating name of entry output file*/

	outputFp.objectFp = fopen(objectFileName, "w");
	if (outputFp.objectFp == NULL) {
		fprintf(stderr, "Cannot create object file: %s \n", objectFileName);
		return;
	}
	printf("The object file: %s was created.\n", objectFileName);
	fprintf(outputFp.objectFp, "\n|------------------|");  /*Creating first cells for output file table.*/
	fprintf(outputFp.objectFp, "\n| Base 16 |Base 16 |");
	fprintf(outputFp.objectFp, "\n| Address |code    |");
	fprintf(outputFp.objectFp, "\n|------------------|");
	fprintf(outputFp.objectFp, "\n|     %4X  %4X   |", totalAddressCode, totalAddressData);
	fprintf(outputFp.objectFp, "\n|------------------|\n");
	while ((fgets(runningStr, MAX_LINE_SIZE, fp)) != 0) { /*Scanning every line in input file*/
		if (needSkiping(runningStr)) { /* In case that skip isneeded.*/
			continue;
		}   /*Frstly the output is created only from code lines in input file*/
		addressCouner += outputCommandAnilizer(runningStr, head, addressCouner, externFileName, entryFileName);


	}

	fseek(fp, 0, SEEK_SET); /*scanning from the beggining of file pointer for data lines*/
	while ((fgets(runningStr, MAX_LINE_SIZE, fp)) != 0) {
		if (needSkiping(runningStr)) {
			continue;
		} /* Secondly the output is created only from code lines in input file*/
		addressCouner += gettingAddressData(runningStr, head, addressCouner, entryFileName);


	}

	if (outputFp.externFp != NULL) {
		fclose(outputFp.externFp);
		outputFp.externFp = NULL;
	}
	if (outputFp.entryFp != NULL) {
		fclose(outputFp.entryFp);
		outputFp.entryFp=NULL;
	}

	fclose(outputFp.objectFp);
}
/* For creating output for code addresses*/
int gettingAddressCode(char *line, labelInfo *head) {
	char *delim = "\n\t ";
	char labelName[MAX_LINE_SIZE];
	char *commands = "mov,cmp,add,sub,not,clr,lea,inc,dec,jmp,bne,red,prn,jsr,rts,stop"; /*Possible opcde commands.*/
	int commandNum;

	int i = 0;
	char *token = strtok(line, delim);
	char *gettingStr;

	if (token == NULL) { /*In case that line is enmpty*/

		return 0;
	}
	if (*token == ';') { /*In case that line is comments */
		return 0;
	}

	++i;
	if (isLabelDef(token)) { /*Means that this is a label*/

		strcpy(labelName, token);
		 /* Nothing after label definition*/
		token = strtok(NULL, delim);
		if (token == NULL) {
			return 0;
		}
		if (*token == '.') {

			return 0;
		}


	}

	if (!(gettingStr = strstr(commands, token))) {
		/* No opcode commands exist*/
		return 0;
	}
	commandNum = (int)((gettingStr - commands) / 4); /*The opcode num of opcpde commands for example "stop" is 15 */

	return commandAnilizer(commandNum, token);
}

/* For creating output for data addresses*/
int gettingAddressData(char *line, labelInfo *head, int currentAdress,char *entryFileName) {
	char *delim = "\n\t ";
	char *delimStr = "\"";  /* Mainly for .string*/
	char *delimData = "\n\t, ";  /* Mainly to deal with ','*/
	char labelName[MAX_LINE_SIZE];
	char *token = strtok(line, delim);
	int i;
	char methodType;
	char coddingType;
	if (token == NULL) {

		return 0;
	}

	/* For option like: .data -7,-8,12*/
	if (!strcmp(".data", token)) { /*In case that data output will be with .data numbers.*/
		token = strtok(NULL, delimData);
		for (i = 0; token;++i) {
			fprintf(outputFp.objectFp, "| %4X    |  %04X  |\n", currentAdress+i, (atoi(token) & MASKING_BITS)); /* Printing output in output file.*/
			token = strtok(NULL, delimData);
		}
		fprintf(outputFp.objectFp, "|------------------|\n");
		return i;  /* Return how many steps address were proceeded */
	}
	/* For option like: .string "sdfgdsgsfd"*/
	if (!strcmp(token, ".string")) { /*In case that string output will be with .data numbers.*/
		token = strtok(NULL, delimStr);
		if (token == NULL) {
			return 0;
		}
		for (i = 0; token[i] != '\0'; ++i) { /* Printing output in output file.*/
			fprintf(outputFp.objectFp, "| %4X    |  %04X  |\n", currentAdress + i, (token[i] & MASKING_BITS));
			fprintf(outputFp.objectFp, "|------------------|\n");
		}
		fprintf(outputFp.objectFp, "| %4X    |  %04X  |\n", currentAdress + i, (token[i] & MASKING_BITS));
		fprintf(outputFp.objectFp, "|------------------|\n");

		return 1 + i; /* Return how many steps address were proceeded */
	}
	if (*token == '.') {
		return 0;
	}
	if (isLabelDef(token)) { /*Means that this is a label*/

		strcpy(labelName, token);
		REAL_LABLE(labelName); 


		getLabelCoddingType(head, labelName, &methodType, &coddingType);
		if (methodType == 'D' && coddingType == 'R') { /*In case tha needed to create/add information for entry file.*/
			if (outputFp.entryFp == NULL) {
				outputFp.entryFp = fopen(entryFileName, "w");
				if (outputFp.entryFp == NULL) {
					fprintf(stderr, "Cannot create entry file: %s \n", entryFileName);
					return 0;
				}
				printf("The entry file:  %s was created.\n", entryFileName);
			}
			fprintf(outputFp.entryFp, "%-8s %4X\n", labelName, currentAdress);
		}



		token = strtok(NULL, delim);
		if (token == NULL) {
			return 0;
		}
		if (*token == '.') {
			/* For option like: STR: .string "sdfgdsgsfd"*/
			if ( !strcmp(token, ".string")) {
				token = strtok(NULL, delimStr);
				if (token == NULL) {
					return 0;
				}
				for (i = 0; token[i] != '\0'; ++i) {/* Printing output in output file.*/
					fprintf(outputFp.objectFp, "| %4X    |  %04X  |\n", currentAdress + i, (token[i] & MASKING_BITS));
					fprintf(outputFp.objectFp, "|------------------|\n");
				}
				fprintf(outputFp.objectFp, "| %4X    |  %04X  |\n", currentAdress + i, (token[i] & MASKING_BITS));
				fprintf(outputFp.objectFp, "|------------------|\n");

				return 1+i;/* Return how many steps address were proceeded */
			}
			/* For option like: DATA: .data -7,-8,12*/
			if (!strcmp(token, ".data") ) {
				token = strtok(NULL, delimData);
				for (i = 0; token; token = strtok(NULL, delimData),++i) {/* Printing output in output file.*/
					fprintf(outputFp.objectFp, "| %4X    |  %04X  |\n", currentAdress + i, (atoi(token) & MASKING_BITS));
				}
				fprintf(outputFp.objectFp, "|------------------|\n");

				return i;/* Return how many steps address were proceeded */
			}

			return 0;

		}
	}


	return 0;
}

/* in case than line is blank or contain commens sign at the beginning, ';'.*/
int needSkiping(char *runningStr) {
	char *strPtr = runningStr;
	for (; *strPtr == ' ' || *strPtr == '\t'; ++strPtr);
	if (*strPtr == '\n' || *strPtr == EOF || *strPtr == ';') {
		return 1;
	}
	else {
		return 0;
	}
}

/* Getting as input line of input file, initial pointer for the the label table, current running address, the extern and entry file name,
if they will be needed to be created and return and output how many steps of adrdress per linewas proceeded*/
int outputCommandAnilizer(char *runningStr, labelInfo *head,int addressCouner, char *externFileName, char *entryFileName){
	char *commands = "mov,cmp,add,sub,not,clr,lea,inc,dec,jmp,bne,red,prn,jsr,rts,stop"; /*PossibleCommands*/
	char commandNum;
	char *gettingStr;
	char *delim = "\n\t ";
	char *token = strtok(runningStr, delim);
	short commandOutput = 0;    /* The real bit field for inserting coding output (needed 15 from 16 bits).*/
	char labelName[MAX_LINE_SIZE];
	char methodType;
	char coddingType;

	if (isLabelDef(token)) { /*Means that this is a label*/

		strcpy(labelName, token);
		REAL_LABLE(labelName); 

		getLabelCoddingType(head, labelName, &methodType, &coddingType);
		if (methodType == 'C' && coddingType == 'R') { /*In case tha needed to create/add information for entry file.*/
			if (outputFp.entryFp == NULL) {
				outputFp.entryFp = fopen(entryFileName, "w");
				if (outputFp.entryFp == NULL) {
					fprintf(stderr, "Cannot create entry file: %s \n", entryFileName);
					return 0;
				}
				printf("The entry file:  %s was created.\n", entryFileName);
			}
			fprintf(outputFp.entryFp, "%-8s %4X\n", labelName, addressCouner);
		}

		token = strtok(NULL, delim);
		if (token == NULL) {
			return 0;
		}
		if (*token == '.') {

			return 0;
		}


	}


	if (!(gettingStr = strstr(commands, token))) { 
		/* if noopcode */
		return 0;
	}

	commandNum = (int)((gettingStr - commands) / 4); /*The opcode num of opcpde commands for example "stop" is 15 */
	SET_INITIAL_INITIALIZER(commandOutput);  /* In initial: 0111-000000000000 */

	switch (commandNum)
	{             /* Masking output command acording to opcode, also return how many stes needed to proceed.*/
	case movS: 
	case cmpS:    /*  0111-00-0001-000000 */
		          /*  0111-10-0001-000000 */
				  /*  Next analizing:     */
		
	case addS: 
	case subS: SET_OPCODE(commandNum, commandOutput);
		SET_NUM_OPERANDS(GROUP_TWO_OPERANDS, commandOutput);
		return 2 + OutputoperandAnalizer(head, token, commandOutput, addressCouner, externFileName, entryFileName);
	case notS: 
	case clrS: SET_OPCODE(commandNum, commandOutput);
		SET_NUM_OPERANDS(GROUP_ONE_OPERANDS, commandOutput);
		OutputoperandAnalizer(head, token, commandOutput, addressCouner, externFileName, entryFileName);
		return 2;
	case leaS: SET_OPCODE(commandNum, commandOutput);
		SET_NUM_OPERANDS(GROUP_TWO_OPERANDS, commandOutput);
		return 2 + OutputoperandAnalizer(head, token, commandOutput, addressCouner, externFileName, entryFileName);
	case incS: 
	case decS: 
	case jmpS: 
	case bneS: 
	case redS: 
	case prnS: 
	case jsrS: SET_OPCODE(commandNum, commandOutput);
		SET_NUM_OPERANDS(GROUP_ONE_OPERANDS, commandOutput);
		OutputoperandAnalizer(head, token, commandOutput, addressCouner, externFileName, entryFileName);
		return 2;
	case rtsS:
	case stopS: SET_OPCODE(commandNum, commandOutput);
		SET_NUM_OPERANDS(GROUP_NO_OPERANDS, commandOutput);
		OutputoperandAnalizer(head, token, commandOutput, addressCouner, externFileName, entryFileName);
		return 1;
	default: return 0;

	}
	return 0;
}
/* Getting as input line of input file, initial pointer for the the label table,short commandOutput which has 16 bits to real
symbolize the outpus, current running address, the extern and entry file name,
, it really print output in output file and also return for opcode in needed to proceed 0 or 1 steps.*/
int OutputoperandAnalizer(labelInfo *head, char *token,short commandOutput,int addressCouner,char *externFileName, char *entryFileName) {
	char *delim = "\n\t, ";
	char firstOperand[MAX_LINE_SIZE];
	char secondOperand[MAX_LINE_SIZE];
	char labelCodingType;
	int labelAdress;
	int labelCodingNum;
	int firstAsmNum;
	int secondAsmNum;
	int firstRegNum;
	int secondRegNum;
	char twoOperandsStr[MAX_LINE_SIZE];
	token = strtok(NULL, delim);
	if (token == NULL) {   /* Creating an output, no operands:*/
		fprintf(outputFp.objectFp, "| %4X    |  %04X  |\n", addressCouner,( INITIAL_INITIALIZER|(commandOutput )));
		fprintf(outputFp.objectFp, "|------------------|\n");

		return 0;
	}
	strcpy(firstOperand, token);

	 
	token = strtok(NULL, delim);
	if (token == NULL) { /* Only one operand exist.*/

		if (isAsmNum(firstOperand)) { /* like #45 */
			firstAsmNum = getAsmNum(firstOperand);
			firstAsmNum <<= 2;
			firstAsmNum &= MASKING_BITS;
			
			commandOutput |= (0 << 2);
			fprintf(outputFp.objectFp, "| %4X    |  %04X  |\n", addressCouner, commandOutput);
			fprintf(outputFp.objectFp, "| %4X    |  %04X  |\n", addressCouner+1, firstAsmNum);
			fprintf(outputFp.objectFp, "|------------------|\n");

			return 0;
		}
		if (isLabelExist(head, firstOperand)) {  /*if operand is label*/
			labelAdress = getLabelAddressAndCoddingType(head, firstOperand, &labelCodingType);
			if (labelCodingType == 'E') {
				labelCodingNum = 1;
				if (outputFp.externFp == NULL) {   /*In case tha needed to create/add information for extern file.*/
					outputFp.externFp = fopen(externFileName, "w");
					if (outputFp.externFp == NULL) {
						fprintf(stderr, "Cannot create extern file: %s \n", externFileName);
						return 0;
					}
					printf("The extern file: %s was created.\n", externFileName);
				}
				fprintf(outputFp.externFp, "%-8s %4X  \n", firstOperand, addressCouner + 1);
			}
			else {
				labelCodingNum = 2;
			}

			labelAdress <<= 2;
			labelAdress |= labelCodingNum;  /*like 0000010011010-10*/
			
			commandOutput |= (1 << 2); /*Printing in output file*/
			fprintf(outputFp.objectFp, "| %4X    |  %04X  |\n", addressCouner, commandOutput);
			fprintf(outputFp.objectFp, "| %4X    |  %04X  |\n", addressCouner+1, labelAdress);
			fprintf(outputFp.objectFp, "|------------------|\n");

			return 0;
		}
		

		if (isAdressWithOperand(firstOperand)) { /* Like r1[r4]*/
												 /*Printing in output file*/
			commandOutput |= (2 << 2);
			fprintf(outputFp.objectFp, "| %4X    |  %04X  |\n", addressCouner, commandOutput);
			fprintf(outputFp.objectFp, "| %4X    |  %04X  |\n", addressCouner + 1, (((firstOperand[1]) - '0') << 2) | (((firstOperand[4]) - '0') << 8));
			fprintf(outputFp.objectFp, "|------------------|\n");

			return 0;
		}
		if (isReg(firstOperand)) {  /* like r4*/
			firstRegNum = firstOperand[1] - '0';
			firstRegNum <<= 2;
			firstRegNum &= MASKING_BITS;
			
			commandOutput |= (3 << 2);/*Printing in output file*/
			fprintf(outputFp.objectFp, "| %4X    |  %04X  |\n", addressCouner, addressCouner);
			fprintf(outputFp.objectFp, "| %4X    |  %04X  |\n", addressCouner+1, firstRegNum);
			fprintf(outputFp.objectFp, "|------------------|\n");

			return 0;
		}
		fprintf(outputFp.objectFp, "|------------------|\n");

		return 0;
	}
	/* In case of 2 operands.*/
	strcpy(secondOperand, token);
	/*Meaing that firstly will be printt for stringand after verifiing second operan the command, first and second operands will be printed.*/
	if (isAsmNum(firstOperand)) { /* like #45 */
		firstAsmNum = getAsmNum(firstOperand);
		firstAsmNum <<= 2;
		firstAsmNum &= MASKING_BITS;
		sprintf(twoOperandsStr, "| %4X    |  %04X  |\n", addressCouner + 1, firstAsmNum);
		commandOutput |= (0 << 4);
	}
	if (isLabelExist(head, firstOperand)) {
		labelAdress = getLabelAddressAndCoddingType(head, firstOperand, &labelCodingType);
		if (labelCodingType == 'E') {
			labelCodingNum = 1;
			if (outputFp.externFp == NULL) {   /*In case tha needed to create/add information for extern file.*/
				outputFp.externFp = fopen(externFileName, "w");
				if (outputFp.externFp == NULL) {
					fprintf(stderr, "Cannot create extern file: %s \n", externFileName);
					return 0;
				}
				printf("The extern file: %s was created.\n", externFileName);
			}



			fprintf(outputFp.externFp, "%-8s %4X  \n", firstOperand, addressCouner + 1);
		}
		else {
			labelCodingNum = 2;
		}
		labelAdress <<= 2;
		labelAdress |= labelCodingNum;  /*Printing in output string*/
		sprintf(twoOperandsStr, "| %4X    |  %04X  |\n", addressCouner + 1, labelAdress);
		commandOutput |= (1 << 4);
	}
	if (isAdressWithOperand(firstOperand)) { /* Like r1[r4]*/   /*Printing in output string*/
		sprintf(twoOperandsStr, "| %4X    |  %04X  |\n", addressCouner + 1, (((firstOperand[1]) - '0') << 2) | (((firstOperand[4]) - '0') << 8));
		commandOutput |= (2 << 4);
	}
	if (isReg(firstOperand)) {        /* like r4*/
		firstRegNum = firstOperand[1] - '0';
		if (!isReg(secondOperand)) {
			firstRegNum <<= 8;
			firstRegNum &= MASKING_BITS;  /*Printing in output string*/
			sprintf(twoOperandsStr, "| %4X    |  %04X  |\n", addressCouner + 1, firstRegNum);
		}
		else {
			firstRegNum &= MASKING_BITS;
			firstRegNum <<= 6;
		}

		commandOutput |= (3 << 4);
	}

	if (isAsmNum(secondOperand)) {
		secondAsmNum = getAsmNum(secondOperand);
		secondAsmNum <<= 2;
		secondAsmNum &= MASKING_BITS;
		commandOutput |= (0 << 2);   /*Printing in output file*/
		fprintf(outputFp.objectFp, "| %4X    |  %04X  |\n", addressCouner, commandOutput);
		fprintf(outputFp.objectFp, "%s", twoOperandsStr);
		fprintf(outputFp.objectFp, "| %4X    |  %04X  |\n", addressCouner + 2, secondAsmNum);

	}
	if (isLabelExist(head, secondOperand)) {
		labelAdress = getLabelAddressAndCoddingType(head, secondOperand, &labelCodingType);
		if (labelCodingType == 'E') {
			labelCodingNum = 1;
			if (outputFp.externFp == NULL) {   /*In case tha needed to create/add information for extern file.*/
				outputFp.externFp = fopen(externFileName, "w");
				if (outputFp.externFp == NULL) {
					fprintf(stderr, "Cannot create extern file: %s \n", externFileName);
					return 0;
				}
				printf("The extern file: %s was created.\n", externFileName);
			}
			fprintf(outputFp.externFp, "%-8s %4X  \n", secondOperand, addressCouner + 2);
		}
		else {
			labelCodingNum = 2;
		}
		labelAdress <<= 2;
		labelAdress |= labelCodingNum;
		commandOutput |= (1 << 2);   /*Printing in output file*/
		fprintf(outputFp.objectFp, "| %4X    |  %04X  |\n", addressCouner, commandOutput);
		fprintf(outputFp.objectFp, "%s", twoOperandsStr);
		fprintf(outputFp.objectFp, "| %4X    |  %04X  |\n", addressCouner + 2, labelAdress);

	}
	if (isAdressWithOperand(secondOperand)) {
		commandOutput |= (2 << 2);  /*Printing in output file*/
		fprintf(outputFp.objectFp, "| %4X    |  %04X  |\n", addressCouner, commandOutput);
		fprintf(outputFp.objectFp, "%s", twoOperandsStr);
		fprintf(outputFp.objectFp, "| %4X    |  %04X  |\n", addressCouner + 2, (((secondOperand[1]) - '0') << 2) | (((secondOperand[4]) - '0') << 8));

	}
	if (isReg(secondOperand)) {

		secondRegNum = secondOperand[1] - '0';
		if (isReg(firstOperand)) {
			secondRegNum |= firstRegNum;
		}
		secondRegNum <<= 2;
		secondRegNum &= MASKING_BITS;

		commandOutput |= (3 << 2);  /* Printing in output file*/
		fprintf(outputFp.objectFp, "| %4X    |  %04X  |\n", addressCouner, commandOutput);
		if (!isReg(firstOperand)) { /* like: mov r1,r1[r4]*/
			fprintf(outputFp.objectFp, "%s", twoOperandsStr);
			fprintf(outputFp.objectFp, "| %4X    |  %04X  |\n", addressCouner + 2, secondRegNum);
		}
		else   /*like: mov r1,r2*/
		{
			fprintf(outputFp.objectFp, "| %4X    |  %04X  |\n", addressCouner + 1, secondRegNum);
			fprintf(outputFp.objectFp, "|------------------|\n");

			return 0;
		}
	}
	fprintf(outputFp.objectFp, "|------------------|\n");

	return 1;
}

/* Getting the pointer to label table, label name for giving the coddingType (Absolute/External/Relocatable)
and as output getting the label address.*/
int getLabelAddressAndCoddingType(labelInfo *head, char *labelName, char *coddingType) {
	labelInfo *anchor;
	for (anchor = head; anchor; anchor = anchor->next) { /*scanning all label table*/
		if (!strcmp(anchor->name, labelName)) {
			
			*coddingType = anchor->coddingType;
			return anchor->address;
		}
	}
	return 0;
}
/* Getting the pointer to label table, label name for giving the coddingType (Absolute/External/Relocatable) and methodtype
(Coding\Data\Unknown). */
void getLabelCoddingType(labelInfo *head, char *labelName, char *methodType,char *coddingType) {
	labelInfo *anchor;
	for (anchor = head; anchor; anchor = anchor->next) {/*scanning all label table*/
		if (!strcmp(anchor->name, labelName)) {
			*coddingType= anchor->coddingType;
			*methodType = anchor->type;
			return;
		}
	}
	return ;
}
/* Getting as output the asm sum (#-90) will be -90 as output.*/
int getAsmNum(char *token) {
	char *runningStr;
	int numDigits = 0;
	runningStr = token;
	char asmNum[MAX_LINE_SIZE];
	int i=0;
	if (*runningStr == '\0') {

		return 0;
	}
	if (*runningStr != '#') {

		return 0;
	}
	if (*(runningStr + 1) == '\0') {
		return 0;
	}
	++runningStr;
	for (; isspace(*runningStr); ++runningStr);
	if (*runningStr == '+' || *runningStr == '-') {
		asmNum[i++] = *runningStr;
		++runningStr;
	}
	for (; isdigit(*runningStr); ++runningStr, ++numDigits) {
		asmNum[i++] = *runningStr;
	}
	asmNum[i] = '\0';	
	return atoi(asmNum);
}


/* 1 if the token like r6[r1], o else*/
int isAdressWithOperand(char *token) {
	char *runningStr;
	runningStr = token;
	char firstRegNum;
	char secondRegNum;
	for (; isspace(*runningStr); ++runningStr);
	if (*runningStr != 'r') {


		return 0;
	}
	++runningStr;
	if (*runningStr == '\0' || *runningStr == '\n') {

		return 0;
	}
	firstRegNum = *runningStr;

	++runningStr;
	if (*runningStr != '[') {

		return 0;
	}
	++runningStr;
	if (*runningStr != 'r') {

		return 0;
	}
	++runningStr;
	secondRegNum = *runningStr;

	++runningStr;
	if (*runningStr != ']') {

		return 0;
	}
	for (++runningStr; isspace(*runningStr); ++runningStr);
	if (*runningStr == '\0' || *runningStr == '\n') {

		if (!(firstRegNum >= '0' && firstRegNum <= '7')) {
			return 0;
		}

		if (!(secondRegNum >= '0' && secondRegNum <= '7')) {
			return 0;
		}

		if (!(firstRegNum == '1' || firstRegNum == '3' || firstRegNum == '5' || firstRegNum == '7')) {
			return 0;
		}
		if (!(secondRegNum == '2' || secondRegNum == '4' || secondRegNum == '6')) {
			return 0;
		}
		return 1;
	}
	return 0;
}

