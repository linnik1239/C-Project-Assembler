/* This souce file for checking correctness, there 2 scanning correctness for file, first is
to check basis correcness per line,and the second especially for label if no exist more than one label
with the same name as can be correct for example label cannot be defined as absolute and external.*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "correctnesschecking.h"


/* check correctness for File whom 'fp' is pointing, getting also pointer to the beginning of label table,
and file name for printing error comments from which file errors were found.*/
int correctnessChecking(FILE *fp, labelInfo *head, char *fileName) {

	
	char runningStr[MAX_LINE_SIZE];/*For scanning every line*/
	char errorLine[MAX_LINE_SIZE];
	int correct = 1;     /*Correctness,initially the file is correct, the question is what will be with correctness latter.*/
	int lineNum = 1;     /* The number of line per scanning file */
	labelInfo *newCheckingTable=NULL;
	while ((fgets(runningStr, MAX_LINE_SIZE, fp)) != 0) { /*Scannig every line for correctness.*/
		if (*runningStr == '\n' || *runningStr == ';') {/*empty or comment line*/
			++lineNum;
			continue;
		}
		if (!isFullLineCorrect(head, runningStr,lineNum,fileName)) { /*Function for checking every line.*/
			correct = 0; /*If at leaat one line will not be correct, the correctness will be 0.*/
		}
		++lineNum;
	}
	lineNum = 0; /* Initializing again for new file. */
	fseek(fp, 0, SEEK_SET);
	while ((fgets(runningStr, MAX_LINE_SIZE, fp)) != 0) { /* New scanning foe errors espicially for labels if the labels for example are not defined twice.*/
		if (*runningStr == '\n' || *runningStr == ';') {
			++lineNum;
			continue;
		}
		++lineNum;
		newCheckingTable = checkingLabeles(newCheckingTable, runningStr, lineNum, fileName, &correct);/*Function for checking every line especially for labels. making new label table for this.*/
		
	}

	//dispPerson(newCheckingTable);

	deleteList(newCheckingTable); /*Deleeting the new label table.*/
	if (!correct) {
		printf("\nfile '%s' is not correct, object file was not created.\n\n", fileName);
                fprintf(stderr,"----------------------------------------------------------------------------------------\n");
	}


	if (!correct) {
		return NOT_CORRECT;
	}
	return 1;

}

/* return 1 if token is asm num like "#-7", else this function return 0.*/
int isAsmNum(char *token) {
	char *runningStr;
	int numDigits = 0;
	runningStr = token;
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
		++runningStr;
	}
	for (; isdigit(*runningStr); ++runningStr, ++numDigits);

	if (!numDigits) {

		return 0;
	}

	for (; isspace(*runningStr); ++runningStr);
	if (*runningStr == '\0' || *runningStr == '\n') {

		return 1;
	}

	return 0;


}
/*return 1 if token is correct second addressind mode with index operand like "r4[r5]"
in case of errors, 0 is returned and errors are printed for error file. */
int isCorrectAdressWithOperand(char *token, int lineNum, char *fileNAme) {
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
			fprintf(stderr, "**ERROR: file: %s, line: %d The register is not correct outside the brackets.\n", fileNAme, lineNum);
			return 0;
		}

		if (!(secondRegNum >=  '0' && secondRegNum <= '7')) {
			fprintf(stderr, "**ERROR: file: %s, line: %d The register is not correct inside brackets.\n", fileNAme, lineNum);
			return 0;
		}

		if (!(firstRegNum =='1' || firstRegNum == '3' || firstRegNum == '5' || firstRegNum == '7' ) ) {
			fprintf(stderr, "**ERROR: file: %s, line: %d In this type of addressing the register outside the bracket must be odd num\n", fileNAme, lineNum);
			return 0;
		}
		if (!(secondRegNum == '2' || secondRegNum == '4' || secondRegNum == '6' )) {
			fprintf(stderr, "**ERROR: file: %s, line: %d In this type of addressing the register inside the bracket must be even num\n", fileNAme, lineNum);
			return 0;
		}


		return 1;
	}
	return 0;
}
/*return 1 if token is in label table and is in code type else return 0*/
int isLabelData(labelInfo *head, char *token) {

	labelInfo *anchor;
	char currentToken[MAX_LINE_SIZE];
	strcpy(currentToken, token);

	if (isLabelDef(currentToken)) { /*In case the example as "MAIN:"*/
		REAL_LABLE(currentToken); 
	}
	for (anchor = head; anchor; anchor = anchor->next) { /*Searching in all table.*/
		if (!strcmp(anchor->name, currentToken) && anchor->type == 'D') {
			return 1;
		}
	}
	return 0;
}


/*return 1 if token is in label table and is in data type else return 0*/
int isLabelCode(labelInfo *head,  char *token) {

	labelInfo *anchor;
	char currentToken[MAX_LINE_SIZE];
	strcpy(currentToken, token);
	
	if (isLabelDef(currentToken)) {/*In case the example as "MAIN:"*/
		REAL_LABLE(currentToken); 
	}
	for (anchor = head; anchor; anchor = anchor->next) { /*Searching in all table.*/
		if (!strcmp(anchor->name, currentToken) && anchor->type == 'C') {
			return 1;
		}
	}
	return 0;


}
/*Check if name of label, tokem has Unknown coding type (not know if it for code or data).*/
int isLabelUnkown(labelInfo *head, char *token) {

	labelInfo *anchor;
	char currentToken[40];
	strcpy(currentToken, token);

	if (isLabelDef(currentToken)) {/*In case the example as "MAIN:"*/
		REAL_LABLE(currentToken); 
	}

	for (anchor = head; anchor; anchor = anchor->next) {/*Searching in all table.*/
		if (!strcmp(anchor->name, currentToken) && anchor->type == 'U') {
			return 1;
		}
	}
	return 0;
}

/*return 1 if token is exist in label table else return 0*/
int isLabelExist(labelInfo *head, char *token) {
	labelInfo *anchor;

	for (anchor = head; anchor; anchor = anchor->next) {/*Searching in all table.*/
		if (!strcmp(anchor->name, token) ) {
			return 1;
		}
	}
	return 0;
}

/*return 1 if token is register else return 0*/
int isReg(char *token) {


	char *commands = "r1,r2,r3,r4,r5,r6,r7,";
	char *commands2 = "r1|r2|r3|r4|r5|r6|r7|";

	/*Checking if exact needed register exist*/
	if (strstr(commands, token) && strstr(commands2, token) && (strlen(token) == 2)) {
		return 1;
	}

	return 0;
}



/*return 1 if token  register has any addressing option (0,1,2 or 3 addressing mode)for choisen opcode(like "lea" opcode for source operand)
in case of errors, 0 is returned and errors are printed for error file.
optionNum is number fo options. for example '2' is for option 1 and 2 for 'lea' opcode. '3' is for option 1,2,3. '4' is for options 0,1,2,3*/
int isAdressOption(labelInfo *head, char *token, int lineNum, char *fileNAme, char *numOperand,int optionNum) {
	if (isLabelUnkown(head, token)) {/*Option for this mode, label can be an external*/
		if (strlen(token)>30) {
			fprintf(stderr, "**ERROR: file: %s, line: %d Label cannot contain more than 30 digits.\n", fileNAme, lineNum);
			return 0;
		}

		if (isdigit(*token)) {
			fprintf(stderr, "**ERROR: file: %s, line: %d Incorrect label, label cannobt be begin with digit.\n", fileNAme, lineNum);
			return 0;
		}
		return 1;
	}
	if (isLabelData(head, token)) {/*Option for this mode, label can be an external*/
		if (strlen(token)>30) {
			fprintf(stderr, "**ERROR: file: %s, line: %d Label cannot contain more than 30 digits.\n", fileNAme, lineNum);
			return 0;
		}

		if (isdigit(*token)) {
			fprintf(stderr, "**ERROR: file: %s, line: %d Incorrect label, label cannobt be begin with digit.\n", fileNAme, lineNum);
			return 0;
		}
		return 1;
	}
	if (isLabelCode(head, token)) {/*Option for this mode, label can be an external*/
		if (strlen(token)>30) {
			fprintf(stderr, "**ERROR: file: %s, line: %d Label cannot contain more than 30 digits.\n", fileNAme, lineNum);
			return 0;
		}

		if (isdigit(*token)) {
			fprintf(stderr, "**ERROR: file: %s, line: %d Incorrect label, label cannobt be begin with digit.\n", fileNAme, lineNum);
			return 0;
		}
		return 1;
	}
	if (isAdressWithOperandAtAll(token)) {/*Option for this mode*/
		return isCorrectAdressWithOperand(token, lineNum, fileNAme);
	}
	if (optionNum == 2) {
		fprintf(stderr, "**ERROR: file: %s, line: %d For this opcode %s operand is not correct.\n", fileNAme, lineNum, numOperand);
		return 0;
	}
	if (isReg(token)) { /*In this option operand can be a register*/
		return 1;
	}
	if (optionNum == 3) {
		fprintf(stderr, "**ERROR: file: %s, line: %d For this opcode %s operand is not correct.\n", fileNAme, lineNum, numOperand);
		return 0;
	}
	if (isAsmNum(token)) {/*In this option operand can be a asm num like "#-12" */
		int asmNum = atoi(token + 1);
		if (asmNum < -4096 || asmNum>4095) {
			fprintf(stderr, "**ERROR: file: %s, line: %d The num %d  is out of range (only signed 13 bits is possible).\n", fileNAme, lineNum, asmNum);
			return 0;
		}
		return 1;
	}
	fprintf(stderr, "**ERROR: file: %s, line: %d For this opcode  %s operand is not correct.\n", fileNAme, lineNum, numOperand);
	return 0;
}




/*Check if token is opcode or not*/
int isOpcode(char *token) {

	char *commands= "mov,cmp,add,sub,not,clr,lea,inc,dec,jmp,bne,red,prn,jsr,rts,stop,";
	char *commands2= "mov|cmp|add|sub|not|clr|lea|inc|dec|jmp|bne|red|prn|jsr|rts|stop|";

	
	/* Checking if exact needed opcode exist */
	if (strstr(commands, token) && strstr(commands2, token) && (strlen(token)==3  || strlen(token) == 4)) {
		if (strlen(token) == 3 && (!strcmp(token, "sto") || !strcmp(token, "top"))) {
			return 0;
		}
		
		return 1;
	}
	return 0;
}
/*Cheching the correctness pr line*/
int isLineCorrect(labelInfo *head,char *token, int lineNum, char *fileNAme) {
	char *currentToken;

	char *commands = "mov,cmp,add,sub,not,clr,lea,inc,dec,jmp,bne,red,prn,jsr,rts,stop,";
	char *commands2 = "mov|cmp|add|sub|not|clr|lea|inc|dec|jmp|bne|red|prn|jsr|rts|stop|";

	char *gettingStr;
	int commandNum;

	currentToken = token;

	if (!(gettingStr = strstr(commands, currentToken)) || !strstr(commands2, currentToken)) {
		fprintf(stderr, "**ERROR: file: %s, line: %d the opcode was not written correctly.\n", fileNAme, lineNum);
		return 0;
	}
	commandNum = (int)((gettingStr - commands) / 4); /*For finding num for command opcide in order.*/


	/*Checking the correctness for opcode with possible options.*/												 
	switch (commandNum) {
	case movS:
	case addS:
	case subS:
		return checkForChoisenCategory(head, currentToken, CAT_0123_123, lineNum, fileNAme);
	case cmpS:
		return checkForChoisenCategory(head, currentToken, CAT_0123_0123, lineNum, fileNAme);
	case notS:
	case clrS:
	case incS:
	case decS:
	case jmpS:
	case bneS:
	case redS:
	case jsrS:
		return checkForChoisenCategory(head, currentToken, CAT_123, lineNum, fileNAme);
	case leaS:
		return checkForChoisenCategory(head, currentToken, CAT_12_123, lineNum, fileNAme);
	case prnS:
		return checkForChoisenCategory(head, currentToken, CAT_0123, lineNum, fileNAme);
	case rtsS:
	case stopS:
		return checkForChoisenCategory(head, currentToken, CAT_, lineNum, fileNAme);
	default:
		return 0;
	}
	return 0;


}
/*When categorry is choisen like CAT_****_**** this function check correctnrss for this categorry.*/
int checkForChoisenCategory(labelInfo *head, char *currentToken, int category, int lineNum, char *fileNAme) {
	char *delim = "\n\t ";
	char *delim3 = "";
 	char *commaPtr; /*Pointer for first place in string where ',' is appeared.*/
	char beforeComma[50]; /*string befoer first ','*/
	char *beforeCommaPtr; /*pointer to string befoer first ','*/
	char afterComma[50];  /* string after first ','*/
	char afterCommaNew[50];
	char *afteCommaPtr;   /* pointer to string after first ','*/

	switch (category) {
		/*Categories for 2 operands*/
	case CAT_0123_123:
	case CAT_0123_0123:
	case CAT_12_123:

		currentToken = strtok(NULL, delim3);
		if (currentToken == NULL) {
			fprintf(stderr, "**ERROR: file: %s, line: %d Nothing was written after opcode.\n", fileNAme, lineNum);
			return 0;
		}
		commaPtr = strstr(currentToken, ",");
		if (!commaPtr) {
			fprintf(stderr, "**ERROR: file: %s, line: %d For this opcode for 2 operands needed ',' between operands.\n", fileNAme, lineNum);
			return 0;
		}
		strncpy(beforeComma, currentToken, commaPtr - currentToken);/*making string until first ','.*/
		beforeComma[commaPtr - currentToken] = 0;
		strcpy(afterComma, commaPtr);
		beforeCommaPtr = strtok(beforeComma, delim);
		if (beforeCommaPtr == NULL) {
			fprintf(stderr, "**ERROR: file: %s, line: %d Nothing was written before first ','.\n", fileNAme, lineNum);
			return 0;
		}
		if (currentToken[strlen(currentToken) - 1] == ',') {   /*??*/
			*(currentToken + strlen(currentToken) - 1) = '\0';

		}
		if (category == CAT_12_123) { /*For first operand*/
			if (!isAdressOption(head, beforeCommaPtr, lineNum, fileNAme, "first",2)/*!isTwoAdressOption(head, beforeCommaPtr, lineNum, fileNAme, "first")*/) {
				return 0;
			}
		}
		else {  /* For first operand   CAT_0123_123,  CAT_0123_0123 will be default for first operand */
			if (!isAdressOption(head, beforeCommaPtr, lineNum, fileNAme, "first",4)/*!isFourAdressOption(head, beforeCommaPtr, lineNum, fileNAme, "first")*/) {
				return 0;
			}

		}
		if (afterComma[0] != ',') {
			fprintf(stderr, "**ERROR: file: %s, line: %d for this opcode for 2 operands needed ',' between operands.\n", fileNAme, lineNum);
			return 0;
		}
		strcpy(afterCommaNew, afterComma+1);
		afteCommaPtr = strtok(afterComma +1, delim); /*String after first ','.*/
		if (category == CAT_0123_0123) { /*For second operand, if addressind options 0,1,2,3 are possible*/
			if (!isAdressOption(head, afteCommaPtr, lineNum, fileNAme, "second",4)/*!isFourAdressOption(head, afteCommaPtr, lineNum, fileNAme,"second")*/) {
				return 0;
			}
		}
		else { /* For second operand  CAT_12_123,  CAT_0123_123 will be default for first operand, options 1,2,3 are possible */
			if (!isAdressOption(head, afteCommaPtr, lineNum, fileNAme, "second",3)/*!isThreeAdressOption(head, afteCommaPtr, lineNum, fileNAme, "second")*/) {
				return 0;
			}
		}
		afteCommaPtr = strtok(NULL, delim);
		if (afteCommaPtr != NULL && *afteCommaPtr != ';') {
			fprintf(stderr, "**ERROR: file: %s, line: %d Not needed things were written after second operand.\n", fileNAme, lineNum);
			return 0;
		}
		afteCommaPtr = strtok(afterCommaNew, delim);
		afteCommaPtr = strtok(NULL, delim);
		if (afteCommaPtr != NULL && *afteCommaPtr != ';') {
			fprintf(stderr, "**ERROR: file: %s, line: %d Not needed things were written after second operand.\n", fileNAme, lineNum);
			return 0;
		}
		return 1;
		/*For one operand*/
	case CAT_123:
	case CAT_0123:
		currentToken = strtok(NULL, delim);
		if (currentToken == NULL) {
			fprintf(stderr, "**ERROR: file: %s, line: %d Nothing was written after opcode.\n", fileNAme, lineNum);
			return 0;
		} 
		if (category == CAT_123) { /* For addressind options 1,2,3 are possible*/

			if (!isAdressOption(head, currentToken, lineNum, fileNAme, "the",3)/*!isThreeAdressOption(head, currentToken, lineNum, fileNAme,"the")*/) {
				return 0;
			}
		}
		else {  /* For addressind options 0,1,2,3 are possible as default*/
			if (!isAdressOption(head, currentToken, lineNum, fileNAme, "the",4)/*!isFourAdressOption(head, currentToken, lineNum, fileNAme,"the")*/) {
				return 0;
			}
		}
		currentToken = strtok(NULL, delim);
		if (currentToken != NULL && *currentToken != ';') {
			fprintf(stderr, "**ERROR: file: %s, line: %d Not needed things were written after operand.\n", fileNAme, lineNum);
			return 0;
		}
		return 1;
	case CAT_:/*For opcode without any operand. like "stop"*/
		currentToken = strtok(NULL, delim);
		if (currentToken != NULL && *currentToken != ';') {
			fprintf(stderr, "**ERROR: file: %s, line: %d Not needed things were written after opcode.\n", fileNAme, lineNum);

			return 0;
		}
		return 1;
	default: 
		fprintf(stderr, "**ERROR: file: %s, line: %d Not Correct option.\n", fileNAme, lineNum);
		return 0;

	}
	fprintf(stderr, "**ERROR: file: %s, line: %d Not Correct option.\n", fileNAme, lineNum);
	return 0;
}
/*Cheching the correctness pr line*/
int isFullLineCorrect(labelInfo *head, char *token,int lineNum,char *fileNAme) {
	char *delim = "\n\t ";
	char *delim2 = "";
	char *currentToken;
	char tokenCommands[MAX_LINE_SIZE];
	char checkRegOpcode[5];   /*For option if one label is with same name as opcode or name of register*/
	int spaceAtBeginnigLine = 0; /*Chech if exist space before label.*/
	int mustComeBack = 0;  /*For correct option like "X:     .extern DATA1", skpping first label  if needed*/
	if (token == NULL) {
		return 1;
	}
	if (isspace(*token)) {
		spaceAtBeginnigLine = 1;
	}
	strcpy(tokenCommands, token);

	currentToken = strtok(tokenCommands, delim);

	if (currentToken == NULL) {
		return 1; /*Empty line*/
	}
	if (isLabelDef(currentToken)) {
		if (isdigit(*currentToken)) {
			fprintf(stderr, "**ERROR: file: %s, line: %d Label cannot Be begin with digit.\n", fileNAme, lineNum);
			return 0;
		}
		mustComeBack = 1; /*For correct option like "X:     .extern DATA1", skpping first label*/
	}
	do {
		if (!strcmp(".data", currentToken)) { /*if first word is ".data"*/
			currentToken = strtok(NULL, delim2);
			if (currentToken == NULL) {
				fprintf(stderr, "**ERROR: file: %s, line: %d Nothing was written after data declaration.\n", fileNAme, lineNum);
				return 0;
			}
			return  checkCommaCorrectness(currentToken, lineNum, fileNAme);/*Check after ".data"*/
	
		}
		if (!strcmp(currentToken, ".string")) { /*if first word is ".string"*/

			currentToken = strtok(NULL, delim);

			if (currentToken == NULL) {
				fprintf(stderr, "**ERROR: file: %s, line: %d Nothing was written after string declaration.\n", fileNAme, lineNum);
				return 0;
			}


			return isStringCorrect(currentToken, lineNum, fileNAme);/*Check after ".string"*/


		}
		if (isOpcode(currentToken)) {

			return isLineCorrect(head, currentToken, lineNum, fileNAme); /*Check line for opcode*/

		}
		if (*currentToken == ';') {
			/*For line of comments.*/
			return 1;

		}

		if (isLabelCode(head, currentToken)) { /*Check for label opcode*/
			if (isdigit(*currentToken)) {
				fprintf(stderr, "**ERROR: file: %s, line: %d Label cannot Be begin with digit.\n", fileNAme, lineNum);
				return 0;
			}
			if (strlen(currentToken) > 30) {
				fprintf(stderr, "**ERROR: file: %s, line: %d Label cannot contain more than 30 digits.\n", fileNAme, lineNum);
				return 0;
			}
			if (spaceAtBeginnigLine) {
				fprintf(stderr, "**ERROR: file: %s, line: %d Label must be at beginnig of line without any space before.\n", fileNAme, lineNum);
			}

			if (strlen(currentToken) < 5) { /*Check if label was defines as opcode or operand*/
				strcpy(checkRegOpcode, currentToken);
				if (isLabelDef(checkRegOpcode)) {
					checkRegOpcode[strlen(checkRegOpcode) - 1] = '\0';
				}
				if (isReg(checkRegOpcode)) {
					fprintf(stderr, "**ERROR: file: %s, line: %d Label ,%s cannot be with the same name as register.\n", fileNAme, lineNum, checkRegOpcode);
				}
				if (isOpcode(checkRegOpcode)) {
					fprintf(stderr, "**ERROR: file: %s, line: %d Label, %s cannot be with the same name as opcode.\n", fileNAme, lineNum, checkRegOpcode);
				}
			}

			currentToken = strtok(NULL, delim);
			if (currentToken == NULL) {
				fprintf(stderr, "**ERROR: file: %s, line: %d Nothins was written after label definition.\n", fileNAme, lineNum);
				return 0;
			}
			return isLineCorrect(head, currentToken, lineNum, fileNAme); /*Check line after label definition*/
		
		}
		if (isLabelData(head, currentToken)) { /*Check for label data*/
			if (isdigit(*currentToken)) {
				fprintf(stderr, "**ERROR: file: %s, line: %d Label cannot Be begin with digit.\n", fileNAme, lineNum);
				return 0;
			}
			if (strlen(currentToken) > 30) {
				fprintf(stderr, "**ERROR: file: %s, line: %d Label cannot contain more than 30 digits.\n", fileNAme, lineNum);
				return 0;
			}
			if (spaceAtBeginnigLine) {
				fprintf(stderr, "**ERROR: file: %s, line: %d Label must be at beginnig of line withou any space before.\n", fileNAme, lineNum);
			}
			if (strlen(currentToken) < 5) {
				strcpy(checkRegOpcode, currentToken);
				if (isLabelDef(checkRegOpcode)) {
					checkRegOpcode[strlen(checkRegOpcode) - 1] = '\0';
				}
				if (isReg(checkRegOpcode)) {
					fprintf(stderr, "**ERROR: file: %s, line: %d Label, %s  cannot be with the same name as register.\n", fileNAme, lineNum, checkRegOpcode);
				}
				if (isOpcode(checkRegOpcode)) {
					fprintf(stderr, "**ERROR: file: %s, line: %d Label, %s cannot be with the same name as opcode.\n", fileNAme, lineNum, checkRegOpcode);
				}
			}

			currentToken = strtok(NULL, delim);
			if (currentToken == NULL) {
				fprintf(stderr, "**ERROR: file: %s, line: %d Nothins was written after label definition.\n", fileNAme, lineNum);
				return 0;
			}
			if (!strcmp(currentToken, ".string")) {

				currentToken = strtok(NULL, delim);

				if (currentToken == NULL) {
					fprintf(stderr, "**ERROR: file: %s, line: %d Nothing was written after string declaration.\n", fileNAme, lineNum);
					return 0;
				}
				return isStringCorrect(currentToken, lineNum, fileNAme);
			
			}
			if (!strcmp(currentToken, ".data")) {
				currentToken = strtok(NULL, delim2);
				if (currentToken == NULL) {
					fprintf(stderr, "**ERROR: file: %s, line: %d Nothing was written after data declaration.\n", fileNAme, lineNum);
					return 0;
				}
				return checkCommaCorrectness(currentToken, lineNum, fileNAme);
	
			}
			if (!strcmp(currentToken, ".entry")) {
				currentToken = strtok(NULL, delim);
				if (currentToken == NULL) {
					fprintf(stderr, "**ERROR: file: %s, line: %d Nothing were written afeter '.entry'.\n", fileNAme, lineNum);
					return 0;
				}
				if (!isLabelExist(head, currentToken)) {
					fprintf(stderr, "**ERROR: file: %s, line: %d This label, %s was not defined in this file.\n", fileNAme, lineNum, currentToken);
					return 0;
				}
				currentToken = strtok(NULL, delim);
				if (currentToken == NULL) {

					return 1;
				}
				if (currentToken != NULL && *currentToken != ';') {
					fprintf(stderr, "**ERROR: file: %s, line: %d Not needed (%s) things were written in the addition.\n", fileNAme, lineNum, currentToken);
					return 0;
				}
				return 1;
			}
			if (!strcmp(currentToken, ".extern")) {
				currentToken = strtok(NULL, delim);
				if (currentToken == NULL) {
					fprintf(stderr, "**ERROR: file: %s, line: %d Nothing were written after '.extern'.\n", fileNAme, lineNum);
					return 0;
				}
				if (!isLabelExist(head, currentToken)) {
					if (!isLabelExist(head, currentToken)) {
						fprintf(stderr, "**ERROR: file: %s, line: %d This label (%s) was not defined in this file.\n", fileNAme, lineNum, currentToken);
					}
					return 0;
				}
				currentToken = strtok(NULL, delim);
				if (currentToken != NULL && *currentToken != ';') {
					fprintf(stderr, "**ERROR: file: %s, line: %d Not needed things (%s) were written in the addition.\n", fileNAme, lineNum, currentToken);
					return 0;
				}
				return 1;
			}
			return 1;
		}
		if (!strcmp(currentToken, ".entry")) {
			currentToken = strtok(NULL, delim);
			if (currentToken == NULL) {
				fprintf(stderr, "**ERROR: file: %s, line: %d Nothing were written afeter '.entry'.\n", fileNAme, lineNum);
				return 0;
			}
			if (!isLabelExist(head, currentToken)) {
				fprintf(stderr, "**ERROR: file: %s, line: %d This label '%s' was not defined in this file.\n", fileNAme, lineNum, currentToken);
				return 0;
			}
			currentToken = strtok(NULL, delim);
			if (currentToken == NULL) {

				return 1;
			}
			if (currentToken != NULL && *currentToken != ';') {
				fprintf(stderr, "**ERROR: file: %s, line: %d Not needed things (%s) were written in the addition.\n", fileNAme, lineNum, currentToken);
				return 0;
			}
			return 1;
		}
		if (!strcmp(currentToken, ".extern")) {
			currentToken = strtok(NULL, delim);
			if (currentToken == NULL) {
				fprintf(stderr, "**ERROR: file: %s, line: %d Nothing were written afeter '.extern'.\n", fileNAme, lineNum);
				return 0;
			}
			if (!isLabelExist(head, currentToken)) {
				fprintf(stderr, "**ERROR: file: %s, line: %d This label was not defined in this file.\n", fileNAme, lineNum);
				return 0;
			}
			currentToken = strtok(NULL, delim);
			if (currentToken != NULL && *currentToken != ';') {
				fprintf(stderr, "**ERROR: file: %s, line: %d Not needed things (%s) were written in the addition.\n", fileNAme, lineNum, currentToken);
				return 0;
			}
			return 1;
		}
		currentToken = strtok(NULL, delim);
		if (currentToken == NULL) {
			fprintf(stderr, "**ERROR: file: %s, line: %d Nothing was written after data declaration.\n", fileNAme, lineNum);
			return 0;
		}
	}while (mustComeBack--); /*Check again if needed.*/
	fprintf(stderr, "**ERROR: file: %s, line: %d The first word for assembly operation is not correct\n", fileNAme, lineNum);
	return 0;
}

/*Check if token is numeric string*/
int isStrNum(char *token) {
	char *delim = "\n\r0123456789 ";
	char *newToken;
	char *tokenPtr;

	if (atoi(token)) {
		if (strstr(token, ".")) {
			return 0;
		}
		tokenPtr=token;
		for (; isspace(*tokenPtr); ++tokenPtr);
		if (*tokenPtr == '-' || *tokenPtr == '+') {
			++tokenPtr;
		}
		for (; isdigit(*tokenPtr); ++tokenPtr);
		for (; isspace(*tokenPtr); ++tokenPtr);
		if (*tokenPtr != 0 && *tokenPtr != '\n' && *tokenPtr != EOF) {
			return 0;
		}
		return 1;
	}
	newToken = strtok(token, delim); /*For option than number is like 00000 (correct)*/
	if (newToken == NULL) {
		return 1;
	}
	return 0;

}
/* Cheching correctness after  ".data" */
int checkCommaCorrectness(char *token, int lineNum, char *fileNAme) {
	char *currentToken ;
	char localToken[MAX_LINE_SIZE];
	char localToken2[MAX_LINE_SIZE];
	char localToken3[MAX_LINE_SIZE];
	char localToken33[MAX_LINE_SIZE];
	char *commentPtr;
	char *delim = "";
	char *delim2 = "\n\t ";
	char *delim3 = "\n\t, ";
	char *delim4 = ",";
	unsigned int j;
	localToken[0] = '\0'; /*For function strcat*/
	localToken2[0] = '\0';
	localToken3[0] = '\0';
	localToken33[0] = '\0';
	currentToken = token;
	while (currentToken) { /*Get all line for data with spaces*/
		strcat(localToken, currentToken);
		currentToken = strtok(NULL, delim);
	}
	if (commentPtr = strstr(localToken, ";")) {
		localToken[commentPtr - localToken] = '\0'; /*Get all line for data with spaces until comment sign ';'*/
	}
	strcpy(localToken2, localToken);
	currentToken = strtok(localToken2, delim2); /*Get all line for data without spaces*/
	while (currentToken) {
		strcat(localToken3, currentToken);
		currentToken = strtok(NULL, delim2);

	}
	strcpy(localToken33, localToken3);
	currentToken = strtok(localToken3, delim3); /*Check for incorrect characters*/
	while (currentToken) {
		if (!isStrNum(currentToken)) {
			fprintf(stderr, "**ERROR: file: %s, line: %d invalid characters string (%s) was written for data\n", fileNAme, lineNum, currentToken);
			return 0;
		}
		currentToken = strtok(NULL, delim3);
	}
	for (j = 1; j<strlen(localToken33); ++j) { /* Check if exist 2 commas like ",,"*/

		if (localToken33[j] == ',' && localToken33[j - 1] == ',') {
			fprintf(stderr, "**ERROR: file: %s, line: %d 2 contihuas ',' were written between 2 data elements.\n", fileNAme, lineNum);
			return 0;
		}
	}
	if (localToken33[0] == ',' ) { /* Check if exist , At the beginnig*/
		fprintf(stderr, "**ERROR: file: %s, line: %d ',' was written before first data element.\n", fileNAme, lineNum);
		return 0;
	}
	if (localToken33[strlen(localToken33) - 1] == ',') { /* Check if exist , At the end*/
		fprintf(stderr, "**ERROR: file: %s, line: %d ',' was written after last data element.\n", fileNAme, lineNum);
		return 0;
	}

	currentToken = strtok(localToken, delim4);
	while (currentToken) {  /*Check for incorrect characters*/
		if (!isStrNum(currentToken)) {
			fprintf(stderr, "**ERROR: file: %s, line: %d invalid characters string was written for data\n", fileNAme, lineNum);
			return 0;
		}
		currentToken = strtok(NULL, delim4);

	}

	currentToken = strtok(localToken, delim3); /*Check correctnes for range.*/
	while (currentToken) {
		if (atoi(currentToken) < -16384 || 16383 < atoi(currentToken)) {
			fprintf(stderr, "**ERROR: file: %s, line: %d The numeric ,%d data is out of range (valid for signed 15 bits).\n", fileNAme, lineNum, atoi(currentToken));
			return 0;
		}
		currentToken = strtok(NULL, delim3);

	}

	return 1;
}
/*Cheching correctness after  ".string" */
int isStringCorrect(char *token, int lineNum, char *fileNAme) {
	char *delim = "\n\t ";
	char runningStr[MAX_LINE_SIZE];
	runningStr[0] = '\0';
	if (*token != '\"') {
		fprintf(stderr, "**ERROR: file: %s, line: %d Missing openning quatation for string\n", fileNAme, lineNum);
		return 0;
	}
	while (token) {
		strcat(runningStr, token);
		token = strtok(NULL, delim);

	}
	if (strstr(runningStr, ";")) { /*Check before first line for comments.*/
		strncpy(runningStr, runningStr,  strstr(runningStr, ";")-runningStr );
		runningStr[strstr(runningStr, ";")- runningStr] = 0;
	}

	if (runningStr[strlen(runningStr)-1] != '\"') {
		fprintf(stderr, "**ERROR: file: %s, line: %d Missing closing quatation for string\n", fileNAme, lineNum);
		return 0;
	}

	strcpy(runningStr, runningStr + 1);
	REAL_LABLE(runningStr); 

	if (strstr(runningStr, "\"")) {  /*If exist quatation not in one of the ends of string.*/
		fprintf(stderr, "**ERROR: file: %s, line: %d Not needed quatation in a middle of string\n", fileNAme, lineNum);
		return 0;
	}
	return 1;
}

/*This if for one more scanning for cheching correctnes espicially for labels for example that the same labels
will not be defined twice*/
labelInfo *checkingLabeles(labelInfo *head, char *token, int lineNum, char *fileNAme,int *correct) {
	char *delim = "\n\t ";
	char tokenCommands[MAX_LINE_SIZE];
	char *currentToken;
	labelInfo *anchor;
	if (token == NULL) {
		return head;
	}


	strcpy(tokenCommands, token);
	currentToken = strtok(tokenCommands, delim);
	if (currentToken == NULL) {
		return head;
	}
	
	if (isLabelDef(currentToken)) {

		anchor = findExactNeededElement(head, currentToken, 'A');
		if (anchor == NULL) {
			anchor = findExactNeededElement(head, currentToken, 'E'); /*Absolute label cannot be with same name as External label.*/
			if (anchor == NULL) {
				head = addNewPerson(createElement(), head);
				gettingCorrectnessInfoToElement(head, currentToken, 0, 0, 'A');


				currentToken = strtok(NULL, delim);
				if (currentToken == NULL) {
					correct = 0;
					return head;
				}
				if (!strcmp(currentToken, ".entry")) {
					currentToken = strtok(NULL, delim);
					if (currentToken == NULL) {
						correct = 0;
						return head;
					}
					anchor = findExactNeededElement(head, currentToken, 'E');
					if (anchor == NULL) {

						head = addNewPerson(createElement(), head); /*Creation new label in label table as Relocatable*/
						gettingCorrectnessInfoToElement(head, currentToken, 0, 0, 'R');
						return head;


					}
					fprintf(stderr, "**ERROR: file: %s, line: %d This label (%s) was defined as extern before\n", fileNAme, lineNum, currentToken);
					correct = 0;
					return head;
				}


				if (!strcmp(currentToken, ".extern")) {
					currentToken = strtok(NULL, delim);
					if (currentToken == NULL) {
						correct = 0;
						return head;
					}
					anchor = findExactNeededElement(head, currentToken, 'A');

					if (anchor == NULL) {
						/*Creation new label in label table as External*/
						head = addNewPerson(createElement(), head);
						gettingCorrectnessInfoToElement(head, currentToken, 0, 0, 'E');
						return head;
					}
					fprintf(stderr, "**ERROR: file: %s, line: %d This local label (%s) cannot be declared as external\n", fileNAme, lineNum, currentToken);
					correct = 0;
					return head;
				}



				return head;
			}
			fprintf(stderr, "**ERROR: file: %s, line: %d This label (%s) was defined as extern before\n", fileNAme, lineNum, currentToken);
			correct = 0;
			return head;
		}
			
		
		fprintf(stderr, "**ERROR: file: %s, line: %d This label (%s) was defined before\n", fileNAme, lineNum,currentToken);
		correct = 0;
		return head;
	}
	
	if ( !strcmp(currentToken, ".entry")) {
		currentToken = strtok(NULL, delim);
		if (currentToken == NULL) {
			correct = 0;
			return head;
		}
		anchor = findExactNeededElement(head, currentToken, 'E');
		if (anchor == NULL) {
			/*Creation new label in label table as Relocatable*/
			head = addNewPerson(createElement(), head);
			gettingCorrectnessInfoToElement(head, currentToken, 0, 0, 'R');
			return head;


		}
		fprintf(stderr, "**ERROR: file: %s, line: %d This label (%s) was defined as extern before\n", fileNAme, lineNum, currentToken);
		correct = 0;
		return head;
	}


	if (!strcmp(currentToken, ".extern") ) {
		currentToken = strtok(NULL, delim);
		if (currentToken == NULL) {
			correct = 0;
			return head;
		}
		anchor = findExactNeededElement(head, currentToken, 'A');

		if (anchor == NULL) {
			/*Creation new label in label table as External*/
			head = addNewPerson(createElement(), head);
			gettingCorrectnessInfoToElement(head, currentToken, 0, 0, 'E');
			return head;
		}
		fprintf(stderr, "**ERROR: file: %s, line: %d This local label (%s) cannot be declared as external\n", fileNAme, lineNum, currentToken);
		correct = 0;
		return head;
	}


		return head;
}
/*Searching the exact label with exact needed type for the label.*/
labelInfo *findExactNeededElement(labelInfo *element, char *labelName, char type) {
	labelInfo *anchor;
	char theLabelName[MAX_LINE_SIZE];
	strcpy(theLabelName, labelName); 
	if (isLabelDef(theLabelName)) {
		REAL_LABLE(theLabelName); 
	}
	for (anchor = element; anchor!=NULL; anchor = anchor->next) { /*Scanning all new lable table.*/
		if (!strcmp(anchor->name, theLabelName) && anchor->coddingType==type) {
			return anchor;
		}
	}
	return NULL;
}
/*For creating new label table for exact new label elements which exist*/
void gettingCorrectnessInfoToElement(labelInfo *element, char *aName, int aAdress, char aType, char aCoddingType) {
	if (isLabelDef(aName) ){

		REAL_LABLE(aName); 
	}
	element->name = (char*)malloc(sizeof(char)*(1 + strlen(aName)));  
	if (element->name == NULL) {
		fprintf(stderr, "Cannot allocate the name for element.\n");
		exit(1);
	}
	strcpy(element->name, aName);
	element->address = aAdress;
	element->type = aType;
	element->coddingType = aCoddingType;
}

/*return 1 if token is second addressind mode with index operand like "r7[r8]" */
int isAdressWithOperandAtAll(char *token) {
	char *runningStr;
	char *delim = "\n\t ";
	char theOperandStr[MAX_LINE_SIZE];
	theOperandStr[0] = '\0';
	runningStr = strtok(token, delim);
	while (runningStr) {
		strcat(theOperandStr, runningStr);
		runningStr = strtok(NULL, delim);
	}

	if (strlen(theOperandStr) != 6) {
		return 0;
	}
	
	if (theOperandStr[0] != 'r' || theOperandStr[2] != '[' || theOperandStr[3] != 'r' || theOperandStr[5] != ']') {
		return 0;
	}

	return 1;
}


