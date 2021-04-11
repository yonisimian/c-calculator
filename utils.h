#ifndef UTILS_H_
#define UTILS_H_

#include <stdlib.h>
#include <math.h>

#define MAX_LENGTH 100 // The maximum length of an expression

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
    HELP,
    HISTORY,
    QUIT,

    // functions
    SIN,
    COS,
    TAN,
    SQRT,
    LOG,
    LN,
    ABS,

    // constants
    E,
    PI,
    PHI,

    // storage variables
    X,
    Y,
    Z

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

#endif