#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "globals.h"

/** Creates the .ob file **/
void obFile(tableList *codeList, tableList *dataList, char *fileName) {
    tableNode *codeNode;
    tableNode *dataNode;
    FILE *nf;
    char *outFile;
    int tempIC;

    outFile = calloc(1, sizeof(char) * strlen(fileName) + 5);
    if (outFile == NULL) {
        fprintf(stderr, "Memory cannot be allocated to temp for outFile\n");
        perror("\nERROR: ");
        free(outFile);
        exit(1);
    }

    strcpy(outFile, fileName);
    strcat(outFile, ".ob");
    nf = fopen(outFile, "w");
    if (nf == NULL) {
        fprintf(stderr, "Cannot create file for object file\n");
        perror("\nERROR: ");
        free(nf);
        exit(1);
    }

    codeNode = codeList->head;
    dataNode = dataList->head;
    tempIC = IC_START_ADDRESS;
    dataCount = 1;

    fprintf(nf, "      %d %d\n", ICF - 100, DCF);

    while (codeNode->next != NULL) {
        if (codeNode->cmdType == 'R') {
            if (tempIC < 1000)
                fprintf(nf, "0%d  ", tempIC);
            rCmdBinaryToHex(*(rCmd *) codeNode->binaryCode, nf);
        } else if (codeNode->cmdType == 'I') {
            if (tempIC < 1000)
                fprintf(nf, "0%d  ", tempIC);
            iCmdBinaryToHex(*(iCmd *) codeNode->binaryCode, nf);
        } else if (codeNode->cmdType == 'J') {
            if (tempIC < 1000)
                fprintf(nf, "0%d  ", tempIC);
            jCmdBinaryToHex(*(jCmd *) codeNode->binaryCode, nf);
        }
        fprintf(nf, "\n");
        tempIC += 4;
        codeNode = codeNode->next;
    }
    if (codeNode->next == NULL) { /* FINAL PRINT */
        if (codeNode->cmdType == 'R') {
            if (tempIC < 1000)
                fprintf(nf, "0%d  ", tempIC);
            rCmdBinaryToHex(*(rCmd *) codeNode->binaryCode, nf);
        } else if (codeNode->cmdType == 'I') {
            if (tempIC < 1000)
                fprintf(nf, "0%d  ", tempIC);
            iCmdBinaryToHex(*(iCmd *) codeNode->binaryCode, nf);
        } else if (codeNode->cmdType == 'J') {
            if (tempIC < 1000)
                fprintf(nf, "0%d  ", tempIC);
            jCmdBinaryToHex(*(jCmd *) codeNode->binaryCode, nf);
        }
        fprintf(nf, "\n");
        tempIC += 4;
    }

    /* Data to hex */
    while (dataNode->next != NULL) {
        if (dataNode->cmdType == 'a' || dataNode->cmdType == 'b') {
            if (tempIC < 1000 && dataCount == 1)
                fprintf(nf, "0%d  ", tempIC);
            while ((dataNode->cmdType == 'a' || dataNode->cmdType == 'b') && dataNode->next != NULL) {
                if (dataCount == 5) {
                    fprintf(nf, "\n");
                    tempIC += 4;
                    fprintf(nf, "0%d  ", tempIC);
                    dataCount = 1;
                }
                if (dataNode->binaryCode == NULL) {
                    fprintf(nf, "%X", (0 & 0xF0) >> 4);
                    fprintf(nf, "%X ", 0 & 0x0F);
                } else ascizCmdBinaryToHex(*(asciz *) dataNode->binaryCode, nf);
                dataCount++;
                dataNode = dataNode->next;
            }
        }
        if (dataNode->cmdType == 'h') {
            if (tempIC < 1000 && dataCount == 1)
                fprintf(nf, "0%d  ", tempIC);
            while (dataNode->cmdType == 'h' && dataNode->next != NULL) {
                if (dataCount == 5) {
                    fprintf(nf, "\n");
                    tempIC += 4;
                    fprintf(nf, "0%d  ", tempIC);
                    dataCount = 1;
                }
                if (dhCmdBinaryToHex(*(dh *) dataNode->binaryCode, nf, tempIC)) {
                    tempIC += 4;
                }
                dataNode = dataNode->next;
            }
        }
        if (dataNode->cmdType == 'w') {
            if (tempIC < 1000 && dataCount == 1)
                fprintf(nf, "0%d  ", tempIC);
            while (dataNode->cmdType == 'w' && dataNode->next != NULL) {
                if (dataCount == 5) {
                    fprintf(nf, "\n");
                    tempIC += 4;
                    fprintf(nf, "0%d  ", tempIC);
                    dataCount = 1;
                }
                if (dwCmdBinaryToHex(*(dw *) dataNode->binaryCode, nf, tempIC)) {
                    tempIC += 4;
                }
                dataNode = dataNode->next;
            }
        }

        if (dataNode->next == NULL) { /* FINAL PRINT */
            if (dataNode->cmdType == 'a' || dataNode->cmdType == 'b') {
                if (dataCount == 5) {
                    fprintf(nf, "\n");
                    tempIC += 4;
                    fprintf(nf, "0%d  ", tempIC);
                    dataCount = 1;
                }
                if (dataNode->binaryCode == NULL) {
                    fprintf(nf, "%X", (0 & 0xF0) >> 4);
                    fprintf(nf, "%X ", 0 & 0x0F);
                } else ascizCmdBinaryToHex(*(asciz *) dataNode->binaryCode, nf);
                dataCount++;
            }
            if (dataNode->cmdType == 'h') {
                if (dataCount == 5) {
                    fprintf(nf, "\n");
                    tempIC += 4;
                    fprintf(nf, "0%d  ", tempIC);
                    dataCount = 1;
                }
                if (dhCmdBinaryToHex(*(dh *) dataNode->binaryCode, nf, tempIC)) {
                    tempIC += 4;
                    dataCount = 1;
                }
                dataCount++;
            }
            if (dataNode->cmdType == 'w') {
                if (dataCount == 5) {
                    fprintf(nf, "\n");
                    tempIC += 4;
                    fprintf(nf, "0%d  ", tempIC);
                    dataCount = 1;
                }
                if (dwCmdBinaryToHex(*(dw *) dataNode->binaryCode, nf, tempIC)) {
                    tempIC += 4;
                    dataCount = 1;
                }
                dataCount++;
            }
            fprintf(nf, "\n");
            tempIC += 4;
        }
    }
    free(outFile);
    fclose(nf);
}

/** Creates the .ent file **/
void entryOutFile(symTableList *list, char *fileName) {
    symTableNode *p;
    char *outFile;
    FILE *nf;

    if (entryCounter(list) == 0) return;

    outFile = calloc(1, sizeof(char) * strlen(fileName) + 5);
    if (outFile == NULL) {
        fprintf(stderr, "Memory cannot be allocated to temp for outFile\n");
        perror("\nERROR: ");
        free(outFile);
        exit(1);
    }

    strcpy(outFile, fileName);
    strcat(outFile, ".ent");
    nf = fopen(outFile, "w");
    if (nf == NULL) {
        fprintf(stderr, "Cannot create file for object file\n");
        perror("\nERROR: ");
        free(nf);
        exit(1);
    }

    p = list->head;
    while (p->next != NULL) {
        if (strcmp(p->attributes, "code, entry") == 0 || strcmp(p->attributes, "data, entry") == 0) {
            fprintf(nf, "%s 0%d\n", p->symbol, p->value);
        }
        p = p->next;
    }
    if (p->next == NULL) {
        if (strcmp(p->attributes, "code, entry") == 0 || strcmp(p->attributes, "data, entry") == 0) {
            fprintf(nf, "%s 0%d\n", p->symbol, p->value);
        }
    }
    free(outFile);
    fclose(nf);
}

/** Creates the .ext file **/
void externOutFile(symTableList *symList, tableList *codeList, char *fileName) {
    symTableNode *symP;
    tableNode *codeP;
    char *outFile, *tempExternName;
    FILE *nf;

    if (externCounter(symList) == 0) return;

    outFile = calloc(1, sizeof(char) * strlen(fileName) + 5);
    if (outFile == NULL) {
        fprintf(stderr, "Memory cannot be allocated to temp for outFile\n");
        perror("\nERROR: ");
        free(outFile);
        exit(1);
    }

    strcpy(outFile, fileName);
    strcat(outFile, ".ext");
    nf = fopen(outFile, "w");
    if (nf == NULL) {
        fprintf(stderr, "Cannot create file for object file\n");
        perror("\nERROR: ");
        free(nf);
        exit(1);
    }

    symP = symList->head;

    while (symP->next != NULL) {
        codeP = codeList->head;
        if (strcmp(symP->attributes, "external") == 0) {
            tempExternName = symP->symbol;
            while (codeP->next != NULL) {
                if (codeP->cmdType == 'J') {
                    if (strstr(codeP->srcCode, tempExternName) != NULL)
                        fprintf(nf, "%s 0%d\n", symP->symbol, codeP->address);
                }
                codeP = codeP->next;
            }
            if (codeP->next == NULL) { /* Last Line */
                if (codeP->cmdType == 'J') {
                    if (strstr(codeP->srcCode, tempExternName) != NULL)
                        fprintf(nf, "%s 0%d\n", symP->symbol, codeP->address);
                }
            }
        }
        symP = symP->next;
    }


    if (symP->next == NULL) { /* Last Line */
        codeP = codeList->head;
        if (strcmp(symP->attributes, "external") == 0) {
            tempExternName = symP->symbol;
            while (codeP->next != NULL) {
                if (codeP->cmdType == 'J') {
                    if (strstr(codeP->srcCode, tempExternName) != NULL)
                        fprintf(nf, "%s 0%d\n", symP->symbol, codeP->address);
                }
            }
            if (codeP->next == NULL) {
                if (codeP->cmdType == 'J') {
                    if (strstr(codeP->srcCode, tempExternName) != NULL)
                        fprintf(nf, "%s 0%d\n", symP->symbol, codeP->address);
                }
            }
        }
    }
    free(outFile);
    fclose(nf);
}
