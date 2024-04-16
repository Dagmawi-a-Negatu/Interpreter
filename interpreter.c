#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

#define ERROR -999999

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
        if (result == ERROR) {
            fprintf(outputFile, "Syntax Error\n");
        } else {
            fprintf(outputFile, "Syntax OK\n");
            fprintf(outputFile, "Value is %d\n", result);
        }
    }

    free(line);
    fclose(inputFile);
    fclose(outputFile);
    return 0;
}
