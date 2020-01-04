# C-Project-Assembler

Name: Teodor Linnik



Name: Teodor Linnik



The project gets from the command line from Linux the names of input files (without ".as" ending) needed to check and make for
the output files in case of correctness.
for example in the command line: 
./Assembler myprog ps badpath-datasize badpath-immedsize badpath-pass1 badpath-pass2

Acording to this input the checked files will be: myprog.as ps.as badpath-datasize.as 
badpath-immedsize.as badpath-pass1.as badpath-pass2.as, the program is case sensetive for input files,
for example "mov" is not like "MOV".

This project consists of 3 parts in main.c file.

The first part is for building a label table for information about the labels for output.
The label table is done by the linked list which pointer points to the begging for the label 
(the first element in the linked list). The linked list construction is:
typedef struct labelInfo {
	char *name; /*Label name*/
	int address; /*Address of this label*/
	char type;   /*The type: 'C' Coding label, 'D' data label, 'U' Unknown label for external label.*/
	char coddingType; /*The codding type: 'A' Another label like "MER: mov r1,r2" that is not entry and notextern, 'E' External label, 'R' entRy label .*/
	struct labelInfo *next;
}labelInfo;

First scanning for every line was done only for lines for code. and then the second scanning for only data
lines and according to this the label table is built. The third scanned done for updating the coddingType
for labels: 'A' Absolute label\'E' External label\'R' Relocatable label.
The files for the first part: buildlabeltable.c and buildlabeltable.h .

The second step is done for checking correctness. For this every input file scan twice, firstly every line for checking
correctness and in a second time for building new label table for verifying the correctness for appearance twice (or more)
for some labels.
The files for the second part: correctnesschecking.c and correctnesschecking.h .


The third and final step is making output file (in case of full correctness from previous step).
The output files for each exampleFile.as are exampleFile.ob, if needed also: exampleFile.ent exampleFile.ext
This is also was done like first step, firstly scanning for output the lines of code and then lines for
data for every input file.
The files for third part: BuildingOutputand BuildingOutput.

This project also contain ConstructsAndDefinitions.h for Struct for label table and needed definitions for macro.

The third and final step is making the output file (in case of full correctness from the previous step).
The output files for each exampleFile.as are exampleFile.ob, if needed also: exampleFile.ent exampleFile.ext
This is also was done like the first step, firstly scanning for output the lines of code and then lines for
data for every input file.
The files for the third part: BuildingOutputand BuildingOutput.

This project also contains ConstructsAndDefinitions.h for Struct for the label table and needed definitions for macro.
