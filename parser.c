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
#include <limits.h>
#include <math.h>
#include "tokenizer.h"
#include "parser.h"

#define ERROR -999999

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

int bexpr(char *token){
    char *tempToken = token;
    int result = expr(&tempToken);

    if (result == ERROR) {
        return ERROR; // If the expression is invalid, return error
    
    }

    // Check for the semicolon after the expression
    if (*tempToken != ';') {
        fprintf(stderr, "Syntax Error: Expression must end with a semicolon\n");
        return ERROR;
    }

    // Move past the semicolon
    tempToken++;

    // Check if the expression ends after the semicolon
    if (*tempToken != '\0') {
        fprintf(stderr, "Syntax Error: Unexpected characters after semicolon\n");
        return ERROR;
    }

    return result;
}

int expr(char **expr) {
//    printf("Entering expr with current_expr: %s\n", *expr); // Debug print
    int term_val = term(expr);
//    printf("Returned from term with value: %d, current_expr: %s\n", term_val, *expr); // Debug print
    if (term_val == ERROR) 
        return ERROR;
    
    int result = ttail(expr, term_val);
  //  printf("Returned from ttail with value: %d, current_expr: %s\n", result, *expr); // Debug print

    return result;


   
}

int ttail(char **expr, int acc) {


    char op = add_sub_tok(expr);

    while (op != '\0') {
      //  printf("Processing ttail term for operation '%c'\n", op);

        int term_val = term(expr);
        if (term_val == ERROR){
            fprintf(stderr, "Error in ttail: term function returned ERROR\n");
            return ERROR;
        }

       // printf("Term value obtained in ttail: %d\n", term_val);

        if (op == '+') {
            acc += term_val; 
        } else if (op == '-') {
            acc -= term_val; 
        }

        op = add_sub_tok(expr);
//        printf("Next operation found in ttail: '%c'\n", op);
    }
    return acc;

}

int term(char **expr) {
    int stmt_val = stmt(expr);
    if (stmt_val == ERROR) {
        return ERROR;
    }
    return stail(expr, stmt_val);
}

int stail(char **expr, int acc) {
    char op = mul_div_tok(expr);

    while (op != '\0') {
        int stmt_val = stmt(expr);        
        
        if (stmt_val == ERROR) {
        // If stmt returns ERROR, the loop should break and stail should return ERROR
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



int stmt(char **expr) {

    int factor_val = factor(expr);

    if (factor_val == ERROR) {
        fprintf(stderr, "Error in stmt: factor returned ERROR\n");
        return ERROR;
    }

    int result = ftail(expr, factor_val);

    return result;
}


int ftail(char **expr, int acc) {
    char* comp_op = compare_tok(expr);
    if (comp_op == NULL) {
     
        return acc;
    }

    if (strlen(comp_op) == 2) {
        (*expr) += 2;
    } else {
        (*expr) += 1;
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

int expp(char **current_expr) {
    while (isspace(**current_expr)) (*current_expr)++;

    if (**current_expr == '(') { 
        (*current_expr)++;

        int value = expr(current_expr); 

        if (value == ERROR) {
            fprintf(stderr, "Error: Malformed expression inside parentheses.\n");
            return ERROR; 
        }

        if (**current_expr != ')') {
            fprintf(stderr, "Syntax Error: Expected ')' but got '%c'\n", **current_expr);
            return ERROR;
        }
        (*current_expr)++; // Consume the closing parenthesis
        while (isspace(**current_expr)) (*current_expr)++;
   
        return value;
    } else {
        return num(current_expr); 

    }
}


char add_sub_tok(char **expr) {
    while (isspace(**expr)) (*expr)++;

    char op = **expr;

    if (op == '+' || op == '-') {
        (*expr)++;
        return op;
    }


    return '\0';
}

char mul_div_tok(char **expr) {
    while (isspace(**expr)) (*expr)++; 

    char op = **expr;
    if (op == '*' || op == '/') {
        (*expr)++;
        return op; 
    }

    return '\0';
}

char* compare_tok(char **expr) {
    while (isspace(**expr)) (*expr)++; 

    char first_char = **expr;
   
    if (first_char == '<' || first_char == '>' || first_char  == '!' || first_char == '=') {
        char second_char = *(*expr + 1);
        
        static char result[3];
        result[0] = first_char;
        result[1] = second_char;
        result[2] = '\0';

        if (second_char == '=') {
            *expr += 2; 
            return result; 
        }else if (first_char == '<' || first_char == '>') {
            *expr += 1;
            result[1] = '\0';
            return result;
        }
    }

    return NULL;
}


int num(char **expr) {
    int sign = 1; 
    while (isspace(**expr)) (*expr)++;

    if (**expr == '+' || **expr == '-') {
        sign = (**expr == '-') ? -1 : 1;
        (*expr)++;

        // After consuming a sign, there should be no space before the number       
        if (isspace(**expr)) {
            fprintf(stderr, "Syntax error: unexpected space after sign\n");
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
