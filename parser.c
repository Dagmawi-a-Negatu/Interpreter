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
    while (isspace(**expr)) (*expr)++;
    if (**expr == '+' || **expr == '-') {
        char op = *(*expr)++;
        int term_val = term(expr);
        if (term_val == ERROR) return ERROR;
        acc = (op == '+') ? acc + term_val : acc - term_val;
        return ttail(expr, acc);
    } else if (**expr == ';') {
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
