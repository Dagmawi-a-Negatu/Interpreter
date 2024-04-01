/**
 *
 * tokenizer.c - A simple token recognizer. 
 * 
 * This program tokenizes textual input based on predefined patterns, extracting
 * meaningful lexemes such as operators, literals, and symbols. It uses PCRE (Perl
 * Compatible Regular Expressions) to match patterns and categorize each lexeme.
 * The tokenizer supports various token categories including arithmetic operators,
 * assignment operators, parentheses, and integer literals. Unrecognized tokens
 * are flagged as lexical errors, enhancing the debugging process during lexical
 * analysis.
 * 
 * The program processes input line-by-line, tokenizes each line into lexemes, and
 * categorizes them according to the defined patterns. It handles non-matching
 * segments and whitespace, ensuring comprehensive analysis of the input text. 
 * 
 * @file tokenizer.c
 * @brief Implements tokenization logic for analyzing and categorizing textual input.
 * @author Dagmawi Negatu and Darwin Bueso Galdamez
 * @version 03/16/2024
 */                                                                            
                                                                                
#include <stdio.h>                                                              
#include <string.h>                                                             
#include <stdlib.h>                                                             
#include <pcre.h>                                                               
#include <ctype.h>                                                              
#include "tokenizer.h"                                                          
#include <stdbool.h>
                                                                                
// global variables                                                             
char *line; // Global pointer to line of input                                  
// (optional) can declare some additional variable if you want to               
//Output file meant to hold the writing of extracted lexemes                                                                        
FILE  *out_file = NULL;                                                         
                                                   
                                                                                                                                
                                                                                
/**
 * The main function serves as the entry point for the tokenizer program. 
 * It takes two command line arguments: the path to the input file and the 
 * path to the output file. The program reads from the input file line by 
 * line, tokenizes each line using regular expressions to identify meaningful 
 * lexemes, and writes the results to the output file. Each lexeme is 
 * categorized, and its details are logged for further processing. 
 * 
 * This function also performs error checking to ensure both input and output 
 * files can be successfully opened. If an error occurs, it prints a relevant 
 * error message and terminates execution with an exit status of 1. 
 * This approach ensures that users are immediately aware of any issues with 
 * file access, allowing for quick troubleshooting.
 * 
 * 
 * @param argc The number of command-line arguments passed to the program.
 * @param argv An array of pointers to the strings which are those 
 * command-line arguments. Expected arguments are the paths to the input 
 * file and the output file, in that order.
 * 
 * @return Returns 0 upon successful execution to indicate to the operating 
 * system that the program has completed without errors. Returns 1 if an error 
 * occurs, signaling an unsuccessful execution.
 */                                                                             
int main(int argc, char *argv[])                                                
{                                                                                
  char input_line[LINE]; /* Line of input, fixed size        */                 
  FILE *in_file = NULL;  /* File pointer                     */                                                                              
                                                                                
                                                                                
  if (argc != 3)                                                                
  {                                                                             
    printf("Usage: tokenizer inputFile outputFile\n");                          
    exit(1);                                                                    
  }                                                                             
                                                                                
  in_file = fopen(argv[1], "r");                                                
  if (in_file == NULL)                                                          
  {                                                                             
    fprintf(stderr, "ERROR: could not open %s for reading\n", argv[1]);         
    exit(1);                                                                    
  }                                                                             
                                                                                
  out_file = fopen(argv[2], "w");                                               
  if (out_file == NULL)                                                         
  {                                                                             
    fprintf(stderr, "ERROR: could not open %s for writing\n", argv[2]);         
    exit(1);                                                                    
  }                                                                             
                                                                                                                                                           
  while (fgets(input_line, LINE, in_file) != NULL)                              
  {                                                                             
    // Add code here. Keep this file no longer than 50 lines of code.           
    // Use helper functions.                                                    
    line = input_line;                                                          
   // start = TRUE;                                                             
                                                       
    get_token(line);                                                            
    // Output the statement number                                                                                                                           
                                                                                
                                                                                
  }                                                                             
                                                                                
  fclose(in_file);                                                              
  fclose(out_file);                                                             
  return 0;                                                                     
                                                                                
}                                                                               
  /**
 * Tokenizes the input string into recognized tokens using predefined regular 
 * expressions. Each token found is categorized and processed, and details are 
 * printed to the output file. The function handles both matching tokens and 
 * segments of text that do not match any token patterns, ensuring comprehensive 
 * analysis of the input. Non-matching segments can indicate potential errors or 
 * unrecognized symbols. After tokenizing, it also addresses any remaining text 
 * post-last recognized token for completeness.
 *
 * Utilizes PCRE for efficient pattern matching, crucial for parsing in compilers 
 * and interpreters.
 *
 * @param token_ptr Pointer to the input string to be tokenized.
 */                                                                            
 void get_token(char *token_ptr)                                                
 {                                                                             
    // Add code here. Keep this file no longer than 50 lines of code.           
    // required variables by the perl compatiable expression                                
    const char *error;                                                       
    int erroffset;                                                              
    pcre *re;                                                                   
    int ovector[30], rc;                                                                     
    // Pattern to match numbers, single and double character operators          
    char *pattern = "(\\d+|!=|==|<=|>=|[=+\\-*/^<>();])";                
                                                                                
    // Compile the regular expression pattern                                   
    re = pcre_compile(pattern, 0, &error, &erroffset, NULL);                    
                                                                                
    if (re == NULL)                                                             
    {                                                                           
      printf("PCRE compilation failed at offset %d: %s\n", erroffset, error);   
      exit(EXIT_FAILURE);                                                       
    }                                                                                  
    // Execute the regular expression and find matches                          
    int i = 0;
    int previous_end = 0; // Track end of the last match
    while ((rc = pcre_exec(re, NULL, token_ptr, strlen(token_ptr), i, 0,
        ovector, sizeof(ovector)/sizeof(ovector[0]))) >= 0) {
         // Handle non-matching segment before this match
        if (ovector[0] > previous_end && isNotWhitespaceOrTab(token_ptr +
            previous_end)) {
        // There's a non-matching segment before this match
            int nonmatch_length = ovector[0] - previous_end;
            char nonmatch[128]; // Ensure this is adequately sized
            strncpy(nonmatch, token_ptr + previous_end, nonmatch_length);
            nonmatch[nonmatch_length] = '\0';

        // Process non-matching segment (e.g., log error, handle as invalid input)
            process_nonmatch(out_file, nonmatch);
        }
         
         //Extracts the lexeme found starting from the first lexeme
         char substring[TSIZE]; // Buffer to hold the matched substring           
         int substring_length = ovector[1] - ovector[0];                        
         strncpy(substring, token_ptr  + ovector[0], substring_length);         
         substring[substring_length] = '\0';                       
         //We have found a lexeme, process the lexeme to the output file                                                                             
         process_lexeme(substring);

         previous_end = ovector[1];
         i = ovector[1]; // Update position to continue searching                
                                                                                                                                                       
    }
                                                                                
    //Process if there is a possible unmatched character ending a statement
    process_last_match(token_ptr, previous_end, out_file);


   // Free up the regular expression**/                                         
  pcre_free(re);                                                                
}                

/**
 * Logs non-matching segments identified during tokenization.
 * These segments could indicate input errors or unrecognized characters. 
 * By logging them, this function aids in debugging and refining the 
 * tokenization process. It leverages theprocess_matching function to 
 * further analyze and categorize the non-matching segment if possible.
 *
 * @param out_file File pointer for logging the non-matching segments.
 * @param unmatch The text of the non-matching segment.
 */
void process_nonmatch(FILE *out_file, char *unmatch) {
    fprintf(out_file, "===>  '%s'\n", unmatch);
    process_matching(out_file, unmatch);
}

/**
 * Processes and logs the text following the last matched token, if any.
 * This function aims to identify and handle any remaining text that was 
 * not captured by the main tokenization patterns. It focuses on non-whitespace 
 * segments, indicating potentially unrecognized or incorrectly formatted input, 
 * and logs these for further analysis.
 *
 * @param token_ptr Pointer to the input text.
 * @param previous_end Position at which the last token was recognized.
 * @param out_file Output file for logging any findings.
 */
void process_last_match(char *token_ptr, int previous_end, FILE *out_file) {
    if (strlen(token_ptr) > previous_end) {
        const char *unmatched = token_ptr + previous_end;
        
        // Create a buffer to copy non-whitespace content
        char nonWhitespaceSegment[128] = {0}; // Adjust size as needed
        int j = 0;

        // Copy only non-whitespace characters to the buffer
        for (int i = 0; unmatched[i] != '\0'; ++i) {
            if (!isspace((unsigned char)unmatched[i])) {
                nonWhitespaceSegment[j++] = unmatched[i];
            }
        }
        nonWhitespaceSegment[j] = '\0'; // Null-terminate the string

        // Only process if there's non-whitespace content
        if (j > 0) {
            process_nonmatch(out_file, nonWhitespaceSegment);
        }
    }
}



/**                                                                             
 * Determines if a character is neither a space nor a tab.                      
 *                                                                              
 * @param c Pointer to the character being checked.                             
 * @return Returns 1 if not a space/tab, 0 otherwise.                           
 */
int isNotWhitespaceOrTab(char* c) {
    return *c != ' ' && *c != '\t';
}

/**
 * This function processes each lexeme found within the input text, prints 
 * its details, and checks for statement delimiters (semicolons). Upon 
 * encountering a semicolon, it signifies the end of the current statement 
 * and the potential start of a new one. Consequently, the function increments 
 * the statement counter and resets the lexeme counter to prepare for the next 
 * statement.
 * 
 * Each new statement is introduced with a header for clarity. Between statements, 
 * a separator line is printed for better visual separation in the output file.
 *
 * @param lexeme The current lexeme to be processed and printed.
 */
void process_lexeme(char* lexeme) {
    static int statementCount = 1;
    static int lexemeCount = 0;
    static bool needToPrintStatementHeader = true;

    // Check if we need to print the statement header
    if (needToPrintStatementHeader) {
        fprintf(out_file, "Statement #%d\n", statementCount++);
        needToPrintStatementHeader = false;
        lexemeCount = 0;
    }

    // Print the lexeme
    fprintf(out_file, "Lexeme %d is %s",lexemeCount++, lexeme);
    process_matching(out_file, lexeme); 

    // If the lexeme is a semicolon, the next lexeme will be part of a new statement
    if (strcmp(lexeme, ";") == 0) {
        fprintf(out_file, "---------------------------------------------------------\n");
        needToPrintStatementHeader = true;
    }
}


/**
 * This function categorizes a given lexeme into one of the predefined 
 * token categories, such as arithmetic operators, parentheses, comparison 
 * operators, or literals. It facilitates the semantic analysis of tokens 
 * by providing a uniform way to refer to the types of tokens encountered 
 * in the input text. 
 * 
 * If a lexeme does not match any of the predefined categories, it is 
 * categorized as UNKNOWN, which may indicate an unrecognized token or 
 * lexical error.
 *
 * @param lexeme The lexeme to be evaluated and categorized.
 * @return A TokenCategory enumeration value representing the category of 
 * the lexeme.
 */
TokenCategory match_token_category(char* lexeme) {
    if (strcmp(lexeme, "+") == 0) return ADD_OP;
    else if (strcmp(lexeme, "-") == 0) return SUB_OP;
    else if (strcmp(lexeme, "*") == 0) return MULT_OP;
    else if (strcmp(lexeme, "/") == 0) return DIV_OP;
    else if (strcmp(lexeme, "(") == 0) return LEFT_PAREN;
    else if (strcmp(lexeme, ")") == 0) return RIGHT_PAREN;
    else if (strcmp(lexeme, "^") == 0) return EXPON_OP;
    else if (strcmp(lexeme, "=") == 0) return ASSIGN_OP;
    else if (strcmp(lexeme, "<") == 0) return LESS_THEN_OP;
    else if (strcmp(lexeme, "<=") == 0) return LESS_THEN_OR_EQUAL_OP;
    else if (strcmp(lexeme, ">") == 0) return GREATER_THEN_OP;
    else if (strcmp(lexeme, ">=") == 0) return GREATER_THEN_OR_EQUAL_OP;
    else if (strcmp(lexeme, "==") == 0) return EQUALS_OP;
    else if (strcmp(lexeme, "!") == 0) return NOT_OP;
    else if (strcmp(lexeme, "!=") == 0) return NOT_EQUALS_OP;
    else if (strcmp(lexeme, ";") == 0) return SEMI_COLON;
    else if (lexeme[0] >= '0' && lexeme[0] <= '9') return INT_LITERAL; // Simplistic check for an integer literal
    else return UNKNOWN;
}

/**
 * This function processes each lexeme, identifies its token category, 
 * and prints a descriptive statement to the output file indicating the 
 * category of the lexeme. 
 * 
 * It uses the categorization provided by `match_token_category` to apply 
 * the appropriate label. If the lexeme does not match any predefined category, 
 * indicating a potential lexical error or an unrecognized token, an error message 
 * is printed to alert the user.
 *
 * @param file Pointer to the file where lexeme analysis results are logged.
 * @param lexeme String representing the lexeme to be analyzed and categorized.
 */
void process_matching(FILE *file, char* lexeme) {
    TokenCategory category = match_token_category(lexeme);
    switch (category) {
        case ADD_OP: fprintf(file, " and is an ADD_OP\n"); break;
        case SUB_OP: fprintf(file, " and is a SUB_OP\n"); break;
        case MULT_OP: fprintf(file, " and is a MULT_OP\n"); break;
        case DIV_OP: fprintf(file, " and is a DIV_OP\n"); break;
        case LEFT_PAREN: fprintf(file, " and is a LEFT_PAREN\n"); break;
        case RIGHT_PAREN: fprintf(file, " and is a RIGHT_PAREN\n"); break;
        case EXPON_OP: fprintf(file, " and is an EXPON_OP\n"); break;
        case ASSIGN_OP: fprintf(file, " and is an ASSIGN_OP\n"); break;
        case LESS_THEN_OP: fprintf(file, " and is a LESS_THEN_OP\n"); break;
        case LESS_THEN_OR_EQUAL_OP: fprintf(file, " and is a LESS_THEN_OR_EQUAL_OP\n"); break;
        case GREATER_THEN_OP: fprintf(file, " and is a GREATER_THEN_OP\n"); break;
        case GREATER_THEN_OR_EQUAL_OP: fprintf(file, " and is a GREATER_THEN_OR_EQUAL_OP\n"); break;
        case EQUALS_OP: fprintf(file, " and is an EQUALS_OP\n"); break;
        case NOT_OP: fprintf(file, " and is a NOT_OP"); break;
        case NOT_EQUALS_OP: fprintf(file, " and is a NOT_EQUALS_OP\n"); break;
        case SEMI_COLON: fprintf(file, " and is a SEMI_COLON\n"); break;
        case INT_LITERAL: fprintf(file, " and is an INT_LITERAL\n"); break;
        default: fprintf(file, "Lexical error: not a lexeme\n");
    }
}
