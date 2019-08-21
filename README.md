# C-Project-Assembler

Name: Teodor Linnik
ID: 312728017



The project get from command line from linux the names of input files (without ".as" ending) needed to check and make for
them output files in case of correctness.
for example in command line: 
./Assembler myprog ps badpath-datasize badpath-immedsize badpath-pass1 badpath-pass2

Acording to this input the checked files will be: myprog.as ps.as badpath-datasize.as 
badpath-immedsize.as badpath-pass1.as badpath-pass2.as, the program is case sensetive for input files,
for example "mov" is not like "MOV".

This project consist from 3 parts in main.c file.

First part: is for building label table for information abouts the labels for output.
Label table is done by the linked list which pointer point to the begging for the lable 
(first element in linked list). The linked list construction is:
typedef struct labelInfo {
	char *name; /*Label name*/
	int address; /*Address of this label*/
	char type;   /*The type: 'C' Coding label, 'D' data label, 'U' Unknown label for external label.*/
	char coddingType; /*The codding type: 'A' Another label like "MER: mov r1,r2" that is not entry and notextern, 'E' External label, 'R' entRy label .*/
	struct labelInfo *next;
}labelInfo;

First scanning for every line was done only for lines for code. and then the second scanning for only data
lines and acording to this the label table are build. Third scanond done for updating the coddingType
for labels: 'A' Absolute label\'E' External label\'R' Relocatable label.
The files for first part: buildlabeltable.c and buildlabeltable.h .

Second steps is done for checking correcntess. For this every input file scan twice, firstly every line for checking
correcntess and in second time for building new label table for verifying the correctness for appearance twice (or more)
for some labels.
The files for second part: correctnesschecking.c and correctnesschecking.h .


The third and final step is making output file (in case of full correctness from previous step).
The output files for each exampleFile.as are exampleFile.ob, if needed also: exampleFile.ent exampleFile.ext
This is also was done like first step, firstly scanning for output the lines of code and then lines for
data for every input file.
The files for third part: BuildingOutputand BuildingOutput.

This project also contain ConstructsAndDefinitions.h for Struct for label table and needed definitions for macro.
