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

int bexpr(char *token) {
    return expr(&token);
}

int expr(char **expr) {
    int term_val = term(expr);
    if (term_val == ERROR) 
        return ERROR;
    
    return ttail(expr, term_val);

   
}

int ttail(char **expr, int acc) {
   char op = add_sub_tok(expr); 

    while (op != '\0') {
        int term_val = term(expr);
        if (term_val == ERROR) return ERROR;

        if (op == '+') {
            acc += term_val; 
        } else if (op == '-') {
            acc -= term_val; 
        }

        op = add_sub_tok(expr);
    }

    if (**expr == ';' || **expr == '\0') {                                      
        if (**expr == ';') {                                                    
            (*expr)++;                                                          
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

int stail(char **expr, int acc) {
    char op = mul_div_tok(expr);

    while (op != '\0') {
        int stmt_val = stmt(expr);        

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
        return ERROR;
    }

    return ftail(expr, factor_val);
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


        if (exponent == ERROR) {
            return ERROR;
        }

       if (exponent < 0) {
            return ERROR;
        }

        if ((base != 0 && base != 1) && exponent > (log(INT_MAX) / log(base))) {
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
       // printf("Entering parentheses\n"); test prints
        (*current_expr)++;

        int value = expr(current_expr); 
        // printf("Value inside parentheses: %d\n", value); test prints
        if (value == ERROR) {
           // printf("Error inside parentheses\n");
            return ERROR; 
        }

 
        if (**current_expr != ')') {
            fprintf(stderr, "Syntax Error: Missing closing parenthesis.\n");
            return ERROR;  
        }

        (*current_expr)++; 
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

        if (second_char == '=') {
            static char result[3];
            result[0] = first_char;
            result[1] = second_char;
            result[2] = '\0';
            *expr += 2; 
            return result; 
        }else if (first_char == '<' || first_char == '>') {
            static char result[2];
            result[0] = first_char;
            result[1] = '\0';
            *expr += 1;
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
            return ERROR; // Syntax error due to space after sign                   
        }
    }

   
    char *next;
    errno = 0;
    long value = strtol(*expr, &next, 10);  

    if (*expr == next || errno == ERANGE){
        return ERROR;
    }

    *expr = next;
    return (int)value * sign;
}
