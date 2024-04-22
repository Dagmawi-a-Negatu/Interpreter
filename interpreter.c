/* 
 * The interpreter.c file contains the main function that drives the arithmetic expression
 * interpreter. It uses the parsing functions defined in parser.h to evaluate expressions read
 * from an input file and writes the results to an output file.
 */

#include <stdio.h>
#include <stdlib.h>
#include "parser.h"


/**
 * main - the entry point of the interpreter.
 * @argc: the number of command-line arguments.
 * @argv: the array of command-line arguments.
 *
 * This function checks command-line arguments, opens the input and output files, and processes
 * each line of the input file. It prints whether the syntax is OK and, if so, the value of the
 * evaluated expression. It handles file opening/closing and memory deallocation.
 *
 * Returns 0 on success, or 1 on error such as invalid arguments or file access issues.
 */
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <inputfile> <outputfile>\n", argv[0]);
        return 1;
    }

    FILE *inputFile = fopen(argv[1], "r");
    FILE *outputFile = fopen(argv[2], "w");

    if (!inputFile || !outputFile) {
        fprintf(stderr, "Error: Could not open file(s).\n");
        return 1;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, inputFile)) != -1) {
        // Remove newline character if present
        if (read > 0 && line[read - 1] == '\n') line[read - 1] = '\0';
        
        fprintf(outputFile, "%s\n", line); // Print the expression as it is

        int result = bexpr(line);

        switch (result){
            case ERROR:
                fprintf(outputFile, "Syntax Error\n");
                break;
            case MISSING_SEMICOLON:
                fprintf(outputFile, "===> ';' expected\nSyntax Error\n");
                break;
            case MISSING_CLOSING_PARENTHESIS:
                fprintf(outputFile, "===> ')' expected\nSyntax Error\n");
                break;
            default:
                fprintf(outputFile, "Syntax OK\nValue is %d\n", result);
                break;
        }

    }

    free(line);
    fclose(inputFile);
    fclose(outputFile);
    return 0;
}
