#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include "globals.h"

/** IGNORE - FOR BINARY OUTPUT TESTING **/
/* --- PRINTF_BYTE_TO_BINARY macro's --- 
#define PRINTF_BINARY_PATTERN_INT8 "%c%c%c%c%c%c%c%c"
#define PRINTF_BYTE_TO_BINARY_INT8(i)    \
(((i) & 0x80ll) ? '1' : '0'), \
(((i) & 0x40ll) ? '1' : '0'), \
(((i) & 0x20ll) ? '1' : '0'), \
(((i) & 0x10ll) ? '1' : '0'), \
(((i) & 0x08ll) ? '1' : '0'), \
(((i) & 0x04ll) ? '1' : '0'), \
(((i) & 0x02ll) ? '1' : '0'), \
(((i) & 0x01ll) ? '1' : '0')

#define PRINTF_BINARY_PATTERN_INT16 \
PRINTF_BINARY_PATTERN_INT8              PRINTF_BINARY_PATTERN_INT8
#define PRINTF_BYTE_TO_BINARY_INT16(i) \
PRINTF_BYTE_TO_BINARY_INT8((i) >> 8),   PRINTF_BYTE_TO_BINARY_INT8(i)
#define PRINTF_BINARY_PATTERN_INT32 \
PRINTF_BINARY_PATTERN_INT16             PRINTF_BINARY_PATTERN_INT16
#define PRINTF_BYTE_TO_BINARY_INT32(i) \
PRINTF_BYTE_TO_BINARY_INT16((i) >> 16), PRINTF_BYTE_TO_BINARY_INT16(i)
 --- end macros --- */


/** List of all code command names **/
const char *codeCommandNames[] = {
        /* R */        "add", "sub", "and", "or", "nor", "move", "mvhi", "mvlo",
        /* I */        "addi", "subi", "andi", "ori", "nori", "bne", "beq", "blt", "bgt", "lb", "sb", "lw", "sw", "lh",
                       "sh",
        /* J */        "jmp", "la", "call", "stop"
};

/** List of all data command names **/
const char *dataCommandNames[] = {
        ".db", ".dh", ".dw", ".asciz"
};

commandStruct commandList[CODE_COMMANDS_LENGTH];
/** Initializing the command list with each command type and details **/
void commandListInit() {
    structArrayInit(commandList);
}

/** Adds a new error node to the error list **/
void addErrorNode(errTableList *list, int lineNumber, int errorCode) {
    errTableNode *errNode, *p;
    char *line = "Line ";
    char *semi = ": ";
    char totalLine[15] = "\0";
    char lineNumberStr[6] = "\0";

    errNode = calloc(1, sizeof(errTableNode));
    if (errNode == NULL) {
        fprintf(stderr, "Memory cannot be allocated to errNode\n");
        perror("\nERROR: ");
        free(errNode);
        exit(1);
    }
    sprintf(lineNumberStr, "%d", lineNumber); /* Conversion from int to string */
    errNode->lineNum = (char *) calloc(15, sizeof(char));
    if (errNode->lineNum == NULL) {
        fprintf(stderr, "Memory cannot be allocated to errNode -> lineNum\n");
        perror("\nERROR: ");
        free(errNode->lineNum);
        exit(1);
    }
    errNode->err = (char *) calloc(70, sizeof(char));
    if (errNode->err == NULL) {
        fprintf(stderr, "Memory cannot be allocated to errNode -> err\n");
        perror("\nERROR: ");
        free(errNode->err);
        exit(1);
    }
    /* Building the Line x: field */
    strcpy(totalLine, line);
    strcat(totalLine, lineNumberStr);
    strcat(totalLine, semi);
    strcpy(errNode->lineNum, totalLine);
    errNode->err = errorText(errorCode);
    if (list->head == NULL) {
        list->head = errNode;
        list->head->next = NULL;
        list->count++;
        return;
    }
    errNode->next = NULL;
    p = list->head;
    while (p->next != NULL) {
        p = p->next;
    }
    p->next = errNode;
    list->count++;
}

/** Creates the error message to be displayed in the error list node **/
char *errorText(int errCode) {
    switch (errCode) {
        case 0:
            return "Line exceeded limit of 80 characters";
        case 1:
            return "Undefined command name";
        case 2:
            return "Incorrect parameter amount";
        case 3:
            return "Incorrect operand type for command";
        case 4:
            return "Register number not defined or out of range";
        case 5:
            return "Label name is not correct syntax";
        case 6:
            return "Label already defined";
        case 7:
            return "One or more of the numbers in the array is out of bounds";
        case 8:
            return "Label is bigger than the limit of 31 characters";
        case 9:
            return "Illegal comma";
        case 10:
            return "Multiple consecutive commas";
        case 11:
            return "Error in line syntax";
        case 12:
            return "Cannot jump to external label";
        case 13:
            return "Jump address out of bounds";
        case 14:
            return "Code table line could not be built due to errors in line";
        case 15:
            return "Data table line could not be built due to errors in line";
        case 16:
            return "Entry label not defined or does not exist";
        case 17:
            return "Could not insert the unknown label in the code table";
        case 18:
            return "Label name is an assembly built in command";
        case 19:
            return "Illegal character";
        default:
            return NULL;
    }
}

/** Prints the error list **/
void printErrorList(errTableList *list, char *fileName) {
    errTableNode *p;
    p = list->head;
    if (list->count > 0)
        printf(RED "Errors in file: %s.as\n" RESET, fileName);
    if (list->count == 1)
        printf(RED "There is %d error:\n" RESET, list->count);
    if (list->count > 1)
        printf(RED "There are %d errors:\n" RESET, list->count);
    while (p != NULL) {
        printf(RED "%s%s\n" RESET, p->lineNum, p->err);
        p = p->next;
    }
}

/** Prints the symbol list **/
void printSymbolList(symTableList *list) {
    symTableNode *p;
    if (list->head == NULL) return;
    p = list->head;
    printf(" **Symbol Table**\n\n");
    while (p != NULL) {
        printf("%s  %d  %s\n", p->symbol, p->value, p->attributes);
        p = p->next;
    }
}

/** Prints the code list **/
void printCodeList(tableList *codeList) {
    tableNode *p;
    if (codeList->head == NULL) return;
    p = codeList->head;
    printf("\t\t**Code Table**\n\n");
    while (p != NULL) {
        printf("%d \t %s  \t ", p->address, p->srcCode);
        if (p->cmdType == 'R') {
            printRCmd(p->binaryCode);
        } else if (p->cmdType == 'I') {
            printICmd(p->binaryCode);
        } else {
            printJCmd(p->binaryCode);
        }
        printf("\n");
        p = p->next;
    }
}


/** Prints the data list - FOR USE WITH ABOVE TESTING [IGNORE]
void printDataList(tableList *dataList) {
    tableNode *p;
    db *dbTemp;
    dh *dhTemp;
    dw *dwTemp;
    asciz *ascizTemp;
    if (dataList->head == NULL) return;
    p = dataList->head;
    printf("\t\t**Data Table**\n\n");
    while (p != NULL) {
        printf("%d \t %s  \t ", p->address, p->srcCode);
        if (p->cmdType == 'b') {
            dbTemp = (db *) p->binaryCode;
            printf(PRINTF_BINARY_PATTERN_INT8, PRINTF_BYTE_TO_BINARY_INT8(dbTemp->binary));
        } else if (p->cmdType == 'h') {
            dhTemp = (dh *) p->binaryCode;
            printf(PRINTF_BINARY_PATTERN_INT16, PRINTF_BYTE_TO_BINARY_INT16(dhTemp->binary));
        } else if (p->cmdType == 'w') {
            dwTemp = (dw *) p->binaryCode;
            printf(PRINTF_BINARY_PATTERN_INT32, PRINTF_BYTE_TO_BINARY_INT32(dwTemp->binary));
        } else if (p->cmdType == 'a') {
            ascizTemp = (asciz *) p->binaryCode;
            if (ascizTemp != NULL)
                printf(PRINTF_BINARY_PATTERN_INT8, PRINTF_BYTE_TO_BINARY_INT8(ascizTemp->binary));
            else printf(PRINTF_BINARY_PATTERN_INT8, PRINTF_BYTE_TO_BINARY_INT8(0));
        }
        printf("\n");
        p = p->next;
    }
}**/

/** Adds a new table row for the code list **/
int addCodeNode(tableList *codeList, symTableList *symList, errTableList *errList, char labelParam[MAX_LABEL_SIZE],
                char *line, char *cmd, int *regs, int *nums, int IC, int lineNum) {
    tableNode *newNode, *p;
    char *tempLine;

    newNode = calloc(1, sizeof(tableNode));
    if (newNode == NULL) {
        fprintf(stderr, "Memory cannot be allocated to new tableNode\n");
        perror("\nERROR: ");
        free(newNode);
        exit(1);
    }

    tempLine = (char *) calloc(1, MAX_LINE_SIZE * sizeof(char));
    if (tempLine == NULL) {
        fprintf(stderr, "Memory cannot be allocated to tempLine\n");
        perror("\nERROR: ");
        free(tempLine);
        exit(1);
    }
    strcpy(tempLine, line);

    if (cmdTypeFinder(cmd) == 'R') {
        newNode->binaryCode = (rCmd *) rCmdBuilder(regs, cmd);
        newNode->cmdType = 'R';
    } else if (cmdTypeFinder(cmd) == 'I') {
        newNode->binaryCode = (iCmd *) iCmdBuilder(errList, symList, labelParam, regs, nums, cmd, IC, lineNum);
        newNode->cmdType = 'I';
    } else {
        newNode->binaryCode = (jCmd *) jCmdBuilder(symList, labelParam, regs, cmd);
        newNode->cmdType = 'J';
    }

    if (newNode->binaryCode == NULL) return FALSE;

    newNode->address = IC;
    newNode->lineNum = lineNum;
    newNode->srcCode = tempLine;

    if (codeList->head == NULL) {
        codeList->head = newNode;
        codeList->head->next = NULL;
        return ok;
    }
    newNode->next = NULL;
    p = codeList->head;
    while (p->next != NULL) {
        p = p->next;
    }
    p->next = newNode;
    return ok;
}

/** Adds a new table row for the data list **/
int addDataNode(tableList *dataList, errTableList *errList, char *line, char *cmd, int *nums,
                int lineNum, char *ascizStr) {
    tableNode *newNode, *p;
    int i, errorCode;
    char *tempLine;
    db *dbTemp;
    dh *dhTemp;
    dw *dwTemp;
    asciz *ascizTemp;


    tempLine = (char *) calloc(1, MAX_LINE_SIZE * sizeof(char));
    if (tempLine == NULL) {
        fprintf(stderr, "Memory cannot be allocated to tempLine\n");
        perror("\nERROR: ");
        free(tempLine);
        exit(1);
    }
    strcpy(tempLine, line);


    if (strcmp(cmd, ".db") == 0) {
        for (i = 0; i < numCounter; i++) {
            if (nums[i] > MAX_BYTE_NUM_POS || nums[i] < MAX_BYTE_NUM_NEG) {
                errorCode = numberOutOfBounds;
                addErrorNode(errList, lineNum, errorCode);
                continue;
            }
            newNode = calloc(1, sizeof(tableNode));
            if (newNode == NULL) {
                fprintf(stderr, "Memory cannot be allocated to new tableNode\n");
                perror("\nERROR: ");
                free(newNode);
                exit(1);
            }

            dbTemp = calloc(1, sizeof(db));
            if (dbTemp == NULL) {
                fprintf(stderr, "Memory cannot be allocated to dbTemp\n");
                perror("\nERROR: ");
                free(dbTemp);
                exit(1);
            }
            dbTemp->binary = nums[i];
            newNode->binaryCode = (db *) dbTemp;

            newNode->address = DC;
            if (i == 0) newNode->srcCode = tempLine;
            else newNode->srcCode = "";
            newNode->cmdType = 'b';

            if (dataList->head == NULL) {
                dataList->head = newNode;
                dataList->head->next = NULL;
                DC += 1;
                continue;
            }
            newNode->next = NULL;
            p = dataList->head;
            while (p->next != NULL) {
                p = p->next;
            }
            p->next = newNode;
            DC += 1;
        }
        return ok;
    }

    if (strcmp(cmd, ".dh") == 0) {
        for (i = 0; i < numCounter; i++) {
            if (nums[i] > MAX_HALF_WORD_NUM_POS || nums[i] < MAX_HALF_WORD_NUM_NEG) {
                errorCode = numberOutOfBounds;
                addErrorNode(errList, lineNum, errorCode);
                continue;
            }
            newNode = calloc(1, sizeof(tableNode));
            if (newNode == NULL) {
                fprintf(stderr, "Memory cannot be allocated to new tableNode\n");
                perror("\nERROR: ");
                free(newNode);
                exit(1);
            }

            dhTemp = calloc(1, sizeof(dh));
            if (dhTemp == NULL) {
                fprintf(stderr, "Memory cannot be allocated to dhTemp\n");
                perror("\nERROR: ");
                free(dhTemp);
                exit(1);
            }
            dhTemp->binary = nums[i];
            newNode->binaryCode = (dh *) dhTemp;

            newNode->address = DC;
            if (i == 0) newNode->srcCode = tempLine;
            else newNode->srcCode = "";
            newNode->cmdType = 'h';

            if (dataList->head == NULL) {
                dataList->head = newNode;
                dataList->head->next = NULL;
                DC += 2;
                continue;
            }
            newNode->next = NULL;
            p = dataList->head;
            while (p->next != NULL) {
                p = p->next;
            }
            p->next = newNode;
            DC += 2;
        }
        return ok;
    }

    if (strcmp(cmd, ".dw") == 0) {
        for (i = 0; i < numCounter; i++) {
            if (nums[i] > INT_MAX || nums[i] < INT_MIN) {
                errorCode = numberOutOfBounds;
                addErrorNode(errList, lineNum, errorCode);
                continue;
            }
            newNode = calloc(1, sizeof(tableNode));
            if (newNode == NULL) {
                fprintf(stderr, "Memory cannot be allocated to new tableNode\n");
                perror("\nERROR: ");
                free(newNode);
                exit(1);
            }

            dwTemp = calloc(1, sizeof(dw));
            if (dwTemp == NULL) {
                fprintf(stderr, "Memory cannot be allocated to dwTemp\n");
                perror("\nERROR: ");
                free(dwTemp);
                exit(1);
            }
            dwTemp->binary = nums[i];
            newNode->binaryCode = (dw *) dwTemp;

            newNode->address = DC;
            if (i == 0) newNode->srcCode = tempLine;
            else newNode->srcCode = "";
            newNode->cmdType = 'w';

            if (dataList->head == NULL) {
                dataList->head = newNode;
                dataList->head->next = NULL;
                DC += 4;
                continue;
            }
            newNode->next = NULL;
            p = dataList->head;
            while (p->next != NULL) {
                p = p->next;
            }
            p->next = newNode;
            DC += 4;
        }
        return ok;
    }

    if (strcmp(cmd, ".asciz") == 0) {
        for (i = 0; i < strlen(ascizStr); i++) {
            if (ascizStr[i] == '"') continue;
            newNode = calloc(1, sizeof(tableNode));
            if (newNode == NULL) {
                fprintf(stderr, "Memory cannot be allocated to new tableNode\n");
                perror("\nERROR: ");
                free(newNode);
                exit(1);
            }

            ascizTemp = calloc(1, sizeof(asciz));
            if (ascizTemp == NULL) {
                fprintf(stderr, "Memory cannot be allocated to ascizTemp\n");
                perror("\nERROR: ");
                free(ascizTemp);
                exit(1);
            }
            ascizTemp->binary = (unsigned char) ascizStr[i];
            newNode->binaryCode = (asciz *) ascizTemp;

            newNode->address = DC;
            if (i == 1) newNode->srcCode = tempLine;
            else newNode->srcCode = "";
            newNode->cmdType = 'a';

            if (dataList->head == NULL) {
                dataList->head = newNode;
                dataList->head->next = NULL;
                DC += 1;
                continue;
            }
            newNode->next = NULL;
            p = dataList->head;
            while (p->next != NULL) {
                p = p->next;
            }
            p->next = newNode;
            DC += 1;
        }

        /* For last \0 entry */
        newNode = calloc(1, sizeof(tableNode));
        if (newNode == NULL) {
            fprintf(stderr, "Memory cannot be allocated to new tableNode\n");
            perror("\nERROR: ");
            free(newNode);
            exit(1);
        }

        ascizTemp = calloc(1, sizeof(asciz));
        if (ascizTemp == NULL) {
            fprintf(stderr, "Memory cannot be allocated to ascizTemp\n");
            perror("\nERROR: ");
            free(ascizTemp);
            exit(1);
        }
        newNode->binaryCode = 0;
        newNode->address = DC;
        newNode->srcCode = "";
        newNode->cmdType = 'a';

        newNode->next = NULL;
        p = dataList->head;
        while (p->next != NULL) {
            p = p->next;
        }
        p->next = newNode;
        DC += 1;
        return ok;
    }
    return FALSE;
}

/** Adds a new table row for the symbol table **/
int addSymNode(symTableList *symList, int IC, char *labelName, char *attr) {
    symTableNode *newNode, *p;
    char *tempLabel = (char *) calloc(1, strlen(labelName) * sizeof(char) + 1);
    if (tempLabel == NULL) {
        fprintf(stderr, "Memory cannot be allocated to tempLabel\n");
        perror("\nERROR: ");
        free(tempLabel);
        exit(1);
    }
    strcpy(tempLabel, labelName);

    if (isLabelExist(labelName, symList) && symList->head != NULL) return labelAlreadyDefined;
    newNode = calloc(1, sizeof(symTableNode));
    if (newNode == NULL) {
        fprintf(stderr, "Memory cannot be allocated to new symNode\n");
        perror("\nERROR: ");
        free(newNode);
        exit(1);
    }


    newNode->symbol = tempLabel;
    newNode->attributes = attr;
    newNode->value = IC;

    if (symList->head == NULL) {
        symList->head = newNode;
        symList->head->next = NULL;
        return ok;
    }
    newNode->next = NULL;
    p = symList->head;
    while (p->next != NULL) {
        p = p->next;
    }
    p->next = newNode;
    return ok;
}

/** Checks if a label already exists in the symbol table **/
int isLabelExist(char *label, symTableList *symList) {
    symTableNode *p;
    p = symList->head;
    while (p != NULL) {
        if (strcmp(p->symbol, label) == 0) return TRUE;
        p = p->next;
    }
    return FALSE;
}

/** Checks if the input line contains : for labels **/
int isLabel(char *input) {
    int i;

    for (i = 0; i < strlen(input); i++) {
        if (input[i] == ':') return 1;
    }
    return 0;
}

/** Checks if the max line size has been exceeded **/
int lineSizeException(int length) {
    if (length > MAX_LINE_SIZE)
        return lineExceededLimit;
    return ok;
}

/** Checks if the label is valid syntax and size, and that it isn't a built in command name **/
int labelChecker(char *label) {
    int i;
    if (strlen(label) > MAX_LABEL_SIZE) return labelTooLarge;
    for (i = 0; i < strlen(label); i++) {
        if (!isalpha(label[i]) && !isdigit(label[i])) return undefinedLabel;
    }
    for (i = 0; i < CODE_COMMANDS_LENGTH; i++) {
        if (strcmp(label, codeCommandNames[i]) == 0)
            return labelIsBuiltInCmd;
    }
    for (i = 0; i < DATA_COMMANDS_LENGTH; i++) {
        if (strcmp(label, dataCommandNames[i]) == 0)
            return labelIsBuiltInCmd;
    }
    return ok;
}

/** Checks if the cmd is valid syntax **/
int cmdChecker(char *cmd) {
    int i;
    if (strlen(cmd) > MAX_CMD_SIZE) return undefinedCmdName;
    for (i = 0; i < CODE_COMMANDS_LENGTH; i++) {
        if (strcmp(cmd, codeCommandNames[i]) == 0)
            return ok;
    }
    for (i = 0; i < DATA_COMMANDS_LENGTH; i++) {
        if (strcmp(cmd, dataCommandNames[i]) == 0)
            return ok;
    }
    return undefinedCmdName;
}

/** Checks there is a comma in str for multiple consec comma **/
int isCommaF(char *str) {
    int i;

    if (strlen(str) == 0) return 1;
    for (i = 0; i < strlen(str); i++) {
        if (str[i] == ' ') continue;
        if (str[i] == ',') return 1;
        if (isdigit(str[i]) != 0) return 0; /* If char is a number */
        if (isalpha(str[i]) != 0) return 0; /* If char is a letter */
    }
    return 0;
}

/** Checks there is a comma in str **/
int isComma(char *str) {
    int i;

    if (strlen(str) == 0) return 1;
    for (i = 0; i < strlen(str); i++) {
        if (str[i] == ',') return 1;
    }
    return 0;
}

/** Checks if there are multiple consecutive commas **/
int mulCommaChecker(char *input) {
    int i;

    if (input[0] == ',') return illegalComma; /* If the first char of the input is a comma */
    if (isCommaF(input + (strlen(input) - 2)) == 1)
        return illegalComma; /* If the last char of the input is a comma */
    for (i = 1; i < strlen(input); i++) {
        if (input[i] == ',') {
            if (isCommaF(input + i + 1) == 1) return multipleConsecCommas;
        }
    }
    return ok;
}

/** Checks if the input is extern **/
int isExtern(char *input) {
    char *check;
    char tempInput[MAX_LINE_SIZE];
    strcpy(tempInput, input);
    if (isBackSlashN(tempInput))
        tempInput[strlen(tempInput) - 1] = '\0';
    if (strcmp(tempInput, ".extern") == 0) return ok;
    check = strstr(tempInput, ".extern");
    check = strtok(check, " ");
    if (check != NULL && strcmp(check, ".extern") == 0)
        return ok;
    return FALSE;
}

/** Checks if the input is entry **/
int isEntry(char *input) {
    char *check;
    char tempInput[MAX_LINE_SIZE];
    strcpy(tempInput, input);
    if (isBackSlashN(tempInput))
        tempInput[strlen(tempInput) - 1] = '\0';
    if (strcmp(tempInput, ".entry") == 0) return ok;
    check = strstr(tempInput, ".entry");
    check = strtok(check, " ");
    if (check != NULL && strcmp(check, ".entry") == 0)
        return ok;
    return FALSE;
}

/** Checks if a string contains '\n' **/
int isBackSlashN(char *str) {
    int i;
    for (i = 0; i < strlen(str); i++) {
        if (str[i] == '\n') return TRUE;
    }
    return FALSE;
}

/** Checks if there are parameters after the entry or extern command **/
int entryExternSyntaxChecker(char *input, char *labelName) {
    char *check = NULL;
    char tempInput[MAX_LINE_SIZE];
    strcpy(tempInput, input);
    check = strstr(tempInput, labelName);
    if (isBackSlashN(check))
        check[strlen(check) - 1] = '\0';
    if (onlyContains(check, labelName)) return ok;
    else return incorrectParamAmount;
}

/** Checks if only the needle string is in the haystack without other characters **/
int onlyContains(char *haystack, char *needle) {
    int counter, i, j, k, haystackCounter;
    haystackCounter = 0;
    counter = 0;
    i = 0;
    for (k = 0; k < strlen(haystack); k++)
        if (isalpha(haystack[k]) || isdigit(haystack[k])) haystackCounter++;
    for (j = 0; j < strlen(haystack); j++) {
        if (haystack[j] == ' ' || haystack[j] == '\t') continue;
        if (haystack[j] == needle[i] && strlen(needle) == 1 && haystackCounter == 1) return ok;
        if (haystack[j] == needle[i]) {
            counter++;
            break;
        }
    }
    for (i = 1; i < strlen(needle);) {
        for (j = 1; j < strlen(haystack); j++) {
            if (haystack[j] == needle[i]) {
                counter++;
                i++;
            }
        }
    }
    if (counter == strlen(needle) && haystackCounter == counter) return ok;
    return FALSE;
}

/** Checks if there is a $ in a string **/
int containsRegSign(char *str) {
    int i;
    for (i = 0; i < strlen(str); i++) {
        if (str[i] == '$') return TRUE;
    }
    return FALSE;
}

/** Counts how many numbers in a number array **/
int countNums(const int *nums) {
    int i, count;
    count = 0;
    for (i = 0; nums[i] != '$'; i++) {
        if (nums[i] != '$') count++;
    }
    return count;
}

/** Checks if there is a label in the parameters **/
int isParamsContainLabel(char *params, char *cmd) {
    int i;
    char paramCopy[strlen(params)];
    strcpy(paramCopy, paramsIdentifier(cmd, params));
    if (isBackSlashN(paramCopy))
        paramCopy[strlen(paramCopy) - 1] = '\0';
    for (i = 0; i < strlen(paramCopy); i++) {
        if (isalpha(paramCopy[i]) != 0) return TRUE;
    }
    return FALSE;
}

/** Trims the label in case there is leftover characters **/
char *labelTrimmer(char *label) {
    int i;
    char *temp;
    for (i = 0; i < strlen(label); i++) {
        if (isspace(label[i + 1]) != 0) {
            temp = strtok(label, &label[i]);
            return temp;
        }
    }
    return label;
}

/** Checks if the command is one of the J type commands **/
int checkJCmds(char *cmd) {
    if (strcmp(cmd, "jmp") == 0 || strcmp(cmd, "la") == 0 || strcmp(cmd, "call") == 0) return TRUE;
    return FALSE;
}

/** Checks if the parameters contains numbers **/
int containNums(char *params) {
    int i;
    for (i = 0; i < strlen(params); i++) {
        if (params[i] == '$') i += 3;
        if (isdigit(params[i])) return TRUE;
    }
    return FALSE;
}

/** Checks if the input line from the file is a comment line and checks its syntax**/
int isComment(char *input) {
    int i;
    if (input[0] == ';') return TRUE;
    if (isBackSlashN(input))
        input[strlen(input) - 1] = '\0';
    for (i = 0; i < strlen(input); i++) {
        if (isalpha(input[i]) != 0 && input[i] != ';' && input[i] != ' ') return errorInLineSyntax;
        if (input[i] == ';') return TRUE;
    }
    return TRUE;
}

/** Checks if the input line from the file is a comment line**/
int isCommentCheck(char *input) {
    int i;
    if (input[0] == ';') return TRUE;
    if (isBackSlashN(input))
        input[strlen(input) - 1] = '\0';
    for (i = 0; i < strlen(input); i++) {
        if (input[i] == ';') return TRUE;
    }
    return FALSE;
}

/** Checks if the command is a data command **/
int isDataCmd(char *cmd) {
    if (strcmp(cmd, dataCommandNames[0]) == 0) return TRUE;
    if (strcmp(cmd, dataCommandNames[1]) == 0) return TRUE;
    if (strcmp(cmd, dataCommandNames[2]) == 0) return TRUE;
    if (strcmp(cmd, dataCommandNames[3]) == 0) return TRUE;
    return FALSE;
}

/** Checks how many registers are in the regs array **/
int regsCounter(const int *regs) {
    int i, count;
    count = 0;
    for (i = 0; i < 3; i++) {
        if (regs[i] != -1) count++;
    }
    return count;
}

/** Checks parameters for each command **/
int paramCmdCheck(char *cmd, int *regs, const int *nums) {
    int numCount, regCount;
    numCount = regCount = 0;
    if (nums != NULL)
        numCount = numCounter;
    if (regs != NULL)
        regCount = regsCounter(regs);
    if (cmd == NULL) return checkForExternEntry;

    /** R **/
    if (strcmp(cmd, "add") == 0 || strcmp(cmd, "sub") == 0 || strcmp(cmd, "and") == 0 || strcmp(cmd, "or") == 0 ||
        strcmp(cmd, "nor") == 0) {
        if (regCount == 3 && numCount == 0) return ok;
        return FALSE;
    }
    if (strcmp(cmd, "move") == 0 || strcmp(cmd, "mvhi") == 0 || strcmp(cmd, "mvlo") == 0) {
        if (regCount == 2 && numCount == 0) return ok;
        return FALSE;
    }
    /** I **/
    if (strcmp(cmd, "addi") == 0 || strcmp(cmd, "subi") == 0 || strcmp(cmd, "andi") == 0 ||
        strcmp(cmd, "ori") == 0 ||
        strcmp(cmd, "nori") == 0) {
        if (regCount == 2 && numCount == 1 && nums[0] > MAX_HALF_WORD_NUM_NEG && nums[0] < MAX_HALF_WORD_NUM_POS)
            return ok;
        return FALSE;
    }
    if (strcmp(cmd, "beq") == 0 || strcmp(cmd, "bne") == 0 || strcmp(cmd, "blt") == 0 || strcmp(cmd, "bgt") == 0) {
        if (regCount == 2 && numCount == 0) return ok;
        return FALSE;
    }
    if (strcmp(cmd, "lb") == 0 || strcmp(cmd, "sb") == 0 || strcmp(cmd, "lw") == 0 || strcmp(cmd, "sw") == 0 ||
        strcmp(cmd, "lh") == 0 || strcmp(cmd, "sh") == 0) {
        if (regCount == 2 && numCount == 1 && nums[0] > MAX_HALF_WORD_NUM_NEG && nums[0] < MAX_HALF_WORD_NUM_POS)
            return ok;
        return FALSE;
    }
    /** J **/
    if (strcmp(cmd, "jmp") == 0 || strcmp(cmd, "la") == 0 || strcmp(cmd, "call") == 0) {
        if ((regCount == 1 || regCount == 0) && numCount == 0) return ok;
        return FALSE;
    }
    if (strcmp(cmd, "stop") == 0) {
        if (regCount == 0 && numCount == 0) return ok;
        return FALSE;
    }

    /** DATA **/
    if (strcmp(cmd, ".asciz") == 0) {
        if (regCount == 0 && numCount == 0) return ok;
        return FALSE;
    }
    if (strcmp(cmd, ".db") == 0) {
        int i;
        if (regCount == 0 && numCount > 0 && nums != NULL) {
            for (i = 0; i < numCount; i++) {
                if (nums[i] < MAX_BYTE_NUM_NEG || nums[i] > MAX_BYTE_NUM_POS)
                    return FALSE;
            }
            return ok;
        }
        return FALSE;
    }
    if (strcmp(cmd, ".dh") == 0) {
        int i;
        if (regCount == 0 && numCount > 0 && nums != NULL) {
            for (i = 0; i < numCount; i++) {
                if (nums[i] < MAX_HALF_WORD_NUM_NEG || nums[i] > MAX_HALF_WORD_NUM_POS)
                    return FALSE;
            }
            return ok;
        }
        return FALSE;
    }
    if (strcmp(cmd, ".dw") == 0) {
        int i;
        if (regCount == 0 && numCount > 0 && nums != NULL) {
            for (i = 0; i < numCount; i++) {
                if (nums[i] < INT_MIN || nums[i] > INT_MAX)
                    return FALSE;
            }
            return ok;
        }
        return FALSE;
    }
    return FALSE;
}

/** Returns command's opcode number **/
int cmdOpCodeFinder(char *cmd) {
    int i;
    for (i = 0; i < CODE_COMMANDS_LENGTH; i++) {
        if (strcmp(commandList[i].name, cmd) == 0) return commandList[i].opcode;
    }
    return FALSE;
}

/** Returns command's funct number **/
int cmdFunctCodeFinder(char *cmd) {
    int i;
    for (i = 0; i < CODE_COMMANDS_LENGTH; i++) {
        if (strcmp(commandList[i].name, cmd) == 0) return commandList[i].funct;
    }
    return FALSE;
}

/** Returns command's type **/
char cmdTypeFinder(char *cmd) {
    int i;
    for (i = 0; i < CODE_COMMANDS_LENGTH; i++) {
        if (strcmp(commandList[i].name, cmd) == 0) return commandList[i].type;
    }
    return FALSE;
}

/** Builds the binary line for the code table **/
rCmd *rCmdBuilder(int *regs, char *cmd) {
    rCmd *tempRCmd;

    tempRCmd = calloc(1, sizeof(rCmd));
    if (tempRCmd == NULL) {
        fprintf(stderr, "Memory cannot be allocated to tempRCmd\n");
        perror("\nERROR: ");
        free(tempRCmd);
        exit(1);
    }
    tempRCmd->notUsed = 0;
    tempRCmd->funct = cmdFunctCodeFinder(cmd);
    if (strcmp(cmd, "add") == 0 || strcmp(cmd, "sub") == 0 || strcmp(cmd, "and") == 0 || strcmp(cmd, "or") == 0 ||
        strcmp(cmd, "nor") == 0) {
        tempRCmd->rs = regs[0];
        tempRCmd->rt = regs[1];
        tempRCmd->rd = regs[2];
        tempRCmd->opcode = 0;
    } else if (strcmp(cmd, "move") == 0 || strcmp(cmd, "mvhi") == 0 || strcmp(cmd, "mvlo") == 0) {
        tempRCmd->rt = 0;
        tempRCmd->rs = regs[0];
        tempRCmd->rd = regs[1];
        tempRCmd->opcode = 1;
    }
    return tempRCmd;
}

/** Prints the binary line for rCmds **/
void printRCmd(rCmd *node) {
    int count;
    typedef struct {
        unsigned data: 32;
    } mask;
    unsigned x;
    mask temp = {0};
    count = 0;
    temp.data = temp.data |
                (((int) node->notUsed) | ((int) node->funct << 6) | ((int) node->rd << 11) |
                 ((int) node->rt << 16) |
                 ((int) node->rs << 21) | ((int) node->opcode << 26));
    x = 1;
    x = x << (unsigned) (32 - 1);
    while (x) {
        if (temp.data & x)
            printf("1");
        else printf("0");
        x >>= 1;
        if (count == 5) {
            printf(" ");
            count++;
            continue;
        }
        if (count == 10) {
            printf(" ");
            count++;
            continue;
        }
        if (count == 15) {
            printf(" ");
            count++;
            continue;
        }
        if (count == 20) {
            printf(" ");
            count++;
            continue;
        }
        if (count == 25) {
            printf(" ");
            count++;
            continue;
        }
        count++;
    }
    printf("\n");
}

/** Builds the binary line for the code table **/
iCmd *
iCmdBuilder(errTableList *errList, symTableList *list, char labelParam[MAX_LABEL_SIZE], int *regs, int *nums,
            char *cmd,
            int IC, int lineNum) {
    iCmd *tempICmd;
    int errorCode;

    tempICmd = calloc(1, sizeof(iCmd));
    if (tempICmd == NULL) {
        fprintf(stderr, "Memory cannot be allocated to tempICmd\n");
        perror("\nERROR: ");
        free(tempICmd);
        exit(1);
    }
    if (strcmp(cmd, "addi") == 0 || strcmp(cmd, "subi") == 0 || strcmp(cmd, "andi") == 0 ||
        strcmp(cmd, "ori") == 0 ||
        strcmp(cmd, "nori") == 0) {
        tempICmd->rs = regs[0];
        tempICmd->rt = regs[1];
        tempICmd->immed = nums[0];
        tempICmd->opcode = cmdOpCodeFinder(cmd);
    } else if (strcmp(cmd, "beq") == 0 || strcmp(cmd, "bne") == 0 || strcmp(cmd, "blt") == 0 ||
               strcmp(cmd, "bgt") == 0) {
        tempICmd->rs = regs[0];
        tempICmd->rt = regs[1];
        tempICmd->opcode = cmdOpCodeFinder(cmd);
        if (list->head == NULL) tempICmd->immed = IC;
        else {
            if (labelAddressSymTableFinder(list, labelParam) == FALSE) tempICmd->immed = IC;
            if (labelAddressSymTableFinder(list, labelParam) == errorInImmed) { /* Extern */
                errorCode = labelIsDefinedAsExtern;
                addErrorNode(errList, lineNum, errorCode);
                return NULL;
            }
            if (labelAddressSymTableFinder(list, labelParam) != FALSE)
                tempICmd->immed = IC;
            if ((int) tempICmd->immed > MAX_HALF_WORD_NUM_POS - 1 ||
                (int) tempICmd->immed < MAX_HALF_WORD_NUM_NEG + 1) {
                errorCode = jmpTooBig;
                addErrorNode(errList, lineNum, errorCode);
                return NULL;
            }
        }
    } else if (strcmp(cmd, "lb") == 0 || strcmp(cmd, "sb") == 0 || strcmp(cmd, "lw") == 0 ||
               strcmp(cmd, "sw") == 0 ||
               strcmp(cmd, "lh") == 0 || strcmp(cmd, "sh") == 0) {
        tempICmd->rs = regs[0];
        tempICmd->rt = regs[1];
        tempICmd->immed = nums[0];
        tempICmd->opcode = cmdOpCodeFinder(cmd);
    }
    return tempICmd;
}

/** Prints the binary line for iCmds **/
void printICmd(iCmd *node) {
    int count;
    typedef struct {
        unsigned data: 32;
    } mask;
    unsigned x;
    mask temp = {0};
    count = 0;
    temp.data =
            temp.data | (node->immed) | ((int) node->rt << 16) | ((int) node->rs << 21) |
            ((int) node->opcode << 26);
    x = 1;
    x = x << (unsigned) (32 - 1);
    while (x) {
        if (temp.data & x)
            printf("1");
        else printf("0");
        x >>= 1;
        if (count == 5) {
            printf(" ");
            count++;
            continue;
        }
        if (count == 10) {
            printf(" ");
            count++;
            continue;
        }
        if (count == 15) {
            printf(" ");
            count++;
            continue;
        }
        count++;
    }
    printf("\n");
}


/** Returns the address of a label if it appears in the symbol list **/
int labelAddressSymTableFinder(symTableList *list, char *label) {
    symTableNode *p;
    p = list->head;
    while (p->next != NULL) {
        if (strcmp(p->symbol, label) == 0 && strcmp(p->attributes, "external") != 0) return p->value;
        if (strcmp(p->symbol, label) == 0 && strcmp(p->attributes, "external") == 0) return errorInImmed;
        p = p->next;
    }
    return FALSE;
}

/** Builds the binary line for the code table **/
jCmd *jCmdBuilder(symTableList *list, char labelParam[MAX_LABEL_SIZE], int *regs, char *cmd) {
    jCmd *tempJCmd;

    tempJCmd = calloc(1, sizeof(iCmd));
    if (tempJCmd == NULL) {
        fprintf(stderr, "Memory cannot be allocated to tempJCmd\n");
        perror("\nERROR: ");
        free(tempJCmd);
        exit(1);
    }

    if (strcmp(cmd, "jmp") == 0) {
        if (regs[0] != -1) {
            tempJCmd->reg = 1;
            tempJCmd->address = regs[0];
        }
        if (strcmp(labelParam, "") != 0) {
            tempJCmd->reg = 0;
            if (list->head == NULL) tempJCmd->address = UNKNOWN;
            else {
                if (labelAddressSymTableFinder(list, labelParam) == FALSE) tempJCmd->address = UNKNOWN;
                if (labelAddressSymTableFinder(list, labelParam) == errorInImmed) tempJCmd->address = 0;
                if (labelAddressSymTableFinder(list, labelParam) != FALSE)
                    tempJCmd->address = labelAddressSymTableFinder(list, labelParam);
            }
        }
        tempJCmd->opcode = cmdOpCodeFinder(cmd);
    }
    if (strcmp(cmd, "la") == 0 || strcmp(cmd, "call") == 0) {
        if (list->head == NULL) tempJCmd->address = UNKNOWN;
        else {
            if (labelAddressSymTableFinder(list, labelParam) == FALSE) tempJCmd->address = UNKNOWN;
            if (labelAddressSymTableFinder(list, labelParam) == errorInImmed) tempJCmd->address = 0;
            if (labelAddressSymTableFinder(list, labelParam) != FALSE)
                tempJCmd->address = labelAddressSymTableFinder(list, labelParam);
        }
        tempJCmd->reg = 0;
        tempJCmd->opcode = cmdOpCodeFinder(cmd);
    }
    if (strcmp(cmd, "stop") == 0) {
        tempJCmd->address = 0;
        tempJCmd->reg = 0;
        tempJCmd->opcode = cmdOpCodeFinder(cmd);
    }
    return tempJCmd;
}

/** Prints the binary line for jCmds **/
void printJCmd(jCmd *node) {
    int count;
    typedef struct {
        unsigned data: 32;
    } mask;
    unsigned x;
    mask temp = {0};
    count = 0;
    temp.data = temp.data | (node->address) | ((int) node->reg << 25) | ((int) node->opcode << 26);
    x = 1;
    x = x << (unsigned) (32 - 1);
    while (x) {
        if (temp.data & x)
            printf("1");
        else printf("0");
        x >>= 1;
        if (count == 5) {
            printf(" ");
            count++;
            continue;
        }
        if (count == 6) {
            printf(" ");
            count++;
            continue;
        }
        count++;
    }
    printf("\n");
}

/** Update the dataList's addresses with ICF **/
void updateDataListICF(tableList *dataList) {
    tableNode *p;

    p = dataList->head;
    while (p->next != NULL) {
        p->address += ICF;
        p = p->next;
    }
    if ((p->cmdType == 'a' || p->cmdType == 'b' || p->cmdType == 'h' || p->cmdType == 'w') && p->next == NULL)
        p->address += ICF;
}

/** Update the symbol list's addresses with ICF **/
void updateSymListICF(symTableList *symList) {
    symTableNode *p;

    p = symList->head;
    while (p->next != NULL) {
        if (strcmp(p->attributes, "data") == 0)
            p->value += ICF;
        p = p->next;
    }
}


/*************************************************************** SECOND PASS FUNCTIONS ***************************************************************/

/** Adds the entry tag to the symbol list **/
int addEntryToSymList(symTableList *symList, char *label) {
    symTableNode *p;
    char *temp;
    if (isLabelExist(label, symList) == FALSE) return FALSE;
    temp = calloc(1, sizeof(char) * 20);
    if (temp == NULL) {
        fprintf(stderr, "Memory cannot be allocated to temp for entry symbol list\n");
        perror("\nERROR: ");
        free(temp);
        exit(1);
    }

    p = symList->head;
    while (p->next != NULL) {
        if (strcmp(p->symbol, label) == 0) {
            strcpy(temp, p->attributes);
            strcat(temp, ", entry");
            p->attributes = temp;
            return ok;
        }
        p = p->next;
    }
    free(temp);
    return FALSE;
}

/** Returns true if the command is one of the J commands **/
int isJCmd(char *cmd) {
    if (strcmp(cmd, "jmp") == 0 || strcmp(cmd, "la") == 0 || strcmp(cmd, "call") == 0) {
        return TRUE;
    }
    return FALSE;
}

/** Returns true if the command is one of the J commands **/
int isIConditions(char *cmd) {
    if (strcmp(cmd, "beq") == 0 || strcmp(cmd, "bne") == 0 || strcmp(cmd, "blt") == 0 || strcmp(cmd, "bgt") == 0) {
        return TRUE;
    }
    return FALSE;
}

/** Returns the address of a given label in the symbol table **/
int symListLabelAddress(symTableList *symList, char *label) {
    symTableNode *p;
    p = symList->head;
    while (p->next != NULL) {
        if (strcmp(p->symbol, label) == 0) {
            return p->value;
        }
        p = p->next;
    }
    if (strcmp(p->symbol, label) == 0) {
        return p->value;
    }
    return undefinedLabel;
}

/** Inserts the missing address from the first pass in the line **/
int jCmdFixer(tableList *codeTable, symTableList *symList, char *labelParam, int lineNum) {
    tableNode *p;
    jCmd *temp;
    unsigned int addressToInsert = symListLabelAddress(symList, labelParam);
    p = codeTable->head;
    if (symListLabelAddress(symList, labelParam) == undefinedLabel && strcmp(labelParam, "") != 0) return FALSE;
    while (p->next != NULL) {
        if (p->lineNum == lineNum) {
            temp = (jCmd *) p->binaryCode;
            if (temp->address == (unsigned int) UNKNOWN || temp->address == (unsigned int) errorInImmed)
                temp->address = addressToInsert;
            return ok;
        }
        p = p->next;
    }
    return FALSE;
}

/** Inserts the missing address from the first pass in the line **/
int iCmdFixer(tableList *codeTable, symTableList *symList, char *labelParam, int lineNum) {
    tableNode *p;
    iCmd *temp;
    unsigned int addressToInsert = symListLabelAddress(symList, labelParam);
    p = codeTable->head;
    if (symListLabelAddress(symList, labelParam) == undefinedLabel && strcmp(labelParam, "") != 0) return FALSE;
    while (p->next != NULL) {
        if (p->lineNum == lineNum) {
            temp = (iCmd *) p->binaryCode;
            temp->immed = addressToInsert - temp->immed;
            return ok;
        }
        p = p->next;
    }
    return FALSE;
}

/** Check the file's extension to check if its .as **/
int fileExtensionChecker(char *fileName) {
    char *tempChecker;
    tempChecker = calloc(1, sizeof(char) * strlen(fileName));
    if (tempChecker == NULL) {
        fprintf(stderr, "Memory cannot be allocated for fileExtension tempChecker\n");
        perror("\nERROR: ");
        free(tempChecker);
        exit(1);
    }

    strcpy(tempChecker, fileName);
    tempChecker = strstr(fileName, ".");
    if (strcmp(tempChecker, ".as") == 0) return ok;
    return FALSE;
}

/** Counts how many entry labels are there **/
int entryCounter(symTableList *symList) {
    symTableNode *p;
    int count;

    count = 0;
    p = symList->head;
    while (p->next != NULL) {
        if (strcmp(p->attributes, "code, entry") == 0 || strcmp(p->attributes, "data, entry") == 0) count++;
        p = p->next;
    }
    if (p->next == NULL)
        if (strcmp(p->attributes, "code, entry") == 0 || strcmp(p->attributes, "data, entry") == 0) count++;
    return count;
}

/** Counts how many extern labels are there **/
int externCounter(symTableList *symList) {
    symTableNode *p;
    int count;

    count = 0;
    p = symList->head;
    while (p->next != NULL) {
        if (strcmp(p->attributes, "external") == 0) count++;
        p = p->next;
    }
    if (p->next == NULL)
        if (strcmp(p->attributes, "external") == 0) count++;
    return count;
}

/** Checks if there is an alphabet in a given string **/
int containsAlphabet(char *input) {
    int i;

    for (i = 0; i < strlen(input); i++) {
        if (isalpha(input[i]) != 0) return TRUE;
    }
    return FALSE;
}

/** Checks for line syntax for accidental or illegal characters **/
int lineCheckerFunc(char *cmd, char *line) {
    int i, regCounter, commaCounter, dotCounter, illegalChar, quoteCounter, letterCounter, signCounter;
    regCounter = commaCounter = dotCounter = illegalChar = quoteCounter = letterCounter = signCounter = 0;

    for (i = 0; i < strlen(line); i++) {
        if (line[i] == ' ' || line[i] == '\t') continue;
        if (isalpha(line[i]) != 0) letterCounter++;
        if (line[i] == ',') commaCounter++;
        if (line[i] == '.') dotCounter++;
        if (line[i] == '$') regCounter++;
        if (line[i] == '"') quoteCounter++;
        if (line[i] == '-' || line[i] == '+') signCounter++;
        if (line[i] != '$' && line[i] != '.' && line[i] != ',' && line[i] != '"' && line[i] != '-' && line[i] != '+' &&
            isalpha(line[i]) == 0 && isdigit(line[i]) == 0)
            illegalChar++;
    }

    if (illegalChar > 0) return FALSE;
    /* R */
    if (strcmp(cmd, "add") == 0 || strcmp(cmd, "sub") == 0 || strcmp(cmd, "and") == 0 || strcmp(cmd, "or") == 0 ||
        strcmp(cmd, "nor") == 0) {
        if (letterCounter > 3) return FALSE;
        if (regCounter > 3) return FALSE;
        if (commaCounter > 2) return FALSE;
        if (dotCounter > 0) return FALSE;
        if (quoteCounter > 0) return FALSE;
        if (signCounter > 0) return FALSE;
    }
    if (strcmp(cmd, "move") == 0 || strcmp(cmd, "mvhi") == 0 || strcmp(cmd, "mvlo") == 0) {
        if (letterCounter > 4) return FALSE;
        if (regCounter > 2) return FALSE;
        if (commaCounter > 1) return FALSE;
        if (dotCounter > 0) return FALSE;
        if (quoteCounter > 0) return FALSE;
        if (signCounter > 0) return FALSE;
    }

    /* I */
    if (strcmp(cmd, "addi") == 0 || strcmp(cmd, "subi") == 0 || strcmp(cmd, "andi") == 0 || strcmp(cmd, "ori") == 0 ||
        strcmp(cmd, "nori") == 0) {
        if (letterCounter > 4) return FALSE;
        if (regCounter > 2) return FALSE;
        if (commaCounter > 2) return FALSE;
        if (dotCounter > 0) return FALSE;
        if (quoteCounter > 0) return FALSE;
        if (signCounter > 1) return FALSE;
    }
    if (strcmp(cmd, "beq") == 0 || strcmp(cmd, "bne") == 0 || strcmp(cmd, "blt") == 0 || strcmp(cmd, "bgt") == 0) {
        if (letterCounter < 3) return FALSE;
        if (regCounter > 2) return FALSE;
        if (commaCounter > 2) return FALSE;
        if (dotCounter > 0) return FALSE;
        if (quoteCounter > 0) return FALSE;
        if (signCounter > 0) return FALSE;
    }
    if (strcmp(cmd, "lb") == 0 || strcmp(cmd, "sb") == 0 || strcmp(cmd, "lw") == 0 || strcmp(cmd, "sw") == 0 ||
        strcmp(cmd, "lh") == 0 || strcmp(cmd, "sh") == 0) {
        if (letterCounter > 2) return FALSE;
        if (regCounter > 2) return FALSE;
        if (commaCounter > 2) return FALSE;
        if (dotCounter > 0) return FALSE;
        if (quoteCounter > 0) return FALSE;
        if (signCounter > 1) return FALSE;
    }

    /* J */
    if (strcmp(cmd, "jmp") == 0) {
        if (letterCounter < 3) return FALSE;
        if (regCounter > 1) return FALSE;
        if (commaCounter > 0) return FALSE;
        if (dotCounter > 0) return FALSE;
        if (quoteCounter > 0) return FALSE;
        if (signCounter > 0) return FALSE;
    }
    if (strcmp(cmd, "la") == 0) {
        if (letterCounter < 2) return FALSE;
        if (regCounter > 0) return FALSE;
        if (commaCounter > 0) return FALSE;
        if (dotCounter > 0) return FALSE;
        if (quoteCounter > 0) return FALSE;
        if (signCounter > 0) return FALSE;
    }
    if (strcmp(cmd, "call") == 0) {
        if (letterCounter < 4) return FALSE;
        if (regCounter > 0) return FALSE;
        if (commaCounter > 0) return FALSE;
        if (dotCounter > 0) return FALSE;
        if (quoteCounter > 0) return FALSE;
        if (signCounter > 0) return FALSE;
    }
    if (strcmp(cmd, "stop") == 0) {
        if (letterCounter > 4) return FALSE;
        if (regCounter > 0) return FALSE;
        if (commaCounter > 0) return FALSE;
        if (dotCounter > 0) return FALSE;
        if (quoteCounter > 0) return FALSE;
        if (signCounter > 0) return FALSE;
    }

    /* Data */
    if (strcmp(cmd, ".dw") == 0 || strcmp(cmd, ".dh") == 0 || strcmp(cmd, ".db") == 0) {
        if (letterCounter > 4) return FALSE;
        if (regCounter > 0) return FALSE;
        if (dotCounter > 1) return FALSE;
        if (quoteCounter > 0) return FALSE;
    }
    if (strcmp(cmd, ".asciz") == 0) {
        if (letterCounter < 5) return FALSE;
        if (quoteCounter > 2) return FALSE;
    }

    return ok;
}

