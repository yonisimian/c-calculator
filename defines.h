#ifndef DEFINES_H_
#define DEFINES_H_

#define MAX_LENGTH 256                // The maximum length of an expression
#define MAX_HISTORY_COUNT 50          // for the history queue
#define TITLES_FILE_NAME "titles.txt" // for reading titles
#define BORDER_CHAR '#'               // how the border will look

#define SYNTAX_ERROR "SYNTAX ERROR"
#define MATH_ERROR   "MATH ERROR"
#define CODING_ERROR "CODING ERROR"
#define ERROR_VALUE INFINITY

#define e   2.71828182846
#define pi  3.14159265358
#define phi 1.61803398874

extern char is_radians; // initialized in "list.c", changed in "calculator.c"

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
    PARENTHESES,// ( )
    LETTER,     // input letters, can become FUNCTIONs (sin) or OPRANDs (pi)
    FUNCTION    // recognized words, stored in the list for calculating
} OpType;

typedef enum Word
{
    UNKNOWN_WORD,

    // command words
    HELP, HISTORY, QUIT, RAD, DEG,

    // functions
    SIN, COS, TAN, SQRT, LOG, LN, ABS,

    // constants
    E, PI, PHI,

    // storage variables
    X, Y, Z, ANS

} Word;

typedef struct Op
{
    OpType type;    // OPRAND or OPERATOR or FUNCTION
    double value;
} Op;

#endif