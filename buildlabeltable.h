#ifndef BUILDLABELTABLE_H
#define BUILDLABELTABLE_H

#include "ConstructsAndDefinitions.h"


/*This function getting the line from file, pointer for the last element from current code label table, pointer for current running address
this function return the pointer for the updated code table for last element and updated running address.*/
labelInfo * lineCheckCode(char *line, labelInfo *head, int *address);
/*This function getting the line from file, pointer for the last element from current data label table, pointer for current running address
this function return the pointer for the updated data table for last element and updated running address.*/
labelInfo * lineCheckData(char *line, labelInfo *head, int *adres);

/*This function hetting the pointer for first element of linked list and returning the reversed list with pointer for
last element for linked list.*/
labelInfo * reverseLinkedList(labelInfo *head);
/*This function get 2 linked lists, linking them and return the pointer for first element for united linked list.*/
labelInfo * linkTwoLists(labelInfo *firstList, labelInfo *secondList);

/*This function getting the number of defined opcode, the line, function return how many steps memory needed to proced for this command.*/
int commandAnilizer(int commandNum, char *token);

/*This functionthe line, function return how many step smemory needed to proced for this data.*/
int dataAnalizer(char *token);

/*In case that 2 operands are addressing num 3 like " mov r1,r2" it return 0, else this return 1 needed for defining if needed
to proceed one more steps for for code counter.*/
int operandAnalizer(char *token);

/*Deleeting all linked list of tables elements.*/
void deleteList(labelInfo *head);
void printElement(labelInfo *element);

/*Creating element*/
labelInfo *createElement();

/*For updating information  in table for label element.*/
void gettingInfoToElement(labelInfo *element, char *aName, int aAdress, char aType, char aCoddingType);

/*Adding new person for label table  return the pointer for last added element for new linked list*/
labelInfo * addNewPerson(labelInfo *person, labelInfo *list);
void dispPerson(labelInfo *element);

/*searching sor information in label with this name table when needed, return pointer forneeded element
or NULL if label doesn't exist it return NULL.*/
labelInfo * findNeededElement(labelInfo *element, char *labelName);

/*Function for updating the label if needed forexample chage status of label from Absolute ro Relocatable.*/
void makeEntryChanges(labelInfo *element, char *line);
/*return true if label which was declared exist like ".entry Start".*/
int labelDeclaredExist(labelInfo *element, char *labelName);
/*return true if label which was defined exist like "LOOP: inc r1*/
int labelDefinedExist(labelInfo *element, char *labelName);
/*return true if label which was defined exist like "LOOP: inc r1*/
int isLabelDef(char *labelName);

#endif /* BUILDLABELTABLE_H*/
