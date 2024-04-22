#ifndef PARSER_H
#define PARSER_H

#define ERROR -999999
#define MISSING_SEMICOLON -999998
#define MISSING_CLOSING_PARENTHESIS -999997

int bexpr(char *token);
int expr(char **expr);                                                          
int ttail(char **expr, int acc);                                                
int term(char **expr);                                                          
int stail(char **expr, int acc);                                                
int stmt(char **expr);                                                          
int ftail(char **expr, int acc);                                                
int factor(char **expr);                                                        
int expp(char **current_expr);                                                          
char add_sub_tok(char **expr);                                                  
char mul_div_tok(char **expr);                                                  
char* compare_tok(char **expr);                                                 
int num(char **expr);

#endif // PARSER_H
