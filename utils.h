#ifndef UTILS_H_
#define UTILS_H_

#define MAX_LENGTH 256 // The maximum length of an expression
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

/** Get the type of the character in a specific index */
OpType getType(char* string, int index);

/** Return the word's enum value if recognized.
 * NOTE: unrecognized words return UNKNOWN (=0). */
int getWord(char* word);

/** Return the substring of "string" from "start" to ("end" - 1)
 * NOTE: remember to free after usage!  */
char* substring(char string[], int start, int length);

/** Convert substring to double (inclusive)
 * WARNING: NO VALIDATION! */
double substringToDouble(char string[], int start, int length);

/** Reduce each space group in a string into one space. */
void reduceSpaces(char* string);

/** Calculate the factorial of a non-negative number. */
int factorial(int num);

char* generateHelpTitle(void);
char* generateQuitTitle(void);

/** Take an array of lines, add a border and put the result to dest.
 * NOTE: must be handled carefully. */
void makeItCool(char dest[], char lines[][MAX_LENGTH], int num_of_lines, int line_length);

#endif