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
    OPRAND,
    OPERATOR,
    BRACKETS
} OpType;

typedef struct Op
{
    OpType type;
    double value;
} Op;

OpType getType(char* string, int index);
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
        case '(': case ')': case '[': case ']': case '{': case '}': return BRACKETS;
        default:
        {
            if (c >= '0' && c <= '9')
                return OPRAND;
            
            return UNKNOWN;
        }
    }
}

// Returns the substring of "string" from "start" to ("end" - 1)
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