/* Fall 2021 COP 3402 HW1
   Anjaly Davis, Jennifer Nguyen
   September 15th, 2021 */

#include <stdio.h>
#include <stdlib.h>
#include "compiler.h"

// Global Variables
#define MAX_PAS_LENGTH 500

void print_execution(int line, char *opname, struct instruction *IR, int PC, int BP, int SP, int DP, int *pas, int GP)
{
    int i;
    // print out instruction and registers
    printf("%d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t", line, opname, IR->l, IR->m, PC, BP, SP, DP);
    
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

int base(int l, int *pas, int BP)
{
    int arb = BP;   // arb = activation record base
    while ( l > 0)     //find base L levels down
    {
        arb = pas[arb];
        l--;
    }
    return arb;
}

int vm(int argc, char *argv[])
{
    FILE * ifp;
    ifp = fopen(argv[1], "r");
    
    int pas[MAX_PAS_LENGTH];
    int halt = 1;   // allows fetch and execution to run
    struct instruction *IR = malloc(sizeof(struct instruction));    // Instruction Register
    int IC = 0;     // Instruction Counter
    
    // All initial values for pas is 0
    int i;
    for (i = 0; i < MAX_PAS_LENGTH; i++)
    {
        pas[i] = 0;
    }
    
    // Scan in registers from input
    while (fscanf(ifp, "%d", &pas[IC]) != EOF)
    {
        IC++;
    }
    
    // Initial Values
    int GP = IC;        // Global Data Pointer
    int DP = IC - 1;    // Data Pointer
    int BP = IC;        // Base Pointer
    int PC = 0;         // Program counter
    int SP = MAX_PAS_LENGTH;        // Stack Pointer
    
    // Print headers
    printf("\t\tPC\tBP\tSP\tDP\tdata\n");
    printf("Initial Values: %d\t%d\t%d\t%d\n", PC, BP, SP, DP);
    
    while (halt == 1)
    {
        // Fetch Cycle
        IR->opcode = pas[PC];
        IR->l = pas[PC + 1];
        IR->m = pas[PC + 2];
        PC += 3;
        int line = (PC/3)-1;    // track line number

        // Execute Cycle
        switch(IR->opcode)
        {
            //LIT
            case 1:
                if(BP == GP)
                {
                    DP = DP+1;
                    pas[DP] = IR->m;
                }
                else
                {
                    SP = SP-1;
                    pas[SP] = IR->m;
                }
                print_execution(line, "LIT", IR, PC, BP, SP, DP, pas, GP);
                break;
                
            // OPR
            case 2:
                switch(IR->m)
                {
                    //RTN
                    case 0:
                        SP = BP + 1;
                        BP = pas[SP-3];
                        PC = pas[SP-4];
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
                        print_execution(line, "GEQ", IR, PC, BP, SP, DP, pas, GP);
                        break;
                }
                break;
                
            //LOD
            case 3:
                if(BP == GP)
                {
                    DP = DP+1;
                    pas[DP] = pas[GP+IR->m];
                }
                else if((base(IR->l, pas, BP)) == GP)
                {
                    SP = SP-1;
                    pas[SP] = pas[GP+IR->m];
                }
                else
                {
                    SP = SP-1;
                    pas[SP] = pas[(base(IR->l, pas, BP))-IR->m];
                }
                print_execution(line, "LOD", IR, PC, BP, SP, DP, pas, GP);
                break;

            //STO
            case 4:
                if(BP == GP)
                {
                    pas[GP+IR->m] = pas[DP];
                    DP = DP-1;
                }
                else if((base(IR->l, pas, BP)) == GP)
                {
                    pas[GP+IR->m] = pas[SP];
                    SP = SP+1;
                }
                else
                {
                    pas[(base(IR->l, pas, BP))-IR->m] = pas[SP];
                    SP = SP+1;
                }
                print_execution(line, "STO", IR, PC, BP, SP, DP, pas, GP);
                break;

            //CAL
            case 5:
                pas[SP-1] = 0;          // Functional Value
                pas[SP-2] = base(IR->l, pas, BP);           // Static Link
                pas[SP-3] = BP;         // Dynamic Link
                pas[SP-4] = PC;         // Return Address
                BP = SP-1;
                PC = IR->m;
                print_execution(line, "CAL", IR, PC, BP, SP, DP, pas, GP);
                break;

            //INC
            case 6:
                if(BP == GP)
                {
                    DP += IR->m;
                }
                else
                {
                    SP -= IR->m;
                }
                print_execution(line, "INC", IR, PC, BP, SP, DP, pas, GP);
                break;

            //JMP
            case 7:
                PC = IR->m;
                print_execution(line, "JMP", IR, PC, BP, SP, DP, pas, GP);
                break;

            //JPC
            case 8:
                if(BP == GP)
                {
                    if(pas[DP] == 0)
                    {
                        PC = IR->m;
                    }
                    DP = DP-1;
                }
                else
                {
                    if(pas[SP] == 0)
                    {
                        PC = IR->m;
                    }
                    SP = SP+1;
                }
                print_execution(line, "JPC", IR, PC, BP, SP, DP, pas, GP);
                break;

          // SYS
          case 9:
              switch(IR->m)
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
            print_execution(line, "SYS", IR, PC, BP, SP, DP, pas, GP);
             break;
             
         default: printf("err\t");
         
        }
    }
    fclose(ifp);
    free(IR);
    return 0;
}
