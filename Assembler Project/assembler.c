/** MMN14 Final Assembler Project by:
Tomer Rosenfeld
Shir-May Rappaport
**/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "globals.h"

/** Main function for program **/
int main(int argc, char **argv) {
    FILE *fd;
    int argIndex;
    if (argc < 2) {
        printf(RED "Program called without arguments. Terminating...\n" RESET);
        exit(1);
    }

    argIndex = 1;

    /** Loop for all the files in the argv **/
    while (argIndex < argc) {
        /** File handler definition **/
        char *fileName = argv[argIndex];
        if (fileExtensionChecker(fileName) == FALSE) {
            argIndex++;
            continue;
        }
        fd = fopen(fileName, "r");
        if (fd == NULL) {
            fprintf(stderr, "Filename: %s could not be opened\n", fileName);
            perror("\nERROR: ");
            free(fd);
            argIndex++;
            continue;
        }
        fileName = strtok(argv[argIndex], ".");


        /** Error Table **/
        errTableList *errorList = calloc(1, sizeof(errTableList));
        if (errorList == NULL) {
            fprintf(stderr, "Memory cannot be allocated for errorList\n");
            perror("\nERROR: ");
            free(errorList);
            exit(1);
        }
        errorList->head = NULL;
        errorList->count = 0;

        /** Code Table **/
        tableList *codeTable = calloc(1, sizeof(tableList));
        if (codeTable == NULL) {
            fprintf(stderr, "Memory cannot be allocated for main table\n");
            perror("\nERROR: ");
            free(codeTable);
            exit(1);
        }
        codeTable->head = NULL;

        /** Data Table **/
        tableList *dataTable = calloc(1, sizeof(tableList));
        if (dataTable == NULL) {
            fprintf(stderr, "Memory cannot be allocated for main table\n");
            perror("\nERROR: ");
            free(dataTable);
            exit(1);
        }
        dataTable->head = NULL;

        /** Symbol Table **/
        symTableList *symTable = calloc(1, sizeof(symTableList));
        if (symTable == NULL) {
            fprintf(stderr, "Memory cannot be allocated for symbol table\n");
            perror("\nERROR: ");
            free(symTable);
            exit(1);
        }
        symTable->head = NULL;


        /** Start of first pass **/
        firstPass(errorList, codeTable, dataTable, symTable, fd);

        if (errorList->count > 0) {
            printf(RED "\nFirst pass failed due to %d errors in %s.as file. Second pass will not occur\n" RESET,
                   errorList->count, fileName);
            printErrorList(errorList, argv[argIndex]);
            argIndex++;
            continue;
        }
        if (ICF == 100 && DCF == 0 && symTable->head == NULL) { /* For Empty File*/
            argIndex++;
            continue;
        }
		
        /** Start of second pass **/
        secondPass(errorList, codeTable, symTable, fd);


        if (errorList->count > 0) {
            printf(RED "\nSecond pass failed due to %d errors in %s.as file. Output files will not be generated\n" RESET,
                   errorList->count, fileName);
            printErrorList(errorList, argv[argIndex]);
            argIndex++;
            continue;
        }
		
        /** Create output files **/
        obFile(codeTable, dataTable, fileName);
        entryOutFile(symTable, fileName);
        externOutFile(symTable, codeTable, fileName);


        free(errorList);
        free(codeTable);
        free(dataTable);
        free(symTable);
        
		
        argIndex++;
    } /** end of while loop **/
    
    fclose(fd);
    return 0;
}

