/* Fall 2021 COP 3402 HW1
   Anjaly Davis, Jennifer Nguyen
   September 15th, 2021 */

#include <stdio.h>
#include <stdlib.h>
#include "compiler.h"

// Global Variables
#define MAX_PAS_LENGTH 500

void print_execution(int line, char *opname, struct instruction IR, int PC, int BP, int SP, int DP, int *pas, int GP)
{
    int i;
    // print out instruction and registers
    printf("%d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t", line, opname, IR.l, IR.m, PC, BP, SP, DP);
    
    // print data section
    for (i = GP; i <= DP; i++)
        printf("%d ", pas[i]);
    
    printf("\n");
    
    // print stack
    printf("\tstack : ");
    for (i = MAX_PAS_LENGTH - 1; i >= SP; i--)
        printf("%d ", pas[i]);
    
    printf("\n");
}

int base(int L, int *pas, int BP)
{
    int ctr = L;
    int rtn = BP;
    while (ctr > 0)
    {
        rtn = pas[rtn-1];
        ctr--;
    }
    return rtn;
}

void execute_program(instruction *code, int printFlag)
{
    // variables
    int *pas = calloc(MAX_PAS_LENGTH, sizeof(int));
    int IC, line, halt;
    struct instruction IR;
    // read in program
    IC = 0;
    line = 0;
    halt = 1;
    while (code[line].opcode != -1)
    {
        pas[IC] = code[line].opcode;
        IC++;
        pas[IC] = code[line].l;
        IC++;
        pas[IC] = code[line].m;
        IC++;
        line++;
    }
    
    // Initial Values
    int GP = IC;        // Global Data Pointer
    int DP = IC - 1;    // Data Pointer
    int BP = IC;        // Base Pointer
    int PC = 0;         // Program counter
    int SP = MAX_PAS_LENGTH;        // Stack Pointer
    
    if (printFlag == 1)
    {
        // Print headers
        printf("\t\tPC\tBP\tSP\tDP\tdata\n");
        printf("Initial Values: %d\t%d\t%d\t%d\n", PC, BP, SP, DP);
    }
    
    while (halt == 1)
    {
        // Fetch Cycle
        IR = code[PC];
        int line = PC;    // track line number
        PC++;

        // Execute Cycle
        switch(IR.opcode)
        {
            //LIT
            case 1:
                if(BP == GP)
                {
                    DP = DP+1;
                    pas[DP] = IR.m;
                }
                else
                {
                    SP = SP-1;
                    pas[SP] = IR.m;
                }
                if (printFlag == 1)
                    print_execution(line, "LIT", IR, PC, BP, SP, DP, pas, GP);
                break;
                
            // OPR
            case 2:
                switch(IR.m)
                {
                    //RTN
                    case 0:
                        SP = BP + 1;
                        PC = pas[SP-4];
                        BP = pas[SP-3];
                        if (printFlag == 1)
                            print_execution(line, "RTN", IR, PC, BP, SP, DP, pas, GP);
                        break;

                    //NEG
                    case 1:
                        if(BP == GP)
                        {
                            pas[DP] = -1*pas[DP];
                        }
                        else
                        {
                            pas[SP] = -1*pas[SP];
                        }
                        if (printFlag == 1)
                            print_execution(line, "NEG", IR, PC, BP, SP, DP, pas, GP);
                        break;

                    //ADD
                    case 2:
                        if(BP == GP)
                        {
                            DP = DP-1;
                            pas[DP] = pas[DP] + pas[DP+1];
                        }
                        else
                        {
                            SP = SP+1;
                            pas[SP] = pas[SP] + pas[SP-1];
                        }
                        if (printFlag == 1)
                            print_execution(line, "ADD", IR, PC, BP, SP, DP, pas, GP);
                        break;

                    //SUB
                    case 3:
                        if(BP == GP)
                        {
                            DP = DP-1;
                            pas[DP] = pas[DP] - pas[DP+1];
                        }
                        else
                        {
                            SP = SP+1;
                            pas[SP] = pas[SP] - pas[SP-1];
                        }
                        if (printFlag == 1)
                            print_execution(line, "SUB", IR, PC, BP, SP, DP, pas, GP);
                        break;

                    //MUL
                    case 4:
                        if(BP == GP)
                        {
                            DP = DP-1;
                            pas[DP] = pas[DP] * pas[DP+1];
                        }
                        else
                        {
                            SP = SP+1;
                            pas[SP] = pas[SP] * pas[SP-1];
                        }
                        if (printFlag == 1)
                            print_execution(line, "MUL", IR, PC, BP, SP, DP, pas, GP);
                        break;

                    //DIV
                    case 5:
                        if(BP == GP)
                        {
                            DP = DP-1;
                            pas[DP] = pas[DP] / pas[DP+1];
                        }
                        else
                        {
                            SP = SP+1;
                            pas[SP] = pas[SP] / pas[SP-1];
                        }
                        if (printFlag == 1)
                            print_execution(line, "DIV", IR, PC, BP, SP, DP, pas, GP);
                        break;

                    //ODD
                    case 6:
                        if(BP == GP)
                        {
                            pas[DP] = pas[DP] % 2;
                        }
                        else
                        {
                            pas[SP] = pas[SP] % 2;
                        }
                        if (printFlag == 1)
                            print_execution(line, "ODD", IR, PC, BP, SP, DP, pas, GP);
                        break;

                    //MOD
                    case 7:
                        if(BP == GP)
                        {
                            DP = DP-1;
                            pas[DP] = pas[DP] % pas[DP+1];
                        }
                        else
                        {
                            SP = SP+1;
                            pas[SP] = pas[SP] % pas[SP-1];
                        }
                        if (printFlag == 1)
                            print_execution(line, "MOD", IR, PC, BP, SP, DP, pas, GP);
                        break;

                    //EQL
                    case 8:
                        if(BP == GP)
                        {
                            DP = DP-1;
                            pas[DP] = pas[DP] == pas[DP+1];
                        }
                        else
                        {
                            SP = SP+1;
                            pas[SP] = pas[SP] == pas[SP-1];
                        }
                        if (printFlag == 1)
                            print_execution(line, "EQL", IR, PC, BP, SP, DP, pas, GP);
                        break;

                    //NEQ
                    case 9:
                        if(BP == GP)
                        {
                            DP = DP-1;
                            pas[DP] = pas[DP] != pas[DP+1];
                        }
                        else
                        {
                            SP = SP+1;
                            pas[SP] = pas[SP] != pas[SP-1];
                        }
                        if (printFlag == 1)
                            print_execution(line, "NEQ", IR, PC, BP, SP, DP, pas, GP);
                        break;

                    //LSS
                    case 10:
                        if(BP == GP)
                        {
                            DP = DP-1;
                            pas[DP] = pas[DP] < pas[DP+1];
                        }
                        else
                        {
                            SP = SP+1;
                            pas[SP] = pas[SP] < pas[SP-1];
                        }
                        if (printFlag == 1)
                            print_execution(line, "LSS", IR, PC, BP, SP, DP, pas, GP);
                        break;

                    //LEQ
                    case 11:
                        if(BP == GP)
                        {
                            DP = DP-1;
                            pas[DP] = pas[DP] <= pas[DP+1];
                        }
                        else
                        {
                            SP = SP+1;
                            pas[SP] = pas[SP] <= pas[SP-1];
                        }
                        if (printFlag == 1)
                            print_execution(line, "LEQ", IR, PC, BP, SP, DP, pas, GP);
                        break;

                    //GTR
                    case 12:
                        if(BP == GP)
                        {
                            DP = DP - 1;
                            pas[DP] = pas[DP] > pas[DP+1];
                        }
                        else
                        {
                            SP = SP+1;
                            pas[SP] = pas[SP] > pas[SP-1];
                        }
                        if (printFlag == 1)
                            print_execution(line, "GTR", IR, PC, BP, SP, DP, pas, GP);
                        break;

                    //GEQ
                    case 13:
                        if(BP == GP)
                        {
                            DP = DP-1;
                            pas[DP] = pas[DP] >= pas[DP+1];
                        }
                        else
                        {
                            SP = SP+1;
                            pas[SP] = pas[SP] >= pas[SP-1];
                        }
                        if (printFlag == 1)
                            print_execution(line, "GEQ", IR, PC, BP, SP, DP, pas, GP);
                        break;
                }
                break;
                
            //LOD
            case 3:
                if(BP == GP)
                {
                    DP = DP+1;
                    pas[DP] = pas[GP+IR.m];
                }
                else if((base(IR.l, pas, BP)) == GP)
                {
                    SP = SP-1;
                    pas[SP] = pas[GP+IR.m];
                }
                else
                {
                    SP = SP-1;
                    pas[SP] = pas[(base(IR.l, pas, BP))-IR.m];
                }
                if (printFlag == 1)
                    print_execution(line, "LOD", IR, PC, BP, SP, DP, pas, GP);
                break;

            //STO
            case 4:
                if(BP == GP)
                {
                    pas[GP+IR.m] = pas[DP];
                    DP = DP-1;
                }
                else if((base(IR.l, pas, BP)) == GP)
                {
                    pas[GP+IR.m] = pas[SP];
                    SP = SP+1;
                }
                else
                {
                    pas[(base(IR.l, pas, BP))-IR.m] = pas[SP];
                    SP = SP+1;
                }
                if (printFlag == 1)
                    print_execution(line, "STO", IR, PC, BP, SP, DP, pas, GP);
                break;

            //CAL
            case 5:
                pas[SP-1] = 0;          // Functional Value
                pas[SP-2] = base(IR.l, pas, BP);           // Static Link
                pas[SP-3] = BP;         // Dynamic Link
                pas[SP-4] = PC;         // Return Address
                BP = SP-1;
                PC = IR.m;
                if (printFlag == 1)
                    print_execution(line, "CAL", IR, PC, BP, SP, DP, pas, GP);
                break;

            //INC
            case 6:
                if(BP == GP)
                {
                    DP += IR.m;
                }
                else
                {
                    SP -= IR.m;
                }
                if (printFlag == 1)
                    print_execution(line, "INC", IR, PC, BP, SP, DP, pas, GP);
                break;

            //JMP
            case 7:
                PC = IR.m;
                if (printFlag == 1)
                    print_execution(line, "JMP", IR, PC, BP, SP, DP, pas, GP);
                break;

            //JPC
            case 8:
                if(BP == GP)
                {
                    if(pas[DP] == 0)
                    {
                        PC = IR.m;
                    }
                    DP = DP-1;
                }
                else
                {
                    if(pas[SP] == 0)
                    {
                        PC = IR.m;
                    }
                    SP = SP+1;
                }
                if (printFlag == 1)
                    print_execution(line, "JPC", IR, PC, BP, SP, DP, pas, GP);
                break;

          // SYS
          case 9:
              switch(IR.m)
              {
                  case 1:
                      printf("Top of Stack Value: ");
                     if (BP == GP)
                     {
                         printf("%d\n", pas[DP]);
                         DP = DP -1;
                     }
                     else
                     {
                          printf("%d", pas[SP]);
                          SP = SP + 1;
                     }
                    break;
                    
                  case 2:
                    printf("Please Enter an Integer: ");
                     if (BP == GP)
                     {
                          DP = DP + 1;
                          scanf("%d", &pas[DP]);
                     }
                     else
                     {
                          SP = SP - 1;
                          scanf("%d", &pas[SP]);
                     }
                    break;
                      
                  case 3:
                      halt = 0;
                      break;
             }
            if (printFlag == 1)
                print_execution(line, "SYS", IR, PC, BP, SP, DP, pas, GP);
            break;
             
         default:
                printf("err\t");
                return;
         
        }
    }
    free(pas);
}
