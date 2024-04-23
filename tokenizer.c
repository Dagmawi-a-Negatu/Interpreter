
/**                                                                             
 * tokenizer.c - A simple token recognizer.                                     
 * This program tokenizes given input based on regular expressions.                                                                            
 * NOTE: The terms 'token' and 'lexeme' are used interchangeably in this        
 *       program.                                                               
 *                                                                              
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
char *line; // Global pointer to the line of input                                                                                       
FILE  *out_file = NULL; // File pointer for output file                                                         
int count; //Global variable that counts lexemes                                                    
                                                                                
// Function prototypes                                                          
void get_token(char *token_ptr);                                                
                                                                                
/**                                                                             
 * Main function of the tokenizer.                                              
 * Opens the input and output files, reads the input file                       
 * line by line, and processes each line.                                       
 *                                                                              
 * @param argc Number of command line arguments                                 
 * @param argv Array of command line arguments                                  
 * @return Exit status code                                                     
 */                                                                             
int main(int argc, char *argv[])                                                
{                                                                               
  char token[TSIZE];     /* Spot to hold a token, fixed size */                 
  char input_line[LINE]; /* Line of input, fixed size        */                 
  FILE *in_file = NULL;  /* File pointer                     */                 
  int start = 0;                                                                
                                                                                
                                                                                
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
                                                                                
  // (optional) can add some code here if you want some here                    
                                                                                
  count = 0; //lexeme count is 0                                                                              
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
   * Tokenizes a given string based on predefined                               
   * regular expressions.                                                       
   *                                                                            
   * It finds all the tokens in the given string and                            
   * prints them to the output file.                                            
   *                                                                            
   * @param token_ptr Pointer to the string to be tokenized                     
   */                                                                           
 void get_token(char *token_ptr, FILE* out_file)                                                
 {                                                                              
                                                                                
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
    int  statement = 0;                                                         
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
            //process_nonmatch(out_file, nonmatch);                             
            report_lexical_error(out_file, nonmatch);                           
                                                                                
                                                                                
         }                                                                      
         char substring[128]; // Buffer to hold the matched substring           
         int substring_length = ovector[1] - ovector[0];                        
         strncpy(substring, token_ptr  + ovector[0], substring_length);         
         substring[substring_length] = '\0';                                    
                                                                                
         if(strcmp(substring, ";") == 0){                                       
             statement = 1;                                                     
         }                                                                      
         //process_lexeme(substring);                                           
         printf("This is Lexeme: %s\n", substring);                             
                                                                                
         previous_end = ovector[1];                                             
         i = ovector[1]; // Update position to continue searching                
                                                                                
    }                                                                           
                                                                                
                                                                               
                                                                                
    //process_last_match(token_ptr, previous_end, out_file);                    
                                                                                
    // Free up the regular expression**/                                         
    pcre_free(re);                                                              
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

void report_lexical_error(FILE* out_file, const char* error_text) {
    fprintf(out_file, "Lexical error: %s\n", error_text);
}

