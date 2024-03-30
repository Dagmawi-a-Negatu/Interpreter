/*
 * parser.c - recursive descent parser for a simple expression language.
 * Most of the functions in this file model a non-terminal in the
 * grammar listed below
 * Author: William Kreahling and Mark Holliday
 * Date:   Modified 9-29-08 and 3-25-15 and 14 april 2020
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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
 * <expr> -> <term> <ttail>
 * The function for the non-terminal <expr> that views
 * the expression as a series of terms and addition and
 * subtraction operators.
 * @param token: the line being read
 * @return: the number of the evaluated expression or an error
 */
int expr(char *token)
{
   int subtotal = term(token);
   if (subtotal == ERROR)
      return subtotal;
   else
      return ttail(token, subtotal);
}

/**
 * <ttail> -> <add_sub_tok> <term> <ttail> | e
 * The function for the non-terminal <ttail> that is the
 * the rest of an arithmetic expression after the initial
 * term. So it expects an addition or subtraction operator
 * first or the empty string. 
 * @param token: the line being read
 * @param subtotal: the number we have evaluated up to this
 *                  point
 * @return: the number of the evaluated expression or an error
 */
int ttail(char *token, int subtotal)
{
   int term_value;

   if (!strncmp(token, "+", 1))
   {
      add_sub_tok(token);
      term_value = term(token);

      // if term returned an error, give up otherwise call ttail
      if (term_value == ERROR)
         return term_value;
      else
         return ttail(token, (subtotal + term_value));
   }
   else if(!strncmp(token, "-", 1))
   {
      add_sub_tok(token);
      term_value = term(token);

      // if term returned an error, give up otherwise call ttail
      if (term_value == ERROR)
         return term_value;
      else
         return ttail(token, (subtotal - term_value));
   }
   /* empty string */
   else
      return subtotal;
}


