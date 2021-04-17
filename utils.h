#ifndef UTILS_H_
#define UTILS_H_

#define MAX_LENGTH 100 // The maximum length of an expression
#define TITLES_FILE_NAME "titles.txt" // for reading titles
#define BORDER_CHAR '#'

#define SYNTAX_ERROR "SYNTAX ERROR"
#define MATH_ERROR   "MATH ERROR"
#define CODING_ERROR "CODING ERROR"

typedef enum ErrorCode
{
    NO_ERROR,
    TOO_LONG,
    IS_ASSIGNMENT,
    SILENT_ERROR
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

typedef struct Op
{
    OpType type;
    double value;
} Op;

OpType getType(char* string, int index);
int getWord(char* word);
char* substring(char string[], int start, int length);
double substringToDouble(char string[], int start, int length);
void reduceSpaces(char* string);
int factorial(int num);

char* generateHelpTitle(void);
char* generateQuitTitle(void);
void makeItCool(char dest[], char lines[][MAX_LENGTH], int num_of_lines, int line_length);

#endif