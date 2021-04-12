#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "list.h"

void getExpression(char* expression);
enum ErrorCode validate(char* expression);
double calculate(char* expression, int* another);
void addMultiplicationIfNeeded(List list);
void addZeroBeforeMinus(List list);

int findEndOfNumber(char* string, int index);
int findEndOfWord(char* string, int index);
int findClosingBracket(char* string, int index);

void generateHelpTitle();
void generateQuitTitle();
void generateHistoryTitle();

/*
    TODO:
        - Get and print titles from a file (calculate needed #)
        - Check validity of the expression
*/
int main()
{
    // Opening title
    generateHelpTitle();

    // Declarations
    int another = 1; // Continue if we want another culculation
    char expression[MAX_LENGTH]; // The expression to calculate

    // Main loop
    while (another)
    {
        getExpression(expression);
        switch (validate(expression))
        {
            case NO_ERROR:
            {
                double result = calculate(expression, &another);
                if (result != INFINITY)
                {
                    if(result < 0.000001 && result > -0.000001)
                        result = 0;
                    printf("The result is: %g\n", result);
                }
                break;
            }
            case TOO_LONG: printf("The expression is too long. We don't do that here.\n"); break;
            default: printf("Unknown error occured during the calculation.\nWe're sorry :(\n"); break;
        }
        printf("\n");
    }

    // Closing title
    generateQuitTitle();

    return 0;
}



// Get the expression from the user to the variable "expression"
void getExpression(char* expression)
{
    printf("Please insert an expression to calculate: ");
    gets(expression);
}

// Check the validity of an expression, and return the proper error code.
// If the expression is valid, the error core "NO_ERROR" will be returned.
ErrorCode validate(char* expression)
{
    //removeSpaces(expression);
    if ((int)strlen(expression) > MAX_LENGTH)
        return TOO_LONG;
    
    return NO_ERROR;
}

// calculates "expression" and prints result
// sets "another" to 0 if user chose to quit.
double calculate(char* expression, int* another)
{
    // Declerations
    List list = listCreate();
    double result = 0.0;

    // Fill list
    for (int i = 0; expression[i] != '\0' && i < MAX_LENGTH; i++)
    {
        Op cur_op = { .type = getType(expression, i), .value = expression[i] };
        switch (cur_op.type)
        {
            case OPRAND:
            {
                int end = findEndOfNumber(expression, i);
                cur_op.value = substringToDouble(expression, i, end);
                i = end;
                break;
            }
            case OPERATOR:
            {
                break;
            }
            case LETTER:
            {
                int end = findEndOfWord(expression, i);
                char* word = (char*)malloc(end + 1 - i);
                word = substring(expression, i, end + 1);
                int value = getWord(word);

                // commands: HELP, HISTORY, QUIT
                if (value >= HELP && value < SIN)
                {
                    if (strlen(word) == strlen(expression))
                    {
                        if (value == HELP) generateHelpTitle();
                        else if (value == HISTORY) generateHistoryTitle();
                        else *another = 0;
                    }
                    else
                        printf("The words HELP, HISTORY and QUIT must come alone and not inside an expression.\n");
                    return INFINITY;
                }

                // functions: SIN, COS, TAN, SQRT, LOG, LN, ABS
                else if (value >= SIN && value < E)
                {
                    cur_op.type = FUNCTION;
                    cur_op.value = value;
                }

                // constants: E, PI, PHI
                else if (value >= E && value < X)
                {
                    cur_op.type = OPRAND;
                    switch (value)
                    {
                        case E:   cur_op.value = 2.71828182846; break;
                        case PI:  cur_op.value = 3.14159265358; break;
                        case PHI: cur_op.value = 1.61803398874; break;
                    }
                    addMultiplicationIfNeeded(list); // so 5PI becomes 5*PI 
                }

                // storgae variables: X, Y, Z
                else if (value >= X && value < Z)
                {
                    //TODO: allow "x=5" to store 5 in x for later use.
                }

                else
                {
                    printf("SYNTAX ERROR: unknown word %s.\nWrite HELP for more information.\n", word);
                    return INFINITY;
                }
                free(word);
                i = end;
                break;
            }
            default:
            {
                if(cur_op.value == '(')
                {
                    int end = findClosingBracket(expression, i);
                    char* inner_expression = substring(expression, i + 1, end);
                    double inner_result = calculate(inner_expression, another);
                    free(inner_expression);

                    if (end < 1)
                        return INFINITY;

                    cur_op.type = OPRAND;
                    cur_op.value = inner_result;

                    addMultiplicationIfNeeded(list); // so 5(1+2) becomes 5*(1+2)
                    i = end;
                }
                break;
            }
        }
        listAdd(list, cur_op);
    }

    // Calculate list
    listCalculate(list);
    result = listGet(list, 0)->value;

    return result;
}

// adding * if needed so 5PI will become 5*PI = 5 * 3.14
void addMultiplicationIfNeeded(List list)
{
    int size = list->size;
    if (size > 0)
    {
        Op last_op = *listGet(list, size - 1);
        if (last_op.type == OPRAND)
        {
            Op new_op = { .type = OPERATOR, .value = '*' };
            listAdd(list, new_op);
        }
    }
}

// adding 0 if needed so -5 will become (0-5)
void addZeroBeforeMinus(List list)
{
    if (list->size == 0)
        listAdd(list, (Op){ .type = OPRAND, .value = 0 });
    else
    {
        Op last_op = *listGet(list, list->size - 1);
        if (last_op.type != OPRAND)
            listAdd(list, (Op){ .type = OPRAND, .value = 0 });
    }
}

// Find last digit of a number starts in "index"
// NOTE: can return index of a point, since atof can handle it.
int findEndOfNumber(char* string, int index)
{
    while (getType(string, ++index) == OPRAND);
    if (*(string + index) == '.')
        while (getType(string, ++index) == OPRAND);
    
    return index - 1;
}

// Find last letter of a word starts in "index"
int findEndOfWord(char* string, int index)
{
    while (getType(string, ++index) == LETTER);

    return index - 1;
}

// Find index of a bracket that closes the bracket in "index"
int findClosingBracket(char* string, int index)
{
    int opening_brackets = 1;
    while (++index)
    {
        char c = *(string + index);
        switch (c)
        {
            case '(':
                opening_brackets++;
                break;
            case ')':
                if (--opening_brackets == 0)
                    return index;
                break;
            case '\0': case EOF:
                printf("SYNTAX ERROR: Too many opening brackets");
                return -1;
                break;
            default:
                break;
        }
    }
    printf("SYNTAX ERROR: Too many closing brackets");
    return -1;
}

void generateHelpTitle()
{
    printf(" #####################################################################\n");
    printf("##                                                                   ##\n"); 
    printf("##   This is a culculator program.                                   ##\n"); 
    printf("##                                                                   ##\n"); 
    printf("##   Write an expression using the next operations: + - * / %% ^ !    ##\n");
    printf("##   You can use the constants e, pi, phi anytime you'd like.        ##\n"); 
    printf("##   You can use the functions sin, cos, tan, sqrt, log, ln, abs.    ##\n"); 
    printf("##                                                                   ##\n"); 
    printf("##   Write HELP to show this message again.                          ##\n"); 
    printf("##   Write HISTORY to show the culculation history.                  ##\n"); 
    printf("##   Write QUIT to quit the program.                                 ##\n"); 
    printf("##                                                                   ##\n"); 
    printf("##   The program is case-insensitive, so sin = SIN, pi = PI etc.     ##\n"); 
    printf("##   Enjoy!                                                          ##\n"); 
    printf("##                                                                   ##\n"); 
    printf(" #####################################################################\n\n");
}

void generateQuitTitle()
{
    printf(" ###############################################\n");
    printf("##                                             ##\n"); 
    printf("##   Thanks for using my culculator program!   ##\n"); 
    printf("##                                             ##\n"); 
    printf(" ###############################################\n\n");
}

void generateHistoryTitle()
{
    printf(" ####################\n");
    printf("##                  ##\n"); 
    printf("##   Coming soon!   ##\n"); 
    printf("##                  ##\n"); 
    printf(" ####################\n\n");
}