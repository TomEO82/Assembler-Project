#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#include <limits.h>
#include "globals.h"

/** Stores all commands' data **/
void structArrayInit(commandStruct arr[]) {
/* add */
    arr[0].name = "add";
    arr[0].type = 'R';
    arr[0].funct = 1;
    arr[0].opcode = 0;

/* sub */
    arr[1].name = "sub";
    arr[1].type = 'R';
    arr[1].funct = 2;
    arr[1].opcode = 0;

/* and */
    arr[2].name = "and";
    arr[2].type = 'R';
    arr[2].funct = 3;
    arr[2].opcode = 0;

/* or */
    arr[3].name = "or";
    arr[3].type = 'R';
    arr[3].funct = 4;
    arr[3].opcode = 0;

/* nor */
    arr[4].name = "nor";
    arr[4].type = 'R';
    arr[4].funct = 5;
    arr[4].opcode = 0;

/* move */
    arr[5].name = "move";
    arr[5].type = 'R';
    arr[5].funct = 1;
    arr[5].opcode = 1;

/* mvhi */
    arr[6].name = "mvhi";
    arr[6].type = 'R';
    arr[6].funct = 2;
    arr[6].opcode = 1;

/* mvlo */
    arr[7].name = "mvlo";
    arr[7].type = 'R';
    arr[7].funct = 3;
    arr[7].opcode = 1;

/* addi */
    arr[8].name = "addi";
    arr[8].type = 'I';
    arr[8].funct = 0;
    arr[8].opcode = 10;

/* subi */
    arr[9].name = "subi";
    arr[9].type = 'I';
    arr[9].funct = 0;
    arr[9].opcode = 11;

/* andi */
    arr[10].name = "andi";
    arr[10].type = 'I';
    arr[10].funct = 0;
    arr[10].opcode = 12;

/* ori */
    arr[11].name = "ori";
    arr[11].type = 'I';
    arr[11].funct = 0;
    arr[11].opcode = 13;

/* nori */
    arr[12].name = "nori";
    arr[12].type = 'I';
    arr[12].funct = 0;
    arr[12].opcode = 14;

/* bne */
    arr[13].name = "bne";
    arr[13].type = 'I';
    arr[13].funct = 0;
    arr[13].opcode = 15;

/* beq */
    arr[14].name = "beq";
    arr[14].type = 'I';
    arr[14].funct = 0;
    arr[14].opcode = 16;

/* blt */
    arr[15].name = "blt";
    arr[15].type = 'I';
    arr[15].funct = 0;
    arr[15].opcode = 17;

/* bgt */
    arr[16].name = "bgt";
    arr[16].type = 'I';
    arr[16].funct = 0;
    arr[16].opcode = 18;

/* lb */
    arr[17].name = "lb";
    arr[17].type = 'I';
    arr[17].funct = 0;
    arr[17].opcode = 19;

/* sb */
    arr[18].name = "sb";
    arr[18].type = 'I';
    arr[18].funct = 0;
    arr[18].opcode = 20;

/* lw */
    arr[19].name = "lw";
    arr[19].type = 'I';
    arr[19].funct = 0;
    arr[19].opcode = 21;

/* sw */
    arr[20].name = "sw";
    arr[20].type = 'I';
    arr[20].funct = 0;
    arr[20].opcode = 22;

/* lh */
    arr[21].name = "lh";
    arr[21].type = 'I';
    arr[21].funct = 0;
    arr[21].opcode = 23;

/* sh */
    arr[22].name = "sh";
    arr[22].type = 'I';
    arr[22].funct = 0;
    arr[22].opcode = 24;

/* jmp */
    arr[23].name = "jmp";
    arr[23].type = 'J';
    arr[23].funct = 0;
    arr[23].opcode = 30;

/* la */
    arr[24].name = "la";
    arr[24].type = 'J';
    arr[24].funct = 0;
    arr[24].opcode = 31;

/* call */
    arr[25].name = "call";
    arr[25].type = 'J';
    arr[25].funct = 0;
    arr[25].opcode = 32;

/* stop */
    arr[26].name = "stop";
    arr[26].type = 'J';
    arr[26].funct = 0;
    arr[26].opcode = 63;
}

/** Retrieves the command name**/
char *cmdNameIdentifier(int length, char *cmdCopy) {
    return strtok(cmdCopy + length, " \t");
}

/** Retrieves the parameters section of the input line **/
char *paramsIdentifier(char *cmd, char *paramCopy) {
    char *check;
    if (isBackSlashN(paramCopy))
        paramCopy[strlen(paramCopy) - 1] = '\0';
    if (strcmp(cmd, ".asciz") == 0) check = strstr(paramCopy, "\"");
    else {
        if (checkJCmds(cmd)) {
            check = paramCopy + strlen(cmd);
            check = strtok(check, " \t");
            return check;
        }
        check = strstr(paramCopy, cmd);
        check = strtok(check, cmd);
    }
    return check;
}

/** Checks for the .extern or the .entry command **/
char *labelExEnIdentifier(char *input, int isExtern, int isEntry) {
    char *tempInput;
    char *label;
    if (isExtern) tempInput = ".extern";
    if (isEntry) tempInput = ".entry";
    input = strstr(input, ".");
    if (isExtern) {
        label = strtok(input + strlen(tempInput), " \t");
        if (isBackSlashN(label))
            label[strlen(label) - 1] = '\0';
    }
    if (isEntry) {
        label = strtok(input + strlen(tempInput), " \t");
        if (isBackSlashN(label))
            label[strlen(label) - 1] = '\0';
    }
    return label;
}

/** Identifies the registers in an input line and creates the registers array **/
int *registerIdentifier(char *params) {
    int i, avail, regNum;
    char *p;
    int *tempRegister = calloc(1, 3 * sizeof(int));
    if (tempRegister == NULL) {
        fprintf(stderr, "Memory cannot be allocated to tempRegisters array\n");
        perror("\nERROR: ");
        free(tempRegister);
        exit(1);
    }
    memset(tempRegister, -1, 3 * sizeof(int));
    avail = 0;
    for (i = 0; i < strlen(params); i++) {
        if (params[i] == '$' && isdigit(params[i + 1])) {
            regNum = (int) strtol(&params[i + 1], &p, 10);
            if (regNum < 0 || regNum > 31) return NULL;
            tempRegister[avail++] = regNum;
        }
    }
    return tempRegister;
}

/** Identifies the numbers in an input line and creates the numbers array **/
int *numbersIdentifier(char *params, errTableList *errList, int lineNum) {
    int i, count;
    char *p, **temp;
    int *finalNums;
    temp = NULL;
    int *nums = calloc(1, MAX_LINE_SIZE * sizeof(int));
    char inputCopyForNumbers[MAX_LINE_SIZE];
    if (!containNums(params)) return NULL; /* Checks if the parameters contains numbers */
    strcpy(inputCopyForNumbers, params);
    numCounter = count = i = 0;
    if (nums == NULL) {
        printf("\nnums could not be allocated\n");
        free(nums);
        exit(1);
    }
    memset(nums, '$', MAX_LINE_SIZE * sizeof(int));
    p = strtok(inputCopyForNumbers, ",");

    while (p != NULL) {
        if (containsRegSign(p) || isalpha(p[0]) != 0 || containsAlphabet(p)) p = strtok(NULL, ",");
        else {
            if (strtol(p, temp, 10) > INT_MAX || strtol(p, temp, 10) < INT_MIN) {
                addErrorNode(errList, lineNum, numberOutOfBounds);
            }
            nums[i++] = (int) strtol(p, temp, 10);
            p = strtok(NULL, ",");
        }
    }

    for (i = 0; i < MAX_LINE_SIZE; i++) {
        if (nums[i] != DOLLAR_SIGN) count++;
    }
    numCounter = count;
    finalNums = calloc(1, count * sizeof(int) + 1);
    if (finalNums == NULL) {
        printf("\nfinalNums could not be allocated\n");
        free(finalNums);
        exit(1);
    }
    memset(finalNums, '$', count * sizeof(int) + 1);
    for (i = 0; i < count; i++) {
        finalNums[i] = nums[i];
    }
    free(nums);
    return finalNums;
}

/** Checks and returns the label name from the parameters array **/
char *labelParamIdentifier(char *params, char *cmd) {
    char *temp = NULL;
    int i;
    char paramCopy[strlen(params)];
    if (isBackSlashN(params))
        params[strlen(params) - 1] = '\0';
    strcpy(paramCopy, params);
    if (checkJCmds(cmd)) {
        temp = paramCopy + strlen(cmd);
        temp = strtok(temp, " \t");
        return temp;
    }
    temp = strtok(paramCopy, cmd);
    for (i = 0; i < strlen(temp); i++) {
        if (isalpha(temp[i]) != 0) {
            temp = strstr(temp, params + strlen(cmd) + i);
            temp = labelTrimmer(temp);
            break;
        }
    }
    return temp;
}

/** Prints R command types from binary to hex for output files **/
void rCmdBinaryToHex(rCmd line, FILE *f) {
    int i;
    unsigned int final32Bit = 0; /** Initializing an empty 32 bit variable **/
    unsigned int temp;

    /** Copying each segment from the R command line to the 32 bit variable **/
    temp = line.opcode;
    final32Bit |= (temp << 26);
    temp = line.rs;
    final32Bit |= (temp << 21); 
    temp = line.rt;
    final32Bit |= (temp << 16); 
    temp = line.rd;
    final32Bit |= (temp << 11); 
    temp = line.funct;
    final32Bit |= (temp << 6); 
    temp = line.notUsed;
    final32Bit |= (temp); 

    /** Printing the 32 bit variable to the output file **/
    for (i = 0; i < 4; i++) {
        fprintf(f, "%X", (final32Bit & 0xF0) >> 4);
        fprintf(f, "%X ", final32Bit & 0x0F);
        /*Moving to the next 8 bytes */
        final32Bit >>= 8;
    }
}

/** Prints I command types from binary to hex for output files **/
void iCmdBinaryToHex(iCmd line, FILE *f) {
    int i;
    unsigned int final32Bit = 0;
    unsigned int temp;

    temp = line.opcode;
    final32Bit |= (temp << 26);
    temp = line.rs;
    final32Bit |= (temp << 21);
    temp = line.rt;
    final32Bit |= (temp << 16);
    temp = line.immed;
    final32Bit |= (temp);

    for (i = 0; i < 4; i++) {
        fprintf(f, "%X", (final32Bit & 0xF0) >> 4);
        fprintf(f, "%X ", final32Bit & 0x0F);
        final32Bit >>= 8;
    }
}

/** Prints J command types from binary to hex for output files **/
void jCmdBinaryToHex(jCmd line, FILE *f) {
    int i;
    unsigned int final32Bit = 0;
    unsigned int temp;

    temp = line.opcode;
    final32Bit |= (temp << 26);
    temp = line.reg;
    final32Bit |= (temp << 25);
    temp = line.address;
    final32Bit |= (temp);


    for (i = 0; i < 4; i++) {
        fprintf(f, "%X", (final32Bit & 0xF0) >> 4);
        fprintf(f, "%X ", final32Bit & 0x0F);
        final32Bit >>= 8;
    }
}

/** Prints .asciz command types from binary to hex for output files **/
void ascizCmdBinaryToHex(asciz line, FILE *f) {
    uint8_t final8Bit = 0;

    final8Bit |= (uint8_t) line.binary;

    fprintf(f, "%X", (final8Bit & 0xF0) >> 4);
    fprintf(f, "%X ", final8Bit & 0x0F);
}

/** Prints .dh command types from binary to hex for output files **/
int dhCmdBinaryToHex(dh line, FILE *f, int tempIC) {
    int i, countFlag;
    uint16_t final16Bit = 0;
    countFlag = FALSE;

    final16Bit |= (uint16_t) line.binary;

    for (i = 0; i < 2; i++) {
        if (dataCount == 5) {
            fprintf(f, "\n");
            tempIC += 4;
            fprintf(f, "0%d  ", tempIC);
            dataCount = 1;
            countFlag = TRUE;
        }
        fprintf(f, "%X", (final16Bit & 0xF0) >> 4);
        fprintf(f, "%X ", final16Bit & 0x0F);
        final16Bit >>= 8;
        dataCount++;
    }
    return countFlag;
}
    
/** Prints .dw command types from binary to hex for output files **/
int dwCmdBinaryToHex(dw line, FILE *f, int tempIC) {
    int i, countFlag;
    uint32_t final32Bit = 0;
    countFlag = FALSE;

    final32Bit |= (uint32_t) line.binary;

    for (i = 0; i < 4; i++) {
        if (dataCount == 5) {
            fprintf(f, "\n");
            tempIC += 4;
            fprintf(f, "0%d  ", tempIC);
            dataCount = 1;
            countFlag = TRUE;
        }
        fprintf(f, "%X", (final32Bit & 0xF0) >> 4);
        fprintf(f, "%X ", final32Bit & 0x0F);
        final32Bit >>= 8;
        dataCount++;
    }
    return countFlag;
}

