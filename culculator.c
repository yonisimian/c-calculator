#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "list.h"

char* error = "";

void getExpression(char* expression);
enum ErrorCode validate(char* expression);
double calculate(char* expression);
void checkAnotherCulculation(int* another);

int findEndOfNumber(char* string, int index);
int findClosingBracket(char* string, int index);

void generateHelpTitle();
void generateQuitTitle();
void generateHistoryTitle();

/*
    TODO:
        - Get and print opening and closing titles from a file (calculate needed #)
        - Check validity of the expression
        - Make "error" extern / modifiable via utils & list
*/
int main()
{
    // Opening title
    generateHelpTitle();

    // Declarations
    int another = 1; // Continue if we want another culculation
    char expression[MAX_LENGTH]; // The expression to calculate
    error = "";

    // Main loop
    while (another)
    {
        getExpression(expression);
        switch (validate(expression))
        {
            case NO_ERROR:
            {
                double result = calculate(expression);
                if (result == INFINITY)
                    printf("%s\n", error);
                else
                    printf("The result is: %g\n", result);
                break;
            }
            case TOO_LONG: printf("The expression is too long. We don't do that here.\n"); break;
            default: printf("Unknown error occured during the calculation.\nWe're sorry :(\n"); break;
        }

        //printf("\nexpression is %s\n\n", expression);
        checkAnotherCulculation(&another);
        printf("\n");
        error = "";
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
double calculate(char* expression)
{
    // Declerations
    List list = listCreate();
    int list_index = 0;
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
                break;
            default:
            {
                if(cur_op.value == '(')
                {
                    int end = findClosingBracket(expression, i);
                    char* inner_expression = substring(expression, i + 1, end);
                    double inner_result = calculate(inner_expression);
                    free(inner_expression);

                    if (strlen(error) > 0)
                        return 0;

                    cur_op.type = OPRAND;
                    cur_op.value = inner_result;

                    // adding * if needed so 5(1+2) will become 5*(1+2)
                    if (list_index > 0)
                    {
                        Op last_op = *listGet(list, list_index - 1);
                        if (last_op.type == OPRAND)
                        {
                            Op new_op = { .type = OPERATOR, .value = '*' };
                            listAdd(list, new_op);
                        }
                    }
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

// Check if the user wants another culculation
void checkAnotherCulculation(int* another)
{
    printf("\nDo you want me to perform another culculation? (Y/n) ");
    int n = getchar();

    *another = (n == 'n' || n == 'N' || n == EOF) ? 0 : 1;
    getchar(); // IDK why, but it prevents another loop
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
                strcpy(error, "SYNTAX ERROR: Too many opening brackets");
                return -1;
                break;
            default:
                break;
        }
    }
    return -1;
}

void generateHelpTitle()
{
    printf(" #####################################################################\n");
    printf("##                                                                   ##\n"); 
    printf("##   This is a culculator program.                                   ##\n"); 
    printf("##                                                                   ##\n"); 
    printf("##   Write an expression using the next operations: + - * / %% ^ !    ##\n"); 
    printf("##   Write HELP to show this message again.                          ##\n"); 
    printf("##   Write HISTORY to show this culculation history.                 ##\n"); 
    printf("##   Write QUIT to quit the program.                                 ##\n"); 
    printf("##                                                                   ##\n"); 
    printf(" #####################################################################\n\n");
}

void generateQuitTitle()
{
    printf("############################################\n");
    printf("#  Thanks for using my culculator program! #\n"); 
    printf("############################################\n\n");
}

void generateHistoryTitle()
{
    printf("##################\n");
    printf("#  Comming Soon! #\n"); 
    printf("##################\n\n");
}