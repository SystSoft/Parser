/*
    Written by: Anjaly Davis, Jennifer Nguyen
     Fall 2021 COP 3402
     October 1, 2021
     
    This is the lex.c file for the UCF Fall 2021 Systems Software Project.
    For HW2, you must implement the function lexeme *lexanalyzer(char *input).
    You may add as many constants, global variables, and support functions
    as you desire.
    
    If you choose to alter the printing functions or delete list or lex_index,
    you MUST make a note of that in you readme file, otherwise you will lose
    5 points.
*/


#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"
#define MAX_NUMBER_TOKENS 500
#define MAX_IDENT_LEN 11
#define MAX_NUMBER_LEN 5

lexeme *list;
int lex_index;

void printlexerror(int type);
void printtokens();

// compares letter
void compareReserve(lexeme *list)
{
    if (strcmp(list[lex_index].name, "const") == 0)
    {
        list[lex_index].type = constsym;
    }
    else if (strcmp(list[lex_index].name, "var") == 0)
    {
        list[lex_index].type = varsym;
    }
    else if (strcmp(list[lex_index].name, "procedure") == 0)
    {
        list[lex_index].type = procsym;
    }
    else if (strcmp(list[lex_index].name, "begin") == 0)
    {
        list[lex_index].type = beginsym;
    }
    else if (strcmp(list[lex_index].name, "end") == 0)
    {
        list[lex_index].type = endsym;
    }
    else if (strcmp(list[lex_index].name, "while") == 0)
    {
        list[lex_index].type = whilesym;
    }
    else if (strcmp(list[lex_index].name, "do") == 0)
    {
        list[lex_index].type = dosym;
    }
    else if (strcmp(list[lex_index].name, "if") == 0)
    {
        list[lex_index].type = ifsym;
    }
    else if (strcmp(list[lex_index].name, "then") == 0)
    {
        list[lex_index].type = thensym;
    }
    else if (strcmp(list[lex_index].name, "else") == 0)
    {
        list[lex_index].type = elsesym;
    }
    else if (strcmp(list[lex_index].name, "call") == 0)
    {
        list[lex_index].type = callsym;
    }
    else if (strcmp(list[lex_index].name, "write") == 0)
    {
        list[lex_index].type = writesym;
    }
    else if (strcmp(list[lex_index].name, "read") == 0)
    {
        list[lex_index].type = readsym;
    }
    else if (strcmp(list[lex_index].name, "odd") == 0)
    {
        list[lex_index].type = oddsym;
    }
    // if not reserved, it is an identifier
    else
    {
        list[lex_index].type = identsym;
    }
    
}

int letterFirst(char *input, lexeme *list, int k)
{
    int j;
    int length;
    char *identBuffer = malloc(sizeof(char) * MAX_IDENT_LEN);
    
    j = 0;
    while (input[k] != '\0')
    {
        if (iscntrl(input[k]) || isspace(input[k]))
            break;
        
        if (j < MAX_IDENT_LEN)
        {
            if (isalpha(input[k]) || isdigit(input[k]))
            {
                identBuffer[j] = input[k];
                j++;
            }
            else
                break;
        }
        else if (isalpha(input[k]) || isdigit(input[k]))
        {
            printlexerror(4);
            return '\0';
        }
        k++;
    }
    
    length = (int)strlen(identBuffer);
    strncpy(list[lex_index].name, identBuffer, MAX_IDENT_LEN);
    compareReserve(list);
    
    return length;
}

int digitFirst(char *input, lexeme *list, int k)
{
    int j;
    int length;
    char *digitBuffer = malloc(sizeof(char) * MAX_NUMBER_LEN);
    
    j = 0;
    while (input[k] != '\0')
    {
        if (iscntrl(input[k]) || isspace(input[k]))
            break;
  
        if (j < MAX_NUMBER_LEN)
        {
            if (isdigit(input[k]))
            {
                digitBuffer[j] = input[k];
                j++;
            }
            else if (isalpha(input[k]))
            {
                printlexerror(2);
                return '\0';
            }
            else
                break;
        }
        // length error
        else if (isalpha(input[k]) || isdigit(input[k]))
        {
            printlexerror(3);
            return '\0';
        }
        k++;
    }
    length = (int)strlen(digitBuffer);
    
    strncpy(list[lex_index].name, digitBuffer, MAX_NUMBER_LEN);
    list[lex_index].value = atoi(list[lex_index].name);
    list[lex_index].type = numbersym;
    
    return length;
}

int isSymbol(char *input, lexeme *list, int k)
{
    int j; // index for buffer
    int length; // for return to increment lexanalyzer
    char *symBuffer = malloc(sizeof(char) * MAX_NUMBER_LEN);
    
    j = 0;
    while (input[k] != '\0')
    {
        if (iscntrl(input[k]) || isspace(input[k]))
            break;
       
        if (isalpha(input[k]) || isdigit(input[k]))
            break;
        
        if (j < MAX_NUMBER_LEN)
        {
            symBuffer[j] = input[k];
            j++;
            
            // matches as we go, if a match is found break out of loop to return
            length = (int)strlen(symBuffer);
            strncpy(list[lex_index].name, symBuffer, MAX_NUMBER_LEN);
                
            if (strcmp(list[lex_index].name, ":=") == 0)
            {
                list[lex_index].type = assignsym;
                break;
            }
            else if (strcmp(list[lex_index].name, "+") == 0)
            {
                list[lex_index].type = addsym;
                break;
            }
            else if (strcmp(list[lex_index].name, "-") == 0)
            {
                list[lex_index].type = subsym;
                break;
            }
            else if (strcmp(list[lex_index].name, "*") == 0)
            {
                list[lex_index].type = multsym;
                break;
            }
            else if (strcmp(list[lex_index].name, "/") == 0)
            {
                list[lex_index].type = divsym;
                break;
            }
            else if (strcmp(list[lex_index].name, "%") == 0)
            {
                list[lex_index].type = modsym;
                break;
            }
            else if (strcmp(list[lex_index].name, "==") == 0)
            {
                list[lex_index].type = eqlsym;
                break;
            }
            else if (strcmp(list[lex_index].name, "!=") == 0)
            {
                list[lex_index].type = neqsym;
                break;
            }
            
            else if (strcmp(list[lex_index].name, "<=") == 0)
            {
                list[lex_index].type = leqsym;
                break;
            }
            else if (strcmp(list[lex_index].name, ">=") == 0)
            {
                list[lex_index].type = geqsym;
                break;
            }
            else if (strcmp(list[lex_index].name, "(") == 0)
            {
                list[lex_index].type = lparensym;
                break;
            }
            else if (strcmp(list[lex_index].name, ")") == 0)
            {
                list[lex_index].type = rparensym;
                break;
            }
            else if (strcmp(list[lex_index].name, ",") == 0)
            {
                list[lex_index].type = commasym;
                break;
            }
            else if (strcmp(list[lex_index].name, ".") == 0)
            {
                list[lex_index].type = periodsym;
                break;
            }
            else if (strcmp(list[lex_index].name, ";") == 0)
            {
                list[lex_index].type = semicolonsym;
                break;
            }
        }
        // if exceeds max length print error and return
        else
        {
            printlexerror(1);
            return '\0';
        }
        k++;
    }
    
    // symbols that share components with other symbols are checked last
    if (strcmp(list[lex_index].name, "<") == 0)
    {
        list[lex_index].type = lsssym;
    }
    else if (strcmp(list[lex_index].name, ">") == 0)
    {
        list[lex_index].type = gtrsym;
    }
    
    // if no matches found, print error and return
    if (list[lex_index].type == '\0')
    {
        printlexerror(1);
        return '\0';
    }
    return length;
}

lexeme *lexanalyzer(char *input)
{
    int i = 0, add; // i = input counter , add to increment i past the already used input
    list = malloc(MAX_NUMBER_TOKENS * sizeof(struct lexeme));
    lex_index = 0;
    
    while (input[i] != '\0')
    {
        // ignore comments in input
        if (input[i] == '/' && input[i+1] == '/')
        {
            while(input[i] != '\n')
                i++;
        }
        
        // ignore white space
        if (iscntrl(input[i]) || isspace(input[i]))
        {
            i++;
            continue;
        }
    
        if (isalpha(input[i]))
        {
            add = letterFirst(input, list, i); // i is index of the first letter
            lex_index++;
        }
        else if (isdigit(input[i]))
        {
            add = digitFirst(input, list, i); // i is index of the first letter
            lex_index++;
        }
        // if neither letter or digit, it is a symbol
        else
        {
            add = isSymbol(input, list, i);
            lex_index++;
        }
        if (add == '\0')    // an error occured
            return NULL;
        i += add;
    }
    printtokens();
    
    return list;
}

void printtokens()
{
    int i;
    printf("Lexeme Table:\n");
    printf("lexeme\t\ttoken type\n");
    for (i = 0; i < lex_index; i++)
    {
        switch (list[i].type)
        {
            case oddsym:
                printf("%11s\t%d", "odd", oddsym);
                break;
            case eqlsym:
                printf("%11s\t%d", "==", eqlsym);
                break;
            case neqsym:
                printf("%11s\t%d", "!=", neqsym);
                break;
            case lsssym:
                printf("%11s\t%d", "<", lsssym);
                break;
            case leqsym:
                printf("%11s\t%d", "<=", leqsym);
                break;
            case gtrsym:
                printf("%11s\t%d", ">", gtrsym);
                break;
            case geqsym:
                printf("%11s\t%d", ">=", geqsym);
                break;
            case modsym:
                printf("%11s\t%d", "%", modsym);
                break;
            case multsym:
                printf("%11s\t%d", "*", multsym);
                break;
            case divsym:
                printf("%11s\t%d", "/", divsym);
                break;
            case addsym:
                printf("%11s\t%d", "+", addsym);
                break;
            case subsym:
                printf("%11s\t%d", "-", subsym);
                break;
            case lparensym:
                printf("%11s\t%d", "(", lparensym);
                break;
            case rparensym:
                printf("%11s\t%d", ")", rparensym);
                break;
            case commasym:
                printf("%11s\t%d", ",", commasym);
                break;
            case periodsym:
                printf("%11s\t%d", ".", periodsym);
                break;
            case semicolonsym:
                printf("%11s\t%d", ";", semicolonsym);
                break;
            case assignsym:
                printf("%11s\t%d", ":=", assignsym);
                break;
            case beginsym:
                printf("%11s\t%d", "begin", beginsym);
                break;
            case endsym:
                printf("%11s\t%d", "end", endsym);
                break;
            case ifsym:
                printf("%11s\t%d", "if", ifsym);
                break;
            case thensym:
                printf("%11s\t%d", "then", thensym);
                break;
            case elsesym:
                printf("%11s\t%d", "else", elsesym);
                break;
            case whilesym:
                printf("%11s\t%d", "while", whilesym);
                break;
            case dosym:
                printf("%11s\t%d", "do", dosym);
                break;
            case callsym:
                printf("%11s\t%d", "call", callsym);
                break;
            case writesym:
                printf("%11s\t%d", "write", writesym);
                break;
            case readsym:
                printf("%11s\t%d", "read", readsym);
                break;
            case constsym:
                printf("%11s\t%d", "const", constsym);
                break;
            case varsym:
                printf("%11s\t%d", "var", varsym);
                break;
            case procsym:
                printf("%11s\t%d", "procedure", procsym);
                break;
            case identsym:
                printf("%11s\t%d", list[i].name, identsym);
                break;
            case numbersym:
                printf("%11d\t%d", list[i].value, numbersym);
                break;
        }
        printf("\n");
    }
    printf("\n");
    printf("Token List:\n");
    for (i = 0; i < lex_index; i++)
    {
        if (list[i].type == numbersym)
            printf("%d %d ", numbersym, list[i].value);
        else if (list[i].type == identsym)
            printf("%d %s ", identsym, list[i].name);
        else
            printf("%d ", list[i].type);
    }
    printf("\n");
    list[lex_index++].type = -1;
}

void printlexerror(int type)
{
    if (type == 1)
        printf("Lexical Analyzer Error: Invalid Symbol\n");
    else if (type == 2)
        printf("Lexical Analyzer Error: Invalid Identifier\n");
    else if (type == 3)
        printf("Lexical Analyzer Error: Excessive Number Length\n");
    else if (type == 4)
        printf("Lexical Analyzer Error: Excessive Identifier Length\n");
    else
        printf("Implementation Error: Unrecognized Error Type\n");
    
    free(list);
    return;
}
