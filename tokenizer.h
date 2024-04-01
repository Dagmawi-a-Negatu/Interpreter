/**
 * @file tokenizer.h
 * @brief Header file for the tokenizer project, which provides functionality for tokenizing
 * textual input into recognizable tokens (lexemes) according to specified patterns. It includes
 * definitions for maximum line lengths, token sizes, boolean values, and token categories. This
 * file declares functions for processing tokens, handling unmatched segments, and categorizing
 * lexemes. The tokenizer is capable of distinguishing various types of operators, parentheses,
 * integer literals, and identifies unrecognized segments as unknown tokens. It supports detailed
 * token analysis, including identifying token categories and handling lexical errors. This header
 * facilitates the implementation of a lexical analyzer as part of a compiler or interpreter for
 * programming languages or data formats.
 *
 * @author Dagmawi Negatu and Darwin Bueso Galdamez
 * @version 03/16/2024
 */

/* Constants */
#define LINE 100
#define TSIZE 20
#define TRUE 1
#define FALSE 0

    
/**                                                                             
 * Token categories enumeration.                                                
 * Defines various categories of tokens that the tokenizer can recognize,       
 * including operators, parentheses, literals, and more.                        
 */
typedef enum {
    ADD_OP, SUB_OP, MULT_OP, DIV_OP,
    LEFT_PAREN, RIGHT_PAREN, EXPON_OP, ASSIGN_OP,
    LESS_THEN_OP, LESS_THEN_OR_EQUAL_OP, GREATER_THEN_OP, GREATER_THEN_OR_EQUAL_OP,
    EQUALS_OP, NOT_OP, NOT_EQUALS_OP, SEMI_COLON,
    INT_LITERAL, UNKNOWN
} TokenCategory;


/**                                                                             
 * Determines if a character is neither a space nor a tab.                      
 *                                                                              
 * @param c Pointer to the character being checked.                             
 * @return Returns 1 if not a space/tab, 0 otherwise.                           
 */
int isNotWhitespaceOrTab(char *);


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
void process_last_match(char *, int , FILE *);

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
void process_nonmatch(FILE *out_file, char *token_ptr);

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
void get_token(char *);

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
void process_lexeme(char*);

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
TokenCategory match_token_category(char *);

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
void process_matching(FILE *, char * );
