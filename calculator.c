/*
	+-------------------------------------------------------------+
	|                      C Calculator V1.0                      |
	|                                                             |
    | Author: Yehonatan Simian © 2021                             |
    | Read README.md for additional information                   |
    |                                                             |
    | https://github.com/yonisimian                               |
	+-------------------------------------------------------------+
*/

// ====================== includes ====================== //

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <conio.h>
#include <stdarg.h>

#include "list.h"
#include "queue.h"
#include "defines.h"

// ====================== declarations ====================== //

Queue history;
char* HELP_TITLE;
char* QUIT_TITLE;

// Storage variables
double variable_x = 0;
double variable_y = 0;
double variable_z = 0;
double variable_ans = 0;

// main's functions
void getExpression(char* expression);
enum ErrorCode validateExpression(char* expression, int* another);
int isUserCommand(char* expression, int* another);
int isAssignment(char* expression);

double calculate(char* expression);
void addMultiplicationIfNeeded(List list);
void addZeroBeforeMinus(List list);

// More utility functions for calculate()
int findEndOfNumber(char* string, int index);
int findEndOfWord(char* string, int index);
int findClosingBracket(char* string, int index);
OpType getType(char* string, int index);
int getWord(char* word);
static char* WORDS[] = {"help", "history", "quit", "sin", "cos", "tan", "sqrt", "log", "ln", "abs", "e", "pi", "phi", "x", "y", "z", "ans"}; 

// Miscellaneous utility functions
char* substring(char string[], int start, int length);
double substringToDouble(char string[], int start, int length);
void reduceSpaces(char* string);

// Titles' i/o functions
static char* generateTitle(const char* from, const char* to);
char* generateHelpTitle(void);
char* generateQuitTitle(void);
void makeItCool(char dest[], char lines[][MAX_LENGTH], int num_of_lines, int line_length);

double error(int count, ...);
int quitProgram(void);

// ====================== main ====================== //
int main()
{
    // importing titles
    HELP_TITLE = generateHelpTitle();
    if (strcmp(HELP_TITLE, "") == 0)
        return quitProgram();
    QUIT_TITLE = generateQuitTitle();
    printf("%s\n", HELP_TITLE);

    // Declarations
    int another = 1;             // Continue if we want another calculation
    char expression[MAX_LENGTH]; // The expression to calculate
    history = queueCreate();

    // Main loop
    while (another)
    {
        getExpression(expression);
        switch (validateExpression(expression, &another))
        {
            case NO_ERROR:
            {
                double result = calculate(expression);
                if (result != ERROR_VALUE)
                {
                    printf("The result is: %g\n", result);
                    enqueue(history, expression, result);
                    variable_ans = result;
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
// ====================== end of main ====================== //

/** Get the expression input from the user */
void getExpression(char* expression)
{
    printf("Please insert an expression to calculate: ");
    gets(expression);
}

/** Check the validity of an expression, and return the proper error code.
 * If the expression is valid, the error core "NO_ERROR" will be returned.
 * Set "another" to 0 if user chose to quit. */
ErrorCode validateExpression(char* expression, int* another)
{
    if (expression == NULL || strlen(expression) < 1)
        return SILENT_ERROR;

    if (isUserCommand(expression, another))
        return SILENT_ERROR;

    reduceSpaces(expression);
    if ((int)strlen(expression) > MAX_LENGTH)
        return TOO_LONG;

    int is_assignment = isAssignment(expression);
    if (is_assignment == 1)
        return IS_ASSIGNMENT;
    else if (is_assignment == -1) // is assignment, but gone wrong. error message will tell what happened.
        return SILENT_ERROR;
    
    return NO_ERROR;
}

/** Check if the input is HELP, HISTORY or QUIT.
 * Set "another" to 0 if user chose to quit. */
int isUserCommand(char* expression, int* another)
{
    char c = *expression;
    if ((!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))) || strlen(expression) > 10) // if not a letter or len > 10
        return 0;

    switch (getWord(expression))
    {
        case HELP:    printf("%s\n", HELP_TITLE); return 1;
        case HISTORY: queuePrint(history);        return 1;
        case QUIT:    *another = 0;               return 1;
        default:
            return 0;
    }
}

/** Check if the expression is accually an assignment (of storage variables).
 * Returns  1 if it is an assignment.
 * Returns  0 if it is NOT an assignment.
 * Returns -1 if it is an assignment, but gone wrong (example "x = 5+").
 **/
int isAssignment(char* expression)
{
    if (strlen(expression) < 3 || *(expression + 1) != '=' ||
    (*(expression) != 'x' && *(expression) != 'X' && *(expression) != 'y' && *(expression) != 'Y' && *(expression) != 'z' && *(expression) != 'Z'))
        return 0; // this is not an assignment

    expression += 2;
    double result = calculate(expression);
    if (result == ERROR_VALUE)
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

// ====================== calculate ====================== //
/** --- Main Algorithm ---
 * Calculate "expression" and return the result.
 * NOTE: if an error occures, an error message will be printed and ERROR_VALUE will be returned.
 **/
double calculate(char* expression)
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
                char* word = substring(expression, i, end + 1);
                if (word == NULL)
                {
                    printf("ERROR: Not enough memory to run this calculation. You may try again.\n");
                    return error(1, list);
                }
                int value = getWord(word);

                // commands: HELP, HISTORY, QUIT
                if (value >= HELP && value < SIN)
                {
                    printf("The words HELP, HISTORY and QUIT must come alone and not inside an expression.\n");
                    return error(2, list, word);
                }

                // functions: SIN, COS, TAN, SQRT, LOG, LN, ABS
                else if (value >= SIN && value < E)
                {
                    cur_op.type = FUNCTION;
                    cur_op.value = value;
                    addMultiplicationIfNeeded(list); // so 5sin(pi) becomes 5*sin(pi) 
                }

                // constants: E, PI, PHI,   storage variables: X, Y, Z, ANS
                else if (value >= E && value <= ANS)
                {
                    cur_op.type = OPRAND;
                    switch (value)
                    {
                        case E:   cur_op.value = e; break;
                        case PI:  cur_op.value = pi; break;
                        case PHI: cur_op.value = phi; break;
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
                    return error(2, list, word);
                }
                
                free(word);
                i = end;
                break;
            }
            default:
            {
                switch ((int)cur_op.value)
                {
                    case ' ':
                        continue;
                    case '(':
                    {
                        int end = findClosingBracket(expression, i);
                        char* inner_expression = substring(expression, i + 1, end);
                        double inner_result = calculate(inner_expression);
                        free(inner_expression);

                        if (end < 1)
                            return error(1, list);

                        cur_op.type = OPRAND;
                        cur_op.value = inner_result;

                        addMultiplicationIfNeeded(list); // so 5(1+2) becomes 5*(1+2)
                        i = end;
                        break;
                    }
                    case ')':
                    {
                        printf("%s: Too many closing parentheses, you silly mf LOL\n", SYNTAX_ERROR);
                        return error(1, list);
                    }
                    case '=':
                    {
                        printf("%s: '=' is not a valid character except for assignments (for x, y and z)\n", SYNTAX_ERROR);
                        return error(1, list);
                    }
                    default:
                    {
                        printf("%s: Unsupported character %c, what IS this, black magic?!\n", SYNTAX_ERROR, (int)cur_op.value);
                        return error(1, list);
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

    // rounding the result 
    if (result != ERROR_VALUE)
    {
        double epsilon = result - (int)result;
        if (epsilon < 0.000000001) result = (int)result;
        if (epsilon > 0.999999999) result = (int)(result + 1);
    }

    listDestroy(list);

    return result;
}
// ====================== end of calculate ====================== //

/** Add (*) if needed so 5PI will become 5*PI (= 5 * 3.14) */
void addMultiplicationIfNeeded(List list)
{
    int size = listSize(list);
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

/** Add 0 if needed so -5 will become (0-5) */
void addZeroBeforeMinus(List list)
{
    if (listSize(list) == 0)
        listAdd(list, (Op){ .type = OPRAND, .value = 0 });
    else
    {
        Op last_op = *listGet(list, listSize(list) - 1);
        if (last_op.type != OPRAND)
            listAdd(list, (Op){ .type = OPRAND, .value = 0 });
    }
}

/** Find the last digit of a number starts in "index"
 * NOTE: can return index of a point, since atof can handle it. */
int findEndOfNumber(char* string, int index)
{
    while (getType(string, ++index) == OPRAND);
    if (*(string + index) == '.')
        while (getType(string, ++index) == OPRAND);
    
    return index - 1;
}

/** Find the last letter of a word starts in "index" */
int findEndOfWord(char* string, int index)
{
    while (getType(string, ++index) == LETTER);

    return index - 1;
}

/** Find index of a bracket that closes the bracket in "index" */
int findClosingBracket(char* string, int index)
{
    int opening_parentheses = 1;
    while (++index)
    {
        char c = *(string + index);
        switch (c)
        {
            case '(':
                opening_parentheses++;
                break;
            case ')':
                if (--opening_parentheses == 0)
                    return index;
                break;
            case '\0': case EOF:
                printf("%s: Too many opening parentheses\n", SYNTAX_ERROR);
                return -1;
                break;
            default:
                break;
        }
    }
    printf("%s: Too many closing parentheses\n", SYNTAX_ERROR);
    return -1;
}

/** Get the type of the character in a specific index */
OpType getType(char* string, int index)
{
    char c = *(string + index);
    switch (c)
    {
        case '+': case '-': case '*': case '/': case '%': case '!': case '^': return OPERATOR;
        case '(': case ')': /*case '[': case ']': case '{': case '}':*/ return PARENTHESES;
        default:
        {
            if (c >= '0' && c <= '9')
                return OPRAND;
            
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
                return LETTER;
            
            return UNKNOWN;
        }
    }
}

/** Return the word's enum value if recognized.
 * NOTE: unrecognized words return UNKNOWN (=0). */
int getWord(char* word)
{
    for (int i = 0; i <= Z; i++)
        if (!strcasecmp(word, WORDS[i]))
                return (i+1);

    return UNKNOWN;
}

/** Return the substring of "string" from "start" to ("end" - 1)
 * NOTE: remember to free after usage!  */
char* substring(char string[], int start, int end)
{
    char* a = (char*)malloc((end - start + 1) * sizeof(char));

    for (int i = 0; i < end - start; i++)
        *(a + i) = string[start + i];
    *(a + end - start) = '\0';

    return a;
}

/** Convert substring to double (inclusive)
 * WARNING: NO VALIDATION! */
double substringToDouble(char* string, int start, int end)
{
    char s[end - start + 2];

    for (int i = 0; i < end - start + 1; i++)
        s[i] = string[start + i];
    s[end - start + 1] = '\0';

    return atof(s);
}

/** Reduce each space group in a string into one space. */
void reduceSpaces(char* string)
{
    const char* s = string;
    while (*s == ' ' && *s != '\0')
        s++;
    while ((*string++ = *s++) != '\0')
    {
        if ((*string++ = *s++) == ' ')
            while (*s == ' ')
                ++s;
    }
}

char* generateHelpTitle(void)
{
    return generateTitle("---HELP TITLE---\n", "---HELP TITLE END---\n");
}

char* generateQuitTitle(void)
{
    return generateTitle("---QUIT TITLE---\n", "---QUIT TITLE END---");
}

static char* generateTitle(const char* from, const char* to)
{
    FILE* stream = fopen(TITLES_FILE_NAME, "r");

    if (stream == NULL)
    {
        printf("#################################\nERROR: titles.txt not found!\n#################################\n");
        return "";
    }

    // algorithming magik oooohhhh
    char lines[50][MAX_LENGTH];
    char cur_line[MAX_LENGTH]; 
    int num_of_lines = 0;
    int max_length = 0;
    int success = 0;
    while (fgets(cur_line, MAX_LENGTH, stream) != NULL)         // read lines
        if (strcmp(cur_line, from) == 0)        // start reading title
            while (fgets(cur_line, MAX_LENGTH, stream) != NULL) // reading title's lines
            {
                if (strcmp(cur_line, to) != 0)
                {
                    strcpy(lines[num_of_lines++], cur_line);
                    int cur_length = strlen(cur_line);
                    if (cur_length > max_length)
                        max_length = cur_length;
                }
                else
                {
                    success = 1;
                    goto finished_loops;
                }
            }

    finished_loops:
    if (!success)
    {
        printf("#################################\nERROR: titles.txt is corrupted!\n#################################\n");
        return "";
    }

    char* result = (char*)malloc((num_of_lines + 6) * (max_length + 10) + 1); // +1 for the '\0' char
    if (result == NULL)
        return generateTitle(from, to);
        
    makeItCool(result, lines, num_of_lines, max_length);
    fclose(stream);

    return result;
}

/** Take an array of lines, add a border and put the result to dest.
 * NOTE: must be handled carefully. */
void makeItCool(char dest[], char lines[][MAX_LENGTH], int num_of_lines, int line_length)
{
    //printf("starting prettifing. num_of_lines: %d + 6. line_length: %d + 10.\n", num_of_lines, line_length);
    line_length += 10; // 5 more chars in the start and in the end.
    num_of_lines += 6; // 3 more line in the start and in the end, it doesn't really matter.
    char result[num_of_lines * line_length + 1]; // +1 for the '\0' char. 
    int i = 0;
    for (int line = 0; line < num_of_lines; line++, i=0)
    {
        //printf("\n------starting line %d--------\n", line);
        if (line == 0 || line == num_of_lines - 1)      // first and last lines
        {
            while(i < 4)                result[line * line_length + i++] = ' ';
            while(i < line_length - 5)  result[line * line_length + i++] = BORDER_CHAR;
            while(i < line_length - 1)  result[line * line_length + i++] = ' ';
        }
        else if (line == 1 || line == num_of_lines - 2) // second and almost-last lines
        {
            /* first char is ' '*/      result[line * line_length + i++] = ' ';
            while(i < line_length - 2)  result[line * line_length + i++] = BORDER_CHAR;
            /* last char is ' '*/       result[line * line_length + i++] = ' ';
        }
        else if (line == 2 || line == num_of_lines - 3) // second and almost-last lines
        {
            while(i < 2)                result[line * line_length + i++] = BORDER_CHAR;
            while(i < line_length - 3)  result[line * line_length + i++] = ' ';
            while(i < line_length - 1)  result[line * line_length + i++] = BORDER_CHAR;
        }
        else                                            // every other line
        {
            while(i < 2)                result[line * line_length + i++] = BORDER_CHAR;
            while(i < 5)                result[line * line_length + i++] = ' ';
            while((result[line * line_length + i] = lines[line - 3][i - 5]) != '\n') i++; // copy the line itself
            while(i < line_length - 3)  result[line * line_length + i++] = ' ';
            while(i < line_length - 1)  result[line * line_length + i++] = BORDER_CHAR;
        }
        result[line * line_length + i] = '\n';
        //for(int j = 0; j < line * line_length + i; j++) printf("%c", result[j]);
    }
    result[num_of_lines * line_length] = '\0';

    strcpy(dest, result);
}

/** Get count and that number of arguments, free those arguments and return ERROR_VALUE
 * NOTE: function does NOT check if argument is freeable! */ 
double error(int count, ...)
{
    va_list list;
    va_start(list, count);
    for (int i = 0; i < count; i++)
        free(va_arg(list, void*));
    va_end(list);

    return ERROR_VALUE;
}

/** Quit the program ellegantly (did I write it right? or left? :\) **/
int quitProgram(void)
{
    printf("Press any key to continue...\n");
    getch();

    return 0;
}