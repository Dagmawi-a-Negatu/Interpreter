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
#include "tokenizer.h"
#include "parser.h"

#define ERROR -999999

int expr(char **expr);
int ttail(char **expr, int acc);
int term(char **expr);
char add_sub_tok(char **expr);


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
    } else if (op == '\0' && **expr == ';') {
        (*expr)++; 
        return acc;
    } else {
        return ERROR; 
    }
   
}

int term(char **expr) {
    while (isspace(**expr)) (*expr)++;
    char *next;
    int value = strtol(*expr, &next, 10);
    if (*expr == next) {
        return ERROR;
    } else {
        *expr = next;
        return value;
    }
}

// The stail function with the use of mul_div_tok function
int stail(char **expr, int acc) {
    char op = mul_div_tok(expr);  // Use the helper function to get the operator

    while (op != '\0') {
        int stmt_val = stmt(expr);  // Assuming stmt is correctly implemented
        if (stmt_val == ERROR) {
            return ERROR;
        }

        if (op == '*') {
            acc *= stmt_val;
        } else if (op == '/') {
            if (stmt_val == 0) {
                fprintf(stderr, "Runtime Error: Division by zero.\n");
                return ERROR;  // Division by zero error
            }
            acc /= stmt_val;
        }

        op = mul_div_tok(expr);  // Get the next operator if there is more to process
    }
    
    return acc;  // Return the accumulated result
}



// Function to recognize addition and subtraction tokens
char add_sub_tok(char **expr) {
    while (isspace(**expr)) (*expr)++;
    if (**expr == '+' || **expr == '-') {
        return *(*expr)++;  // Consume and return the operator
    }
    return '\0';  // Return null character if no add/sub token is found
}

// Function to recognize multiplication and division tokens
char mul_div_tok(char **expr) {
    while (isspace(**expr)) (*expr)++;
    if (**expr == '*' || **expr == '/') {
        return *(*expr)++;  // Consume and return the operator
    }
    return '\0';  // Return null character if no mul/div token is found
}
