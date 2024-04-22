/*
 * parser.c - recursive descent parser for a simple expression language.
 * Most of the functions in this file model a non-terminal in the
 * grammar listed below
 * Author: Dagmawi Negatu and Darwin Bueso Galdamez
 * Date: April 2024
 */


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include "tokenizer.h"
#include "parser.h"


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

/**
 * bexpr - parses the expression rule from the grammar.
 * @token: the input expression to parse.
 *
 * this function starts the parsing process. It expects a complete expression followed by a semicolon.
 * Returns the result of the expression if it's valid, otherwise returns ERROR.
 */
int bexpr(char *token){
    char *tempToken = token;
    int result = expr(&tempToken);

    if (result == ERROR) {
        return ERROR; // If the expression is invalid, return error
    
    }

    // Check for the semicolon after the expression
    if (*tempToken != ';') {
        return MISSING_SEMICOLON;
    }

    // Move past the semicolon
    tempToken++;

    // Check if the expression ends after the semicolon
    if (*tempToken != '\0') {
//        fprintf(stderr, "Syntax Error: Unexpected characters after semicolon\n");
        return ERROR;
    }

    printf("expression returned %d", result);

    return result;
}

/**
 * expr - parses the <expr> non-terminal of the grammar.
 * @expr: pointer to the string containing the expression to parse.
 *
 * this function parses an expression, which consists of a term and an optional tail (ttail).
 * Returns the computed value of the term combined with any additional terms found in the tail.
 */
int expr(char **expr) {
    int term_val = term(expr);
    if (term_val == ERROR) 
        return ERROR;
    
    int result = ttail(expr, term_val);

    return result;


   
}

/**
 * ttail - parses the <ttail> non-terminal of the grammar.
 * @expr: pointer to the string containing the expression to parse.
 * @acc: accumulated value from previous terms.
 *
 * This function recursively processes a series of terms connected by addition or subtraction.
 * Returns the cumulative value of these terms.
 */
int ttail(char **expr, int acc) {


    char op = add_sub_tok(expr);

    while (op != '\0') {
        int term_val = term(expr);
        if (term_val == ERROR){
            fprintf(stderr, "Error in ttail: term function returned ERROR\n");
            return ERROR;
        }

        if (op == '+') {
            acc += term_val; 
        } else if (op == '-') {
            acc -= term_val; 
        }

        op = add_sub_tok(expr);

    }
    return acc;

}

/**
 * term - parses the <term> non-terminal of the grammar.
 * @expr: pointer to the string containing the expression to parse.
 *
 * this function parses a term, which consists of a statement and an optional tail (stail).
 * Returns the computed value of the statement.
 */
int term(char **expr) {
    int stmt_val = stmt(expr);
    if (stmt_val == ERROR) {
        return ERROR;
    }
    return stail(expr, stmt_val);
}

/**
 * stail - parses the <stail> non-terminal of the grammar.
 * @expr: pointer to the string containing the expression to parse.
 * @acc: accumulated value from previous statements.
 *
 * this function recursively processes a series of statements connected by multiplication or division.
 * Returns the cumulative value of these statements.
 */
int stail(char **expr, int acc) {
    char op = mul_div_tok(expr);

    while (op != '\0') {
        int stmt_val = stmt(expr);        
        
        if (stmt_val == ERROR) {
            return ERROR;
        }

        if (op == (char)ERROR) {
            return ERROR;
        }

        if (op == '*') {
            acc *= stmt_val;
        } else if (op == '/') {
           if (stmt_val == 0) {
                fprintf(stderr, "Runtime Error: Division by zero.\n");
                return ERROR;
            }
            acc /= stmt_val;
        }

        op = mul_div_tok(expr);
    }

    return acc;
}


/**
 * stmt - parses the <stmt> non-terminal of the grammar.
 * @expr: pointer to the string containing the expression to parse.
 *
 * this function parses a statement, which consists of a factor and an optional tail (ftail).
 * Returns the computed value of the factor.
 */
int stmt(char **expr) {

    int factor_val = factor(expr);

    if (factor_val == ERROR) {
        fprintf(stderr, "Error in stmt: factor returned ERROR\n");
        return ERROR;
    }

    int result = ftail(expr, factor_val);

    return result;
}

/**
 * ftail - parses the <ftail> non-terminal of the grammar.
 * @expr: pointer to the string containing the expression to parse.
 * @acc: accumulated value from previous factors.
 *
 * function recursively processes a series of factors connected by comparison operators.
 * Returns the boolean result of these comparisons.
 */
int ftail(char **expr, int acc) {
    char* comp_op = compare_tok(expr);
    if (comp_op == NULL) {
        return acc;
    }
    
    int factor_val = factor(expr); 
    if (factor_val == ERROR) {
        fprintf(stderr, "Error in ftail: factor returned ERROR\n");
        return ERROR;
    }

    int result;
    if (strcmp(comp_op, "<") == 0) {
        result =  acc < factor_val;
    } else if (strcmp(comp_op, ">") == 0) {
        result = acc > factor_val;
    } else if (strcmp(comp_op, "<=") == 0) {
        result = acc <= factor_val;
    } else if (strcmp(comp_op, ">=") == 0) {
        result = acc >= factor_val;
    } else if (strcmp(comp_op, "!=") == 0) {
        result = acc != factor_val;
    } else if (strcmp(comp_op, "==") == 0) {
        result = acc == factor_val;
    }else{
        fprintf(stderr, "Runtime Error: Invalid comparison operator.\n");
        return ERROR;
    }

    return ftail(expr, result);

}

/**
 * factor - parses the <factor> non-terminal of the grammar.
 * @expr: pointer to the string containing the expression to parse.
 *
 * This function parses a factor, which is an exponentiated expression or an expp.
 * Returns the computed value of the exponentiation.
 */
int factor(char **expr) {
    int base = expp(expr); 

    if (base == ERROR) {
        return ERROR;
    }

    
    while (isspace(**expr)) (*expr)++; 
    
    if (**expr == '^') {

        (*expr)++; 
        while (isspace(**expr)) (*expr)++;

        int exponent = factor(expr); 

        if (exponent == ERROR || exponent < 0) {
            return ERROR;
        }


        if (exponent > (log(INT_MAX) / log(base)) && base != 0 && base != 1) {
            fprintf(stderr, "Error: Exponentiation overflow.\n");
            return ERROR;
        }
        
        errno = 0;
        double result = pow((double)base, (double)exponent);

        if (errno != 0 || result > INT_MAX || result < INT_MIN) { 
            fprintf(stderr, "Error: Exponentiation result out of int range.\n");
            return ERROR;
        }
        return (int)result;
    }

    return base;
}

/**
 * expp - parses the <expp> non-terminal of the grammar.
 * @current_expr: pointer to the string containing the expression to parse.
 *
 * function parses an expp, which is either a parenthesized expression or a number.
 * Returns the computed value of the parenthesized expression or the number.
 */
int expp(char **current_expr) {
    while (isspace(**current_expr)) (*current_expr)++;

    if (**current_expr == '(') { 
        (*current_expr)++;

        int value = expr(current_expr); 

       // if (value == ERROR || value == MISSING_CLOSING_PARENTHESIS) {
            //fprintf(stderr, "Error: Malformed expression inside parentheses.\n");
      //      return value; 
     //   }

        if (**current_expr != ')') {
            return MISSING_CLOSING_PARENTHESIS;
        }
        (*current_expr)++; // Consume the closing parenthesis
        while (isspace(**current_expr)) (*current_expr)++;
   
        return value;
    } else {
        return num(current_expr); 

    }
}

/**
 * add_sub_tok - identifies addition and subtraction tokens.
 * @expr: pointer to the string containing the expression to parse.
 *
 * Returns the addition or subtraction character if found, otherwise returns a null character.
 */
char add_sub_tok(char **expr) {
    while (isspace(**expr)) (*expr)++;

    char op = **expr;

    if (op == '+' || op == '-') {
        (*expr)++;
        return op;
    }


    return '\0';
}

/**
 * mul_div_tok - identifies multiplication and division tokens.
 * @expr: pointer to the string containing the expression to parse.
 *
 * Returns the multiplication or division character if found, otherwise returns a null character.
 */
char mul_div_tok(char **expr) {
    while (isspace(**expr)) (*expr)++; 

    char op = **expr;
    if (op == '*' || op == '/') {
        (*expr)++;
        return op; 
    }

    return '\0';
}

/**
 * compare_tok - identifies comparison tokens.
 * @expr: pointer to the string containing the expression to parse.
 *
 * Returns a pointer to the comparison operator if found, otherwise returns NULL.
 */
char* compare_tok(char **expr) {

    while (isspace(**expr)) (*expr)++; 

    char first_char = **expr;

    if (first_char == '<' || first_char == '>' || first_char  == '!' || first_char == '=') {
        char second_char = *(*expr + 1);

        static char result[3] = {'\0', '\0', '\0'};

        if (second_char == '=' || (first_char == '!' && second_char == '=')) {
            result[0] = first_char;
            result[1] = second_char;
            *expr += 2;
            return result; 
        }else if (first_char == '<' || first_char == '>') {
            result[0] = first_char;
            *expr += 1;

            return result;
        }
    }

    return NULL;
}

/**
 * num - parses the <num> non-terminal of the grammar.
 * @expr: pointer to the string containing the expression to parse.
 *
 * this function parses a number, handling potential sign prefixes.
 * Returns the parsed number as an integer.
 */
int num(char **expr) {
    int sign = 1; 
    while (isspace(**expr)) (*expr)++;

    if (**expr == '+' || **expr == '-') {
        sign = (**expr == '-') ? -1 : 1;
        (*expr)++;

        // After consuming a sign, there should be no space before the number       
        if (isspace(**expr)) {
            //fprintf(stderr, "Syntax error: unexpected space after sign\n");
            return ERROR; // Syntax error due to space after sign                   
        }
    }

   
    char *next;
    errno = 0;
    long value = strtol(*expr, &next, 10);  

    if (*expr == next) {
        fprintf(stderr, "Syntax error: no digits found\n");
        return ERROR;  // No digits were parsed
    } else if (errno == ERANGE) {
        fprintf(stderr, "Error: number out of range\n");
        return ERROR;  // Number out of valid long range
    }

    *expr = next;
    return (int)value * sign;
}
