; This is a test input file written by the student.
; Write your own assembly language source program.
; The program should contain at least 10 insructions, 5 labels, 5 directives
; No errors: the assembler should generate successfully all output files:
;     myprog.ob, myprog.ext, myprog.ent
.entry JOIN
.extern M4
.extern SEE
.entry SV5
MAIN: mov r4, r5[r6]
cmp #-4,#+34
inc r4
WE: cmp r4, M4
X: jsr SEE
JOIN: lea M4, r7
rts
sub M4, JOIN
dec JOIN
prn #-65
MAYAMI: .entry JACK
clr r1
add #7 ,r4
DEAL: .string "This is me"
JACK: bne r2
SELF: red r3[r2]
not r5
stop
SV5: .data 45, -7,80,111,54
.data 6,5,3
Y: .string "ytds"
.string "nnht"