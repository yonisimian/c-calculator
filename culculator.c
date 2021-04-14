#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include "utils.h"
#include "list.h"
#include "queue.h"

Queue history;
char* HELP_TITLE;
char* QUIT_TITLE;

void getExpression(char* expression);
enum ErrorCode validate(char* expression);
int isAssignment(char* expression);
double calculate(char* expression, int* another);
void addMultiplicationIfNeeded(List list);
void addZeroBeforeMinus(List list);

int findEndOfNumber(char* string, int index);
int findEndOfWord(char* string, int index);
int findClosingBracket(char* string, int index);

int quitProgram(void);

/*
    TODO:
        - handle spaces
        - seperate headers and implementation
*/
int main()
{
    // importing titles
    HELP_TITLE = generateHelpTitle();
    if (strcmp(HELP_TITLE, "") == 0)
        return quitProgram();
    QUIT_TITLE = generateQuitTitle();
    printf("%s\n", HELP_TITLE);

    // Declarations
    int another = 1; // Continue if we want another culculation
    char expression[MAX_LENGTH]; // The expression to calculate
    history = queueCreate();

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
                    if(result < 0.000000001 && result > -0.000000001)
                        result = 0;
                    printf("The result is: %g\n", result);
                    enqueue(history, expression, result);
                }
                break;
            }
            case TOO_LONG: printf("The expression is too long. We don't do that here.\n"); break;
            case IS_ASSIGNMENT: printf("Assignment done successfuly :D\n"); break;
            case SILENT_ERROR: break;
            default: printf("Unknown error occured during the calculation.\nWe're sorry :(\n"); break;
        }
        printf("\n");
    }

    // Closing title
    printf("%s\n", QUIT_TITLE);

    queueDestroy(history);
    free(HELP_TITLE);
    free(QUIT_TITLE);

    return quitProgram();
}



// Get the expression from the user to the variable "expression"
void getExpression(char* expression)
{
    printf("Please insert an expression to calculate: ");
    gets(expression);
    //scanf("%s", expression);
}

// Check the validity of an expression, and return the proper error code.
// If the expression is valid, the error core "NO_ERROR" will be returned.
ErrorCode validate(char* expression)
{
    //removeSpaces(expression);
    if ((int)strlen(expression) > MAX_LENGTH)
        return TOO_LONG;

    int is_assignment = isAssignment(expression);
    if (is_assignment == 1)
        return IS_ASSIGNMENT;
    else if (is_assignment == -1) // is assignment, but gone wrong. error will tell what happened.
        return SILENT_ERROR;
    
    return NO_ERROR;
}

int isAssignment(char* expression)
{
    if (expression == NULL || strlen(expression) < 3 || *(expression + 1) != '=' ||
    (*(expression) != 'x' && *(expression) != 'X' && *(expression) != 'y' && *(expression) != 'Y' && *(expression) != 'z' && *(expression) != 'Z'))
        return 0; // this is not an assignment

    int dummy = 0;
    expression += 2;
    double result = calculate(expression, &dummy);
    if (result == INFINITY)
        return -1;
    
    expression -= 2;
    switch (*expression)
    {
        case 'x': case 'X': variable_x = result; return 1;
        case 'y': case 'Y': variable_y = result; return 1;
        case 'z': case 'Z': variable_z = result; return 1;
        default:
            return 0;
    }
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
                        if (value == HELP) printf("%s\n", HELP_TITLE);
                        else if (value == HISTORY) queuePrint(history);
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

                // constants: E, PI, PHI,   storage variables: X, Y, Z, ANS
                else if (value >= E && value <= ANS)
                {
                    cur_op.type = OPRAND;
                    switch (value)
                    {
                        case E:   cur_op.value = 2.71828182846; break;
                        case PI:  cur_op.value = 3.14159265358; break;
                        case PHI: cur_op.value = 1.61803398874; break;
                        case X:   cur_op.value = variable_x;    break;
                        case Y:   cur_op.value = variable_y;    break;
                        case Z:   cur_op.value = variable_z;    break;
                        case ANS: cur_op.value = variable_ans;  break;
                    }
                    addMultiplicationIfNeeded(list); // so 5PI becomes 5*PI 
                }

                else
                {
                    printf("%s: unknown word %s. Write HELP for more information.\n", SYNTAX_ERROR, word);
                    return INFINITY;
                }
                free(word);
                i = end;
                break;
            }
            default:
            {
                switch ((int)cur_op.value)
                {
                    case '(':
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
                        break;
                    }
                    case ')':
                    {
                        printf("%s: Too many closing brackets, you silly mf LOL\n", SYNTAX_ERROR);
                        return INFINITY;
                    }
                    case ' ':
                    {
                        printf("%s: I told you we don't support spaces yes, but you COULDN'T listen. Shame.\n", SYNTAX_ERROR);
                        return INFINITY;
                    }
                    case '=':
                    {
                        printf("%s: '=' is not a valid character except for assignments (for x, y and z)\n", SYNTAX_ERROR);
                        return INFINITY;
                    }
                    default:
                    {
                        printf("%s: Unsupported character %c, what IS this, black magic?!\n", SYNTAX_ERROR, (int)cur_op.value);
                        return INFINITY;
                    }
                }
                break;
            }
        }
        listAdd(list, cur_op);
    }

    // Calculate list
    listCalculate(list);
    result = listGet(list, 0)->value;

    listDestroy(list);

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
                printf("%s: Too many opening brackets\n", SYNTAX_ERROR);
                return -1;
                break;
            default:
                break;
        }
    }
    printf("%s: Too many closing brackets\n", SYNTAX_ERROR);
    return -1;
}

// quits the program ellegantly (did I write it right? or left? :\)
int quitProgram(void)
{
    printf("Press any key to continue...");
    getch();

    return 0;
}