#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "buildlabeltable.h"
#include "correctnesschecking.h"


/*This function getting the line from file, pointer for the last element from current code label table, pointer for current running address
this function return the pointer for the updated code table for last element and updated running address.*/
labelInfo * lineCheckCode(char *line, labelInfo *head, int *address) {
	char *delim = "\n\t "; /*For function strtok.*/
	char labelName[MAX_LINE_SIZE];
	char *commands =  "mov,cmp,add,sub,not,clr,lea,inc,dec,jmp,bne,red,prn,jsr,rts,stop,"; /*Possible opcode commands.*/
	char *commands2 = "mov|cmp|add|sub|not|clr|lea|inc|dec|jmp|bne|red|prn|jsr|rts|stop|"; /*Possible opcode commands.*/
	int commandNum; /*The opcode command num*/
	char *token = strtok(line, delim); /*Token every word from line.*/
	char *gettingStr;

	if (token == NULL) { /*In case that line is blank.*/

		return head;
	}
	if (*token == ';') {/*In case that first letter in line is a comment.*/
		return head;
	}
	if (!strcmp(".extern", token)) { /*In case that first word in line is ".extern".*/
		token = strtok(NULL, delim);
		if (token == NULL) { /*In case that line is blank.*/

			return head;
		}
		if (*token == ';') {/*In case that first letter in line is a comment.*/
			return head;
		}
		strcpy(labelName, token);

		if ((head = addNewPerson(createElement(), head)) == NULL) {/*Creating line in table for this label*/
			fprintf(stderr, "Cannot allocate label element.\n");
			exit(1);
		}
		gettingInfoToElement(head, token, 0 /*address is o*/, 'U'/*Unknown if it for coding or data*/, 'E' /*External codding type*/); /*Inserting information in table for this label.*/
		return head;
	}
	if (!strcmp(".entry", token)) {/*In case that first word in line is ".entry".*/
		/*Not needed to insert label for label table.*/
		return head;
	}




	if (isLabelDef(token)) { /*Means that this is a label*/

		strcpy(labelName, token);
		token = strtok(NULL, delim);
		if (token == NULL) {
			return head;
		}


		if (!strcmp(".extern", token)) { /* word ".extern" after label definition.*/
			token = strtok(NULL, delim);
			if (token == NULL) {

				return head;
			}
			if (*token == ';') {/*In case that first letter in line is a comment.*/
				return head;
			}
			strcpy(labelName, token);

			if ((head = addNewPerson(createElement(), head)) == NULL) {/*Creating line in table for this label*/
				fprintf(stderr, "Cannot allocate label element.\n");
				exit(1);
			}
			gettingInfoToElement(head, token, 0 /*address is o*/, 'U'/*Unknown if it for coding or data*/, 'E' /*External codding type*/); /*Inserting information in table for this label.*/
			return head;
		}
		if (!strcmp(".entry", token)) {
			/*Not needed to insert label for label table.*/
			return head;
		}

		if (*token == '.') { /*".data" and ".string" not for this function.*/

			return head;
		}
		if (labelDefinedExist(head, labelName) ){ /*If label exist we will not insert one more element to label list.*/
			return head;
		}
		if ((head = addNewPerson(createElement(), head)) == NULL) {
			fprintf(stderr, "Cannot allocate label element.\n");
			exit(1);
		}
		gettingInfoToElement(head, labelName, *address, 'C', 'A');/*Inserting information in table for this label.*/

	}

	if (!(strstr(commands, token)) || !(strstr(commands2, token))) {/* check if this word is opcode command.*/

		return head;
	}
	gettingStr = strstr(commands, token);
	commandNum = (int)((gettingStr - commands) / 4); /*This how to find opcode number in order, opcode numbers are in range 0-15*/

	*address += commandAnilizer(commandNum, token);/*Calculate how many steps is needed for advancing is ad*/
	return head;
}



/*This function getting the line from file, pointer for the last element from current data label table, pointer for current running address
this function return the pointer for the updated data table for last element and updated running address.*/
labelInfo * lineCheckData(char *line, labelInfo *head, int *address) {
	char *delim = "\n\t ";
	char labelName[MAX_LINE_SIZE];
	char *token = strtok(line, delim);/*Token every word from line.*/
	
	if (token == NULL) { /*In case that line is blank.*/

		return head;
	}
	if (*token == ';') {/*In case that first letter in line is a comment.*/
		return head;
	}
	if (!strcmp(token, ".data") || !strcmp(token, ".string")) { /*If the first word is ".data" or ".string"*/
		*address += dataAnalizer(token);
		return head;
	}
	if (isLabelDef(token)) { /*Means that this is a label*/
		strcpy(labelName, token);
		token = strtok(NULL, delim);
		if (token == NULL) {/* no word after label definition*/
			return head;
		}
		if (*token == '.') {
			if (strcmp(token, ".data") && strcmp(token, ".string")) { /*If the second word is ".data" or ".string"*/

				return head;
			}
			if (labelDefinedExist(head, labelName)) {/*Id label exist in label label the new element label not needed in label table.*/
				return head;
			}
			if ((head = addNewPerson(createElement(), head)) == NULL) {
				fprintf(stderr, "Cannot allocate label element.\n");
				exit(1);
			}
			gettingInfoToElement(head, labelName, *address, 'D','A'); /*Storing information for label in table.*/
			*address += dataAnalizer(token); /* Advance in address acording to data counter. */
			return head; /*Return pointer for last element for new updataed table.*/
		}
	}
	return head;
}
/*This function getting the number of defined opcode, the line, function return how many steps memory needed to proced for this command.*/
int commandAnilizer(int commandNum, char *token) {
	switch (commandNum) /* how many steps needed to advance in address acording to opcode and operands.*/
	{
	case movS: return 2 + operandAnalizer(token); /*If needed more than 2 steps in this opcode.*/
	case cmpS: return 2 + operandAnalizer(token);
	case addS: return 2 + operandAnalizer(token);
	case subS: return 2 + operandAnalizer(token);
	case notS: return 2;
	case clrS: return 2;
	case leaS: return 2 + operandAnalizer(token);
	case incS: return 2;
	case decS: return 2;
	case jmpS: return 2;
	case bneS: return 2;
	case redS: return 2;
	case prnS: return 2;
	case jsrS: return 2;
	case rtsS: return 1;
	case stopS: return 1;
	default: return 0;

	}
}

/*In case that 2 operands are addressing num 3 like " mov r1,r2" it return 0, else this return 1 needed for defining if needed
to proceed one more steps for for code counter.*/
int operandAnalizer(char *token) {
	char *delim = "\n\t, ";
	char *possibleRegisters = "r1,r2,r3,r4,r5,r6,r7";
	char firstOperand[30];
	char secondOperand[30];
	token = strtok(NULL, delim);
	if (token == NULL) {/*No first operand*/
		return 0;
	}
	strcpy(firstOperand, token);
	token = strtok(NULL, delim);
	if (token == NULL) { /*No second operand*/
		return 0;
	}
	strcpy(secondOperand, token);
	if (strstr(possibleRegisters, firstOperand) && strstr(possibleRegisters, secondOperand)) { /* like "mov r1,r2"*/
		return 0;
	}
	/*In default case of operands in opcode the proceededns in one more steps is needed.*/
	return 1;
}

/*Creating element*/
labelInfo * createElement() {
	labelInfo *newElement;
	newElement = (labelInfo*)malloc(sizeof(labelInfo));
	if (newElement == NULL) {
		fprintf(stderr, "Error");
		exit(1);
	}
	newElement->next = NULL;
	newElement->name = NULL;
	newElement->type = 'U';    /*As default every label is unkmown (not dosing andnot data type)  if labels are not updated latter.*/
	newElement->coddingType = 'A';  /*As default every label is Absolute  if labels are not updated latter.*/
	return newElement;
}
void printElement(labelInfo *element) {

	if (element == NULL) {
		printf("Empty element for printing \n\n");
		return;
	}
	printf("Label name: %s\n", element->name);
	printf("Label address: %d\n", element->address);
	printf("Label type: %c\n", element->type);
	printf("Label coding type: %c\n", element->coddingType);
	printf("\n");

}
/*For updating information  in table for label element.*/
void gettingInfoToElement(labelInfo *element, char *aName, int aAdress, char aType, char aCoddingType) {
  	/*if (aType != 'U') {
  		if (!isLabelDef(aName)) {  
  			return;
  		}
  		aName[strlen(aName) - 1] = '\0';
	}*/
	if (isLabelDef(aName)) {  /* Means that this is defines as label like "MAIN:" */
		REAL_LABLE(aName); 
	}
	element->name = (char*)malloc(sizeof(char)*(1 + strlen(aName)));  /*Allocating memory for new name in label table*/
	if (element->name == NULL) {
		fprintf(stderr, "Cannot allocate the name for element.\n");
		exit(1);
	}
	
	strcpy(element->name, aName);
	element->address = aAdress;
	element->type = aType;   /* Like 'D' : data code*/

	element->coddingType = aCoddingType; /* Like 'E' : External*/
}
void dispPerson(labelInfo *element) {
	labelInfo *personPointer = element;
	while (personPointer != NULL) {
		printElement(personPointer);
		personPointer = personPointer->next;
	}

}
/*Adding new person for label table  return the pointer for last added element for new linked list*/
labelInfo * addNewPerson(labelInfo *person, labelInfo *list) {

	if (person == NULL) { /* new element is NULL*/
		return list;
	}
	if (list == NULL) { /* List is NULL */
		list = person;
		return list;
	}
	person->next = list; /*Inserting new person for the list.*/
	list = person;
	return list;
}

/*This functionthe line, function return how many step smemory needed to proced for this data.*/
int dataAnalizer(char *token) {
	char *delim = "\n\t, ";
	if (token == NULL) {
		return 0;
	}
	if (!strcmp(token, ".data")) {
		int dataCounter = 0;
		token = strtok(NULL, delim);
		while (token != NULL) {
			++dataCounter;
			if (*token!='0' && !atoi(token)) {
				return dataCounter; /*Not valid string for data*/
			}
			token = strtok(NULL, delim);
		}
		return dataCounter;  /*Return how many steps in data counter needed to proceed in this line*/

	}
	if (!strcmp(token, ".string")) {
		int strCounter = 0;
		token = strtok(NULL, delim);
		while (token != NULL) {
			strCounter += (int)strlen(token);;
			token = strtok(NULL, delim);
		}
		return strCounter - 1;/*Return how many steps in string counter needed to proceed in this line*/

	}

	return 0;
}
/*This function hetting the pointer for first element of linked list and returning the reversed list with pointer for
last element for linked list.*/
labelInfo * reverseLinkedList(labelInfo *head) {
	labelInfo *currentPointer;
	labelInfo *newHead = NULL;
	labelInfo *runnerList;

	if (head == NULL) {
		return NULL;
	}
	if (head->next == NULL) {
		return head;
	}
	runnerList = head;
	while (runnerList) {
		currentPointer = runnerList;
		runnerList = runnerList->next;
		currentPointer->next = newHead;
		newHead = currentPointer;
	}
	head->next = NULL;
	return newHead;

}
/*This function get 2 linked lists, linking them and return the pointer for first element for united linked list.*/
labelInfo * linkTwoLists(labelInfo *firstList, labelInfo *secondList) {
	labelInfo *start;
	if (firstList == NULL) {
		return secondList;
	}
	for (start = firstList; start->next; start = start->next);
	start->next = secondList;
	return firstList;
}
/*Deleeting all linked list of tables elements.*/
void deleteList(labelInfo *head) {
	labelInfo *deletedElement;
	for (deletedElement = head; head;) {
		head = head->next;
		free(deletedElement->name);
		deletedElement->name = NULL;
		free(deletedElement);
		deletedElement = NULL;
		deletedElement = head;
	}
}


/*searching sor information in label with this name table when needed, return pointer forneeded element
or NULL if label doesn't exist it return NULL.*/
labelInfo * findNeededElement(labelInfo *element, char *labelName) {
	labelInfo *anchor;

	for (anchor = element; anchor; anchor = anchor->next) {/*Scanning all elements in labe table*/
		if (!strcmp(anchor->name, labelName)) {

			return anchor;
		}
	}
	return NULL;
}
/*Function for updating the label if needed for example chage status of label from Absolute ro Relocatable.*/
void makeEntryChanges(labelInfo *element, char *line) {
	char *delim = "\n\t ";
	char *token = strtok(line, delim);
	labelInfo *anchor;
	if (token == NULL) {
		return;
	}
	if (isLabelDef(token)) {/*Mean that this is a label.*/
		token = strtok(NULL, delim);
		if (token == NULL) {
			return;
		}
	}
	if (!strcmp(token, ".entry")) {
		token = strtok(NULL, delim);
		if (token == NULL) {
			return;
		}
		anchor = findNeededElement(element, token);/*Updading the label from beeing Absolute to beeing Relocatable.*/
		if (anchor != NULL) {
			anchor->coddingType = 'R';
		}
	}

}
/*return true if label which was declared exist like ".entry Start".*/
int labelDeclaredExist(labelInfo *element,char *labelName) {
	labelInfo *anchor;
	char theLabelName[MAX_LINE_SIZE];
	strcpy(theLabelName, labelName);
	REAL_LABLE(theLabelName);   
	for (anchor = element; anchor; anchor = anchor->next) {
		if (!strcmp(anchor->name, theLabelName)) {
			if ( anchor->type == 'R') {
				return 1;
			}
			else {
				return 0;
			}
		}
	}
	return 0;
}
/*return true if label which was defined exist like "LOOP: inc r1*/
int labelDefinedExist(labelInfo *element, char *labelName) {
	labelInfo *anchor;
	char theLabelName[80];
	if (element == NULL) {
		return 0;
	}
	strcpy(theLabelName,labelName);
	REAL_LABLE(theLabelName); 
	for (anchor = element; anchor; anchor = anchor->next) {
		if (!strcmp(anchor->name, theLabelName)) {
			if (anchor->type == 'A' ) {
				return 1;
			}
			else {
				return 0;
			}
		}
	}
	return 0;
}
/*return true if label which was defined exist like "LOOP: inc r1*/
int isLabelDef(char *labelName) {
	if (strlen(labelName) >= 2 && labelName[strlen(labelName) - 1] == ':') {
		return 1;
	}
	return 0;
}