; This file is intended to check the bad-path of the assembler.

; Each line (except a few) contains an error in the assembly language code.
; A comment preceding each line explains the error.

; All errors should be detectable in the first pass of the assembler.

; Run the assembler on this file, and verify that it catches all the errors.
; Your assembler messages need not be identical to the comments in this file.

; Disclaimer: this list of errors is not exhaustive; 
;             you are encouraged to identify additional errors.

; 1. this line is ok (DATA4 is declared later)
Start:  dec DATA4

; 2. missing operand
        sub #5

; 3. missing operand
        red

; 4. this line is ok
       .extern DATA4

; 5. invalid target operand (immediate)
        add  #5,#6

; 6. this line is ok (immediate target operand allowed for cmp)
Next:   cmp  #5,#6

; 7. invalid operand (immediate)
        inc  #50

; 8. undefined instruction
        and  r1,r2

; 9. undefined instruction (case sensitivity)
        jSr  Start

; 10. this line is ok (r9 is a label declared later)
        add  r1,r9

; 11. invalid characters (,r2)
        cmp  r1,,r2

; 12. invalid characters (,r3)
        add  #5,r1,r3

; 13. invalid characters (blabla)
        prn r1 blabla

; 14. invalid characters (8])
        cmp r1,r3[r8]

; 15. invalid characters (r5])
        prn Next[r5]

; 16. invalid characters ([r1])
        cmp #3,r2 [r1]

; 17. invalid characters (123)
        inc  123

; 18. invalid characters (xyz)
        cmp  #xyz,r1

; 19. label previously declared
Next:   clr  r2

; 20. label does not start in first column (the label declaration is ignored)
     label1:   sub r1,r7

; 21. invalid source operand (register)   
        lea  r3,r1

; 22. invalid source operand (immediate)   
        lea  #3,r5

; 23. invalid characters (Start)
        stop Start

; 24. invalid characters (4000) 
        .data   200 4000

; 25. this line is ok (arbitrary whitespace between operands)
DATA1:  .data   3000,   4000 ,  5000, 6000

; 26. invalid characters (,3)
        .data   1, ,3

; 27. invalid character (,)
        .data   4,

; 28. invalid characters (#123)
        .data   #123

; 29. invalid characters (.4)
        .data   12.4

; 30. invalid characters (-5)
        .data   --5

; 31. this line is ok (case sensitive labels)
Data1:  .data   100, +200, -300

; 32. invalid label (cannot be an instruction)
mov:    .data   5

; 33. invalid label (cannot be a register)
r1:     .data   200,300

; 34. label previously declared
DATA1:  .data   300

; 35. invalid label(non-alphabetic first character)
1DATA:  .data   300

; 36. this line is ok (r9 is not the name of a register)
r9:     .data   200

; 37. this line is ok (label declaration X is ignored - warning to be issued)
X:      .entry  DATA1

; 38. this line is ok (it is ok to declare the same external more than once)
        .extern DATA4

; 39. local label cannot be declared as external
        .extern Start    
		
; 40. this line is ok (label declaration Y is ignored - warning to be issued)
Y:      .extern DATA8

; 41. this line is ok (STR1 is declared later)
        .entry STR1

; 42. label is already designated as external
        .entry  DATA4

; 43. undefined instruction (note: DATA2 is not a label declaration)
DATA2   .data   4

; 44. undefined directive (case sensitivity)
        .DATA   5

; 45. This line is ok (it is ok to designate the same entry more than once)
        .entry  STR1

; 46. invalid characters (blabla is not a string)
        .string blabla

; 47. invalid characters (blabla)
        .string "abcdefg" blabla

; 48. this line is ok (comma within string is not a separator)
STR1:   .string "abc, ,defg"

; 49. invalid label (too long)
SuperCalifragilisticExpiAlidocious: .data	4	
          
; 50. missing operands in directive
        .data

; 51. missing operand in directive
        .entry

; 52. undefined directive
        .invalid 85,90

; 53. this line is ok
        stop