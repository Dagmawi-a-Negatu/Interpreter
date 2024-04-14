/*
 * parser.c - recursive descent parser for a simple expression language.
 * Most of the functions in this file model a non-terminal in the
 * grammar listed below
 * Author: Dagmawi Negatu and Darwin Bueso Galdamez
 * Date:   Modified 9-29-08 and 3-25-15 and 14 april 2020
 */



#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include "tokenizer.h"
#include "parser.h"

#define ERROR -999999

int expr(char **expr);
int ttail(char **expr, int acc);
int term(char **expr);
int stail(char **expr, int acc);
int stmt(char **expr);
int ftail(char **expr, int acc);
int factor(char **expr);
int expp(char **expr);
char add_sub_tok(char **expr);
char mul_div_tok(char **expr);
char* compare_tok(char **expr);
int num(char **expr);
//int is_operator(char c);

/*
 * <bexpr> ::= <expr> ;
 * <expr> ::=  <term> <ttail>
 * <ttail> ::=  <add_sub_tok> <term> <ttail> | e
 * <term> ::=  <stmt> <stail>
 * <stail> ::=  <mult_div_tok> <stmt> <stail> | e
 * <stmt> ::=  <factor> <ftail>
 * <ftail> ::=  <compare_tok> <factor> <ftail> | e
 * <factor> ::=  <expp> ^ <factor> | <expp>
 * <expp> ::=  ( <expr> ) | <num>
 * <add_sub_tok> ::=  + | -
 * <mul_div_tok> ::=  * | /
 * <compare_tok> ::=  < | > | <= | >= | != | ==
 * <num> ::=  {0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9}+
 */

int parse_expression(char *token) {
    return expr(&token);
}

int expr(char **expr) {
    int term_val = term(expr);
    if (term_val == ERROR) return ERROR;
    return ttail(expr, term_val);
}

int ttail(char **expr, int acc) {
    char op = add_sub_tok(expr);

    if (op == '+' || op == '-') {
        int term_val = term(expr);
        if (term_val == ERROR) return ERROR;
        acc = (op == '+') ? acc + term_val : acc - term_val;
        return ttail(expr, acc);
    } else if (**expr == ';' || **expr == '\0') {
        if (**expr == ';') {
            (*expr)++;  // Consume ';' if present
        }
        return acc;
    } else {
        return ERROR; 
    }
   
}

int term(char **expr) {
    int stmt_val = stmt(expr);
    if (stmt_val == ERROR) {
        return ERROR;
    }
    return stail(expr, stmt_val);  
}

// The stail function with the use of mul_div_tok function
int stail(char **expr, int acc) {
    char op = mul_div_tok(expr);

    while (op != '\0') {
        if (op == (char)ERROR) {
            return ERROR; // Incorrect token, return error
        }

        (*expr)++; // Consume the operator

        int stmt_val = stmt(expr); // Parse the next statement
        if (stmt_val == ERROR) {
            return ERROR; // Propagate error
        }

        // Perform the operation based on the operator
        if (op == '*') {
            acc *= stmt_val;
        } else if (op == '/') {
            if (stmt_val == 0) {
                fprintf(stderr, "Runtime Error: Division by zero.\n");
                return ERROR; // Division by zero error
            }
            acc /= stmt_val;
        }

        op = mul_div_tok(expr); // Look for another multiplication or division operator
    }

    return acc; // Return the accumulated result

}



// Implementation of the stmt function
int stmt(char **expr) {
    // Parse the factor
    int factor_val = factor(expr);
    if (factor_val == ERROR) {
        return ERROR; // Propagate errors
    }

    // Now handle any following comparison operations using ftail
    return ftail(expr, factor_val);
}

// Implement the ftail function
int ftail(char **expr, int acc) {
    char* comp_op = compare_tok(expr);  // Get the comparison operator if there is one
    if (comp_op == NULL) {
        // No comparison operator means we just return the accumulated value
        return acc;
    }

    // Consume the comparison operator
    if (strlen(comp_op) == 2) { // For operators ==, !=, <=, >=
        (*expr) += 2;
    } else {
        (*expr) += 1; // For operators <, >
    }

    // There is a comparison operator, evaluate the comparison
    int factor_val = factor(expr);  // Evaluate the right side of the comparison
    if (factor_val == ERROR) {
        return ERROR;  // Propagate the error
    }

    // Perform the comparison
    if (strcmp(comp_op, "<") == 0) {
        return acc < factor_val;
    } else if (strcmp(comp_op, ">") == 0) {
        return acc > factor_val;
    } else if (strcmp(comp_op, "<=") == 0) {
        return acc <= factor_val;
    } else if (strcmp(comp_op, ">=") == 0) {
        return acc >= factor_val;
    } else if (strcmp(comp_op, "!=") == 0) {
        return acc != factor_val;
    } else if (strcmp(comp_op, "==") == 0) {
        return acc == factor_val;
    }

    return ERROR;  // Return the last evaluated comparison result
}

// Function to parse factors, potentially with exponentiation
int factor(char **expr) {
    int base = expp(expr);  // Parse the base expression or number
    if (base == ERROR) {
        return ERROR;  // Propagate errors
    }

    // Check for the exponentiation operator
    if (**expr == '^') {
        (*expr)++;  // Consume '^'
        int exponent = factor(expr);  // Recursively parse the exponent
        if (exponent == ERROR) {
            return ERROR;  // Propagate errors
        }
        double result = pow((double)base, (double)exponent);

        if (isinf(result) || isnan(result)) {  // Check for infinite or NaN results
            fprintf(stderr, "Error: Non-finite result in exponentiation.\n");
            return ERROR;
        }
        return (int)result;
    }

    return base;  // Return the base if there's no exponentiation
}

// Function to parse exponentiation base or a simple number
int expp(char **current_expr) {
    while (isspace(**current_expr)) (*current_expr)++;

    if (**current_expr == '(') {
        (*current_expr)++;  // Consume '('
        int value = expr(current_expr);  // Recursively parse the expression inside the parentheses
        if (value == ERROR) {
            return ERROR;  // Propagate the error if the internal expression is invalid
        }
        if (**current_expr != ')') {
            return ERROR;  // Error if no closing ')'
        }
        (*current_expr)++;  // Consume ')'  
        return value; // Return the value of the sub-expression
    } else {
        return num(current_expr);  // Parse number
    }
}

// Helper function to check if the character is a valid operator
//int is_operator(char c) {
    //return c == '+' || c == '-' || c == '*' || c == '/' || c == '^' ||
           //c == '<' || c == '>' || c == '=' || c == '!';
//}

// Function to recognize addition and subtraction tokens
char add_sub_tok(char **expr) {
    while (isspace(**expr)) (*expr)++;

    char op = **expr;

    if (op == '+' || op == '-') {
        (*expr)++;
        return op;
    }


    return (char)ERROR;
}

// Function to recognize multiplication and division tokens
char mul_div_tok(char **expr) {
    while (isspace(**expr)) (*expr)++; // Skip whitespace

    // Check for multiplication or division operator
    char op = **expr;
    if (op == '*' || op == '/') {
        return op; // Operator found, return it without consuming
    }

    return '\0'; // Not an operator, return null character
}

// Function to recognize comparison tokens
char* compare_tok(char **expr) {
    while (isspace(**expr)) (*expr)++;  // Skip any whitespace

    char first_char = **expr;
    // Check for two-character comparison operators
    if (first_char == '<' || first_char == '>' || first_char  == '!' || first_char == '=') {
        char second_char = *(*expr + 1);

        if (second_char == '=') {
            static char result[3]; // Static to return a local array
            result[0] = first_char;
            result[1] = second_char;
            result[2] = '\0';
            *expr += 2;  // Consume both characters
            return result;  // Return operators like "<=", ">=", "!=" or "=="
        }else if (first_char == '<' || first_char == '>') {
            static char result[2];
            result[0] = first_char;
            result[1] = '\0';
            *expr += 1;
            return result;
        }
    }

    return NULL;  // Return NULL if no comparison token is found
}

// Function to parse and return a number from the expression
int num(char **expr) {
    int sign = 1; // Default sign is positive
    while (isspace(**expr)) (*expr)++;  // Skip whitespace
    
    // Check for sign
    if (**expr == '+' || **expr == '-') {
        sign = (**expr == '-') ? -1 : 1;
        (*expr)++; // Consume the sign

        // After consuming a sign, there should be no space before the number
        if (isspace(**expr)) {
            return ERROR; // Syntax error due to space after sign
        }
    }

    char *next;
    errno = 0; // To detect range errors
    long value = strtol(*expr, &next, 10);  // Convert string to long

    if (*expr == next || errno == ERANGE) {
        // No digits were found, return error code
        return ERROR;
    } else {
        // Successfully parsed number, update the expression pointer
        *expr = next;
        return (int)value * sign;  // Cast to int if necessary, depending on the expected value range
    }
}
