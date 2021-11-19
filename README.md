/* Fall 2021 COP 3402 HW4
   Anjaly Davis, Jennifer Nguyen
   November 19th, 2021 */

In this assignment, we are implementing a PL/0 compiler as we did in HW3. In this case, we have made some changes to the VM(HW1) file and lex(HW2) file to implement the fully working PL/0 compiler. We have done some modifications to the grammer and to the scanner inorder to get the output. In our VM file, we modified the call and return instructions but in the case of lex file, we modified some functions. 

Compile: make (gcc driver.c lex.c parser.c vm.c -lm)

Run: ./a.out inputfile.txt -s -l -a -v
