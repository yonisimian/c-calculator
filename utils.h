#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_LENGTH 100 // The maximum length of an expression
#define TITLES_FILE_NAME "titles.txt" // for reading titles
#define BORDER_CHAR '#'

#define SYNTAX_ERROR "SYNTAX ERROR"
#define MATH_ERROR   "MATH ERROR"
#define CODING_ERROR "CODING ERROR"

typedef enum ErrorCode
{
    NO_ERROR,
    TOO_LONG
} ErrorCode;

typedef enum OpType
{
    UNKNOWN,
    OPRAND,     // 1 2 etc.
    OPERATOR,   // + - etc.
    BRACKETS,   // ( )
    LETTER,     // each letter, used for recognizing words in the expression
    FUNCTION    // recognized words, stored in the list for calculating
} OpType;

typedef enum Word
{
    UNKNOWN_WORD,

    // command words
    HELP, HISTORY, QUIT,

    // functions
    SIN, COS, TAN, SQRT, LOG, LN, ABS,

    // constants
    E, PI, PHI,

    // storage variables
    X, Y, Z, ANS

} Word;

static char* WORDS[] = {"help", "history", "quit", "sin", "cos", "tan", "sqrt", "log", "ln", "abs", "e", "pi", "phi", "x", "y", "z"}; 

typedef struct Op
{
    OpType type;
    double value;
} Op;

OpType getType(char* string, int index);
int getWord(char* word);
char* substring(char string[], int start, int length);
double substringToDouble(char string[], int start, int length);
void removeSpaces(char* string);
int factorial(int num);

char* generateHelpTitle(void);
char* generateQuitTitle(void);
void makeItCool(char dest[], char lines[][MAX_LENGTH], int num_of_lines, int line_length);
static char* generateTitle(const char* from, const char* to);

// Get the type of the character in a specific index
OpType getType(char* string, int index)
{
    char c = *(string + index);
    switch (c)
    {
        case '+': case '-': case '*': case '/': case '%': case '!': case '^': return OPERATOR;
        case '(': case ')': /*case '[': case ']': case '{': case '}':*/ return BRACKETS;
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

// Returns the word's enum value if recognized.
// NOTE - unrecognized words return UNKNOWN (=0).
int getWord(char* word)
{
    for (int i = 0; i <= Z; i++)
        if (!strcasecmp(word, WORDS[i]))
                return (i+1);

    return UNKNOWN;
}

// Returns the substring of "string" from "start" to ("end" - 1)
// NOTE: remember to free after usage! 
char* substring(char string[], int start, int end)
{
    char* a = (char*)malloc((end - start + 1) * sizeof(char));

    for (int i = 0; i < end - start; i++)
        *(a + i) = string[start + i];
    *(a + end - start) = '\0';

    return a;
}

// Convert substring to double (inclusive)
// WARNING: NO VALIDATION!
double substringToDouble(char* string, int start, int end)
{
    char s[end - start + 2];

    for (int i = 0; i < end - start + 1; i++)
        s[i] = string[start + i];
    s[end - start + 1] = '\0';

    return atof(s);
}

// Remove all spaces from a string.
// NOTE: strings like "6  5" become "65".
void removeSpaces(char* string)
{
    const char* s = string;
    do {
        while (*s == ' ')
            ++s;
    } while ((*string++ = *s++));
}

// Calculates the factorial of a non-negative number
int factorial(int num)
{
    if (num < 0)
        return 0;
    
    if (num == 0 || num == 1)
        return 1;

    return num * factorial(num - 1);
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

    char* result = (char*)malloc((num_of_lines + 6) * (max_length + 10));
    if (result == NULL)
        return generateTitle(from, to);

    makeItCool(result, lines, num_of_lines, max_length);
    fclose(stream);

    return result;
}

// makes a nice border arount the text
void makeItCool(char dest[], char lines[][MAX_LENGTH], int num_of_lines, int line_length)
{
    //printf("starting prettifing. num_of_lines: %d + 6. line_length: %d + 10.\n", num_of_lines, line_length);
    line_length += 10; // 5 more chars in the start and in the end
    num_of_lines += 6; // 3 more line in the start and in the end, it doesn't really matter.
    char result[num_of_lines * line_length];
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

#endif