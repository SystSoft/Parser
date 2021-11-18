/* Fall 2021 COP 3402 HW1
   Anjaly Davis, Jennifer Nguyen
   November 2nd, 2021 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

#define MAX_CODE_LENGTH 1000
#define MAX_SYMBOL_COUNT 100

instruction *code;
int cIndex;
symbol *table;
int tIndex;
int curlevel;
int tok_index;

void emit(int opname, int level, int mvalue);
void addToSymbolTable(int k, char n[], int v, int l, int a, int m);
void printparseerror(int err_code);
void printsymboltable();
void printassemblycode();
int MULTIPLEDECLARATIONCHECK(lexeme token);
int findsymbol(lexeme token, int k);
void mark();
void program(lexeme *list);
void block(lexeme *list);
void const_declaration(lexeme *list);
int var_declaration(lexeme *list);
void procedure_dec(lexeme *list);
void statement(lexeme *list);
void condition(lexeme *list);
void expression(lexeme *list);
void term(lexeme *list);
void factor(lexeme *list);

instruction *parse(lexeme *list, int printTable, int printCode)
{
    code = malloc(MAX_CODE_LENGTH * sizeof(struct instruction));
    table = malloc(MAX_SYMBOL_COUNT * sizeof(struct symbol));
    cIndex = 0;
    tIndex = 0;
    
    program(list);
    
    if (printTable == 1)
        printsymboltable();
       
    if (printCode == 1)
        printassemblycode();
    
    code[cIndex].opcode = -1;
    
    return code;
}

void emit(int opname, int level, int mvalue)
{
    code[cIndex].opcode = opname;
    code[cIndex].l = level;
    code[cIndex].m = mvalue;
    cIndex++;
}

void addToSymbolTable(int k, char n[], int v, int l, int a, int m)
{
    table[tIndex].kind = k;
    strcpy(table[tIndex].name, n);
    table[tIndex].val = v;
    table[tIndex].level = l;
    table[tIndex].addr = a;
    table[tIndex].mark = m;
    tIndex++;
}

int MULTIPLEDECLARATIONCHECK(lexeme token)
{
    int i;
    
    // linear search symbol table for the token
    for (i = 0; i < tIndex; i++)
    {
        // if symbol table has the name given
        if (strcmp(table[i].name, token.name) == 0)
        {
            // checks to see if it’s unmarked
            if (table[i].mark == 0 && table[i].level == curlevel)
            {
                    return i;
            }
        }
            
    }
    // if not found return -1
    return -1;
}

int findsymbol(lexeme token, int k)
{
    int i, maxIndex = -1, maxlevel = -1;
    
    // linear search symbol table for the token
    for (i = 0; i < tIndex; i++)
    {
        // correct name AND kind value AND is unmarked.
        if (strcmp(table[i].name, token.name) == 0 && table[i].kind == k && table[i].mark == 0)
        {
            if(table[i].level >= maxlevel)
            {
                maxlevel = table[i].level;
                maxIndex = i;
            }
        }
    }
    return maxIndex;
}

void mark()
{
    // linear search symbol table for the token backwards
    int i;;
    
    // stop when it finds an unmarked entry whose level is less than the current level
    for (i = tIndex; i >= 0; i--)
    {
        // ignores marked entries
        if (table[i].mark == 1)
            continue;
        
        // if entry’s level is equal to the current level, it marks that entry
        if (table[i].level == curlevel)
            table[i].mark = 1;
        
        if (table[i].mark == 0 && table[i].level < curlevel)
            break;
    }
    
}

void program(lexeme *list)
{
    int i;
    emit(7, 0, 0);            // JMP
    addToSymbolTable(3, "main", 0, 0, 0, 0); // add to symbol table
    curlevel = -1;
    
    block(list);
    
    if (list[tok_index].type != periodsym)
        printparseerror(1);
        
    emit(9, 0, 3);        // HALT
        
    for (i = 0; i < cIndex; i++)
    {
        if (code[i].opcode == 5)
            code[i].m = table[code[i].m].addr;
    }
    code[0].m = table[0].addr;
}

void block(lexeme *list)
{
    int x;
    
    curlevel++;
    int procedure_idx = tIndex - 1;
    
    const_declaration(list);
    
    x = var_declaration(list);
    
    procedure_dec(list);
    
    table[procedure_idx].addr = cIndex * 3;
    
    if (curlevel == 0)
        emit(6, curlevel, x);          // INC
    else
        emit(6, curlevel, x + 3);
    
    statement(list);
    mark();
    curlevel--;
}

void const_declaration(lexeme *list)
{
    char *identsave = NULL;
    if (list[tok_index].type == constsym)
    {
        do
        {
            tok_index++;
            if (list[tok_index].type != identsym)
                printparseerror(2);         // ident missing
               
            int symidx = MULTIPLEDECLARATIONCHECK(list[tok_index]);
            
            if (symidx != -1)
                printparseerror(18);
             
            identsave = list[tok_index].name;
            tok_index++;
                
            if (list[tok_index].type != assignsym)
                printparseerror(2);             // := missing

            tok_index++;
            
            if (list[tok_index].type != numbersym)
                printparseerror(2);         // number missing
            
            addToSymbolTable(1, identsave, atoi(list[tok_index].name), curlevel, 0, 0);
            tok_index++;
        
        }while (list[tok_index].type == commasym);
    
        if (list[tok_index].type != semicolonsym)
        {
            if (list[tok_index].type == identsym)
                printparseerror(13);
            else
                printparseerror(14);
        }
        tok_index++;
    }
}

int var_declaration(lexeme *list)
{
    int numVars = 0;
    if (list[tok_index].type == varsym)
    {
        do
        {
            numVars++;
            tok_index++;
            
            if (list[tok_index].type != identsym)
                printparseerror(3);     // ident missing
                                
            int symidx = MULTIPLEDECLARATIONCHECK(list[tok_index]);
            
            if (symidx != -1)
                printparseerror(18);
            
            if (curlevel == 0)
                addToSymbolTable(2, list[tok_index].name, 0, curlevel, numVars - 1, 0);
            else
                addToSymbolTable(2, list[tok_index].name, 0, curlevel, numVars+2, 0);
            
            tok_index++;
            
        }while (list[tok_index].type == commasym);
        
        if (list[tok_index].type != semicolonsym)
        {
            if (list[tok_index].type == identsym)
                printparseerror(13);
            else
                printparseerror(14);
        }
        tok_index++;
    }
    return numVars;
}

void procedure_dec(lexeme *list)
{
        while (list[tok_index].type == procsym)
        {
            tok_index++;
            if (list[tok_index].type != identsym)
                printparseerror(4);
                
            int symidx = MULTIPLEDECLARATIONCHECK(list[tok_index]);
            
            if (symidx != -1)
                printparseerror(18);
               
            addToSymbolTable(3, list[tok_index].name, 0, curlevel, 0, 0);
            tok_index++;
            if (list[tok_index].type != semicolonsym)
                printparseerror(4);        // must end in semicolon
            
            tok_index++;
            block(list);
            
            if (list[tok_index].type != semicolonsym)
                printparseerror(14);
            
            tok_index++;
            emit(2, curlevel, 0);           //RTN
        }
}

void statement(lexeme *list)
{
    if(list[tok_index].type==identsym)
    {
        int symldx = findsymbol(list[tok_index],2);

        if(symldx == -1)
        {
            if (findsymbol(list[tok_index],1) != findsymbol(list[tok_index],3))
                printparseerror(7);         // does not have kind 3
            else
                printparseerror(6);         // does not have kind 2
        }
        tok_index++;
        if(list[tok_index].type!=assignsym)
            printparseerror(5);         // missing :=
        
        tok_index++;
        expression(list);
        emit(4, curlevel-table[symldx].level, table[symldx].addr);          //STO
        return;
    }
    if(list[tok_index].type==dosym)
    {
        do
        {
            tok_index++;
            statement(list);
        }while(list[tok_index].type==semicolonsym);
        
        if(list[tok_index].type!=odsym)
        {
            if((list[tok_index].type==identsym)||(list[tok_index].type==whilesym)||(list[tok_index].type==readsym)||(list[tok_index].type==writesym)||(list[tok_index].type==dosym)||(list[tok_index].type==whensym)||(list[tok_index].type==callsym))
                printparseerror(15);
            else
                printparseerror(16);
        }
        tok_index++;
        return;
    }
    if(list[tok_index].type==whensym)
    {
        tok_index++;
        condition(list);
        int jpcIdx = cIndex;
        emit(8,curlevel,0); //JPC
        
        if(list[tok_index].type!=dosym)
            printparseerror(8);             // do must follow when
        
        tok_index++;
        statement(list);
        if(list[tok_index].type==elsedosym)
        {
            int jmpIdx = cIndex;
            emit(7,curlevel,jmpIdx); //JMP
            code[jpcIdx].m = cIndex*3;
            tok_index++;
            statement(list);
            code[jmpIdx].m = cIndex*3;
        }
        else
            code[jpcIdx].m = cIndex*3;
        return;
    }
    if(list[tok_index].type==whilesym)
    {
        tok_index++;
        int loopIdx = cIndex;
        condition(list);
        
        if(list[tok_index].type!=dosym)
            printparseerror(9);
            
        tok_index++;
        int jpcIdx = cIndex;
        emit(8,curlevel,0); //JPC
        statement(list);
        emit(7,curlevel,loopIdx*3);     //JMP
        code[jpcIdx].m = cIndex*3;
        return;
    }
    if(list[tok_index].type==readsym)
    {
        tok_index++;
        if(list[tok_index].type!=identsym)
            printparseerror(6);         // missing identifier
            
        int symIdx = findsymbol(list[tok_index],2);
        if(symIdx == -1)
        {
            if(findsymbol(list[tok_index],1)!=findsymbol(list[tok_index],3))
                printparseerror(6);         //missing kind 2
            else
                printparseerror(19);
        }
        tok_index++;
        emit(9, curlevel,2); //READ
        emit(4, curlevel-table[symIdx].level, table[symIdx].addr); //STO
        return;
    }
    if(list[tok_index].type==writesym)
    {
        tok_index++;
        expression(list);
        emit(9,curlevel,1); //WRITE
        return;
    }
    if(list[tok_index].type==callsym)
    {
        tok_index++;
        int symIdx = findsymbol(list[tok_index],3);
        if(symIdx == -1)
        {
            if(findsymbol(list[tok_index],1)!=findsymbol(list[tok_index],2))
                printparseerror(7);         //missing kind 3
            else
                printparseerror(19);
        }
        tok_index++;
        emit(5,curlevel-table[symIdx].level,symIdx); //CAL
    }
}
    
void condition(lexeme *list)
{
    if(list[tok_index].type == oddsym)
    {
        tok_index++;
        expression(list);
        emit(2, curlevel,6);        //ODD
    }
    else
    {
        expression(list);
        if(list[tok_index].type == eqlsym)
        {
            tok_index++;
            expression(list);
            emit(2, curlevel, 8); //EQL
        }
        else if(list[tok_index].type == neqsym)
        {
            tok_index++;
            expression(list);
            emit(2, curlevel, 9); //NEQ
        }
        else if(list[tok_index].type == lsssym)
        {
            tok_index++;
            expression(list);
            emit(2, curlevel, 10); //LSS
        }
        else if(list[tok_index].type == leqsym)
        {
            tok_index++;
            expression(list);
            emit(2, curlevel, 11); //LEQ
        }
        else if(list[tok_index].type == gtrsym)
        {
            tok_index++;
            expression(list);
            emit(2, curlevel, 12); //GTR
        }
        else if(list[tok_index].type == geqsym)
        {
            tok_index++;
            expression(list);
            emit(2, curlevel, 13); //GEQ
        }
        else
            printparseerror(10);
    }
}
    
void expression(lexeme *list)
{
    if(list[tok_index].type == subsym)
    {
        tok_index++;
        term(list);
        emit(2,curlevel,1); //NEG
        
        while((list[tok_index].type==addsym)||(list[tok_index].type==subsym))
        {
            if(list[tok_index].type == addsym)
            {
                tok_index++;
                term(list);
                emit(2,curlevel,2); //ADD
            }
            else
            {
                tok_index++;
                term(list);
                emit(2,curlevel,3); //SUB
            }
        }
    }
    else
    {
        if(list[tok_index].type==addsym)
            tok_index++;
        term(list);
                
        while((list[tok_index].type==addsym)||(list[tok_index].type==subsym))
        {
            if(list[tok_index].type==addsym)
            {
                tok_index++;
                term(list);
                emit(2,curlevel,2); //ADD
            }
            else
            {
                tok_index++;
                term(list);
                emit(2,curlevel,3); //SUB
            }
        }
    }
    if((list[tok_index].type==lparensym)||(list[tok_index].type==identsym)||(list[tok_index].type==numbersym)||(list[tok_index].type==oddsym)) // identifier number odd
        printparseerror(17);
        
}
    
void term(lexeme *list)
{
    factor(list);
    while((list[tok_index].type==multsym)||(list[tok_index].type==divsym)||(list[tok_index].type==modsym))
    {
        if(list[tok_index].type==multsym)
        {
            tok_index++;
            factor(list);
            emit(2,curlevel,4); //MUL
        }
        else if(list[tok_index].type==divsym)
        {
            tok_index++;
            factor(list);
            emit(2,curlevel,5); //DIV
        }
        else
        {
            tok_index++;
            factor(list);
            emit(2,curlevel,7); //MOD
        }
    }
}
    
void factor(lexeme *list)
{
    if(list[tok_index].type==identsym)
    {
        int symIdx_var = findsymbol(list[tok_index],2);
        int symIdx_const = findsymbol(list[tok_index],1);
        
        if((symIdx_var==-1)&&(symIdx_const==-1))
        {
            if(findsymbol(list[tok_index],3)!=-1)
                printparseerror(19);
            else
            {
                printparseerror(18);
            }
        }
        
        if(symIdx_var==-1)
            emit(1,curlevel,table[symIdx_const].val);      // LIT
        else if((symIdx_const=-1)|| (table[symIdx_var].level) > (table[symIdx_const].level))
            emit(3, curlevel-table[symIdx_var].level, table[symIdx_var].addr);      //LOD
        else
            emit(1,curlevel,table[symIdx_const].val);
        tok_index++;
    }
    else if(list[tok_index].type==numbersym)
    {
        emit(1,curlevel,0);
        tok_index++;
    }
    
    else if(list[tok_index].type==lparensym)
    {
        tok_index++;
        expression(list);
    
        if(list[tok_index].type!=rparensym)
            printparseerror(11);
        
        tok_index++;
    }
    else
        printparseerror(12);
}
        

void printparseerror(int err_code)
{
    switch (err_code)
    {
        case 1:
            printf("Parser Error: Program must be closed by a period\n");
            break;
        case 2:
            printf("Parser Error: Constant declarations should follow the pattern 'ident := number {, ident := number}'\n");
            break;
        case 3:
            printf("Parser Error: Variable declarations should follow the pattern 'ident {, ident}'\n");
            break;
        case 4:
            printf("Parser Error: Procedure declarations should follow the pattern 'ident ;'\n");
            break;
        case 5:
            printf("Parser Error: Variables must be assigned using :=\n");
            break;
        case 6:
            printf("Parser Error: Only variables may be assigned to or read\n");
            break;
        case 7:
            printf("Parser Error: call must be followed by a procedure identifier\n");
            break;
        case 8:
            printf("Parser Error: if must be followed by then\n");
            break;
        case 9:
            printf("Parser Error: while must be followed by do\n");
            break;
        case 10:
            printf("Parser Error: Relational operator missing from condition\n");
            break;
        case 11:
            printf("Parser Error: Arithmetic expressions may only contain arithmetic operators, numbers, parentheses, constants, and variables\n");
            break;
        case 12:
            printf("Parser Error: ( must be followed by )\n");
            break;
        case 13:
            printf("Parser Error: Multiple symbols in variable and constant declarations must be separated by commas\n");
            break;
        case 14:
            printf("Parser Error: Symbol declarations should close with a semicolon\n");
            break;
        case 15:
            printf("Parser Error: Statements within begin-end must be separated by a semicolon\n");
            break;
        case 16:
            printf("Parser Error: begin must be followed by end\n");
            break;
        case 17:
            printf("Parser Error: Bad arithmetic\n");
            break;
        case 18:
            printf("Parser Error: Confliciting symbol declarations\n");
            break;
        case 19:
            printf("Parser Error: Undeclared identifier\n");
            break;
        default:
            printf("Implementation Error: unrecognized error code\n");
            break;
    }
    free(code);
    free(table);
    code = NULL;
    exit(0);
}

void printsymboltable()
{
    int i;
    printf("Symbol Table:\n");
    printf("Kind | Name        | Value | Level | Address | Mark\n");
    printf("---------------------------------------------------\n");
    for (i = 0; i < tIndex; i++)
        printf("%4d | %11s | %5d | %5d | %5d | %5d\n", table[i].kind, table[i].name, table[i].val, table[i].level, table[i].addr, table[i].mark);
    
    free(table);
    table = NULL;
}

void printassemblycode()
{
    int i;
    printf("Line\tOP Code\tOP Name\tL\tM\n");
    for (i = 0; i < cIndex; i++)
    {
        printf("%d\t", i);
        printf("%d\t", code[i].opcode);
        switch (code[i].opcode)
        {
            case 1:
                printf("LIT\t");
                break;
            case 2:
                switch (code[i].m)
                {
                    case 0:
                        printf("RTN\t");
                        break;
                    case 1:
                        printf("NEG\t");
                        break;
                    case 2:
                        printf("ADD\t");
                        break;
                    case 3:
                        printf("SUB\t");
                        break;
                    case 4:
                        printf("MUL\t");
                        break;
                    case 5:
                        printf("DIV\t");
                        break;
                    case 6:
                        printf("ODD\t");
                        break;
                    case 7:
                        printf("MOD\t");
                        break;
                    case 8:
                        printf("EQL\t");
                        break;
                    case 9:
                        printf("NEQ\t");
                        break;
                    case 10:
                        printf("LSS\t");
                        break;
                    case 11:
                        printf("LEQ\t");
                        break;
                    case 12:
                        printf("GTR\t");
                        break;
                    case 13:
                        printf("GEQ\t");
                        break;
                    default:
                        printf("err\t");
                        break;
                }
                break;
            case 3:
                printf("LOD\t");
                break;
            case 4:
                printf("STO\t");
                break;
            case 5:
                printf("CAL\t");
                break;
            case 6:
                printf("INC\t");
                break;
            case 7:
                printf("JMP\t");
                break;
            case 8:
                printf("JPC\t");
                break;
            case 9:
                switch (code[i].m)
                {
                    case 1:
                        printf("WRT\t");
                        break;
                    case 2:
                        printf("RED\t");
                        break;
                    case 3:
                        printf("HAL\t");
                        break;
                    default:
                        printf("err\t");
                        break;
                }
                break;
            default:
                printf("err\t");
                break;
        }
        printf("%d\t%d\n", code[i].l, code[i].m);
    }
    if (table != NULL)
        free(table);
}
