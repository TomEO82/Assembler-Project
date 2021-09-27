#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "globals.h"

/** First pass function handling the firs pass algorithm **/
void firstPass(errTableList *errorList, tableList *codeTable, tableList *dataTable, symTableList *symTable, FILE *fd) {
    static int IC;
    /** Inputs definition **/
    char input[MAX_LINE_SIZE];
    char lblCopy[MAX_LINE_SIZE];
    char cmdCopy[MAX_LINE_SIZE];
    char paramCopy[MAX_LINE_SIZE];
    char lineChecker[MAX_LINE_SIZE + 2];
    char lineCheckerCopy[MAX_LINE_SIZE + 2];
    char labelParam[MAX_LABEL_SIZE];

    /** Local variables definitions and init **/
    int errorCode, lineNum;
    char *labelName = NULL;
    char *tempLabel = NULL;
    char *cmd = NULL;
    char *params = NULL;
    char *ascizStr = NULL;
    int *regs;
    int *nums;
    int isExternFlag, isEntryFlag;
    lineNum = 1;
    isExternFlag = isEntryFlag = FALSE;
    numCounter = 0;
    IC = IC_START_ADDRESS;
    DC = DC_START_ADDRESS;

    /** Local structures definitions and init **/
    /* Commands Struct */
    commandListInit();

    /** Loop going over each line of the file given **/
    while (fgets(lineChecker, MAX_LINE_SIZE + 2, fd) != NULL) {
        strcpy(lineCheckerCopy, lineChecker);
        if (lineChecker[0] == '\n' || isCommentCheck(lineChecker) || strtok(lineCheckerCopy, " \t\n") == NULL) { /** If empty line **/
            if (isComment(lineChecker) == errorInLineSyntax) {
                errorCode = errorInLineSyntax;
                addErrorNode(errorList, lineNum, errorCode);
            }
            lineNum++;
            continue;
        }
        if (lineSizeException((int) strlen(lineChecker)) != ok) {
            errorCode = lineSizeException((int) strlen(lineChecker));
            addErrorNode(errorList, lineNum, errorCode);
        }
        if (mulCommaChecker(lineChecker) != ok) {
            errorCode = mulCommaChecker(lineChecker);
            addErrorNode(errorList, lineNum, errorCode);
        }
        strncpy(input, lineChecker, MAX_LINE_SIZE - 1);

        labelParam[0] = '\0';
        isExternFlag = isEntryFlag = 0;
        labelName = NULL;
        cmd = NULL;
        regs = NULL;
        nums = NULL;
        ascizStr = NULL;


        /** Check for entry or extern lines **/
        strcpy(lblCopy, input);
        if (isExtern(input) == ok) {
            if (isComma(input)) {
                errorCode = illegalComma;
                addErrorNode(errorList, lineNum, errorCode);
            }
            if (isLabel(input)) {
                tempLabel = strtok(lblCopy, ":");
                if (labelChecker(tempLabel) != ok) {
                    errorCode = labelChecker(tempLabel);
                    addErrorNode(errorList, lineNum, errorCode);
                }
                labelName = labelExEnIdentifier(input + strlen(tempLabel) + 1, TRUE, FALSE);
            } else labelName = labelExEnIdentifier(input, TRUE, FALSE);
            isExternFlag = TRUE;
        } else if (isEntry(input) == ok) {
            if (isComma(input)) {
                errorCode = illegalComma;
                addErrorNode(errorList, lineNum, errorCode);
            }
            if (isLabel(input)) {
                tempLabel = strtok(lblCopy, ":");
                if (labelChecker(tempLabel) != ok) {
                    errorCode = labelChecker(tempLabel);
                    addErrorNode(errorList, lineNum, errorCode);
                }
                labelName = labelExEnIdentifier(input + strlen(tempLabel) + 1, FALSE, TRUE);
            } else labelName = labelExEnIdentifier(input, FALSE, TRUE);
            isEntryFlag = TRUE;
            if (paramCmdCheck(cmd, regs, nums) == checkForExternEntry) {
                if (entryExternSyntaxChecker(lineChecker, labelName) == incorrectParamAmount) {
                    errorCode = incorrectParamAmount;
                    addErrorNode(errorList, lineNum, errorCode);
                }
            }
            lineNum++;
            continue; /* 10 */
        }
            /** Check for non extern/entry expressions **/
        else {
            strcpy(lblCopy, input);
            strcpy(cmdCopy, input);
            strcpy(paramCopy, input);

            /** Check for label **/
            if (isLabel(input) == TRUE) {
                labelName = strtok(lblCopy, ":");
                if (labelChecker(labelName) != ok) {
                    errorCode = labelChecker(labelName);
                    addErrorNode(errorList, lineNum, errorCode);
                }
                cmd = cmdNameIdentifier((int) strlen(labelName) + 1, cmdCopy);
            } else cmd = strtok(cmdCopy, " \t");
            if (cmd == NULL) {
                errorCode = undefinedCmdName;
                addErrorNode(errorList, lineNum, errorCode);
                continue;
            }
            if (isBackSlashN(cmd))
                cmd[strlen(cmd) - 1] = '\0';
            if ((labelName != NULL && lineCheckerFunc(cmd, lineChecker + strlen(labelName) + 1) == FALSE) || 
            	(labelName == NULL && lineCheckerFunc(cmd, lineChecker) == FALSE)) {
            	errorCode = illegalCharacter;
            	addErrorNode(errorList, lineNum, errorCode);	
            }
            params = paramsIdentifier(cmd, paramCopy);
            if (strcmp(cmd, "stop") != 0) {
                if (params == NULL) {
                    errorCode = incorrectParamAmount;
                    addErrorNode(errorList, lineNum, errorCode);
                } else {
                    if (strcmp(cmd, ".asciz") == 0) ascizStr = paramsIdentifier(cmd, params);
                    else {
                        if (registerIdentifier(params) == NULL) {
                            errorCode = registerNumNotDefined;
                            addErrorNode(errorList, lineNum, errorCode);
                            continue;
                        } else regs = registerIdentifier(params);
                        nums = numbersIdentifier(params, errorList, lineNum);
                        if (isParamsContainLabel(paramCopy, cmd) && labelName == NULL)
                            strcpy(labelParam, labelParamIdentifier(paramCopy, cmd));
                        if (isParamsContainLabel(paramCopy, cmd) && labelName != NULL)
                            strcpy(labelParam, labelParamIdentifier(paramCopy + strlen(labelName) + 1, cmd));
                    }
                }
            }
            if (cmdChecker(cmd) != ok) {
                errorCode = cmdChecker(cmd);
                addErrorNode(errorList, lineNum, errorCode);
            }
        }

        /** Adds label to symbol list **/
        if (labelName != NULL && labelChecker(labelName) == ok) { /* 12 */
            if (cmd != NULL && isDataCmd(cmd)) {
                if (addSymNode(symTable, DC, labelName, "data") == labelAlreadyDefined) {
                    errorCode = labelAlreadyDefined;
                    addErrorNode(errorList, lineNum, errorCode);
                }
            } else if (isExternFlag) {
                addSymNode(symTable, 0, labelName, "external");
            } else if (addSymNode(symTable, IC, labelName, "code") == labelAlreadyDefined) {
                errorCode = labelAlreadyDefined;
                addErrorNode(errorList, lineNum, errorCode);
            }
        }

        /** Checking correct amount of parameters to each command **/
        if (paramCmdCheck(cmd, regs, nums) == checkForExternEntry) { /* 14 */
            if (entryExternSyntaxChecker(lineChecker, labelName) == incorrectParamAmount) {
                errorCode = incorrectParamAmount;
                addErrorNode(errorList, lineNum, errorCode);
            }
        } else if (paramCmdCheck(cmd, regs, nums) == FALSE) {
            errorCode = incorrectOperandForCmd;
            addErrorNode(errorList, lineNum, errorCode);
        }


        /** Adding line to the code or data list **/
        if (isExternFlag == FALSE && isDataCmd(cmd) == FALSE) { /* 15 */
            if (addCodeNode(codeTable, symTable, errorList, labelParam, lineChecker, cmd, regs, nums, IC, lineNum) ==
                FALSE) {
                errorCode = codeTableLineNotBuilt;
                addErrorNode(errorList, lineNum, errorCode);
            }
        } else if (isExternFlag == 0 && isDataCmd(cmd)) {
            if (addDataNode(dataTable, errorList, lineChecker, cmd, nums, lineNum, ascizStr) == FALSE) {
                errorCode = dataTableLineNotBuilt;
                addErrorNode(errorList, lineNum, errorCode);
            }
        }

        if (isExternFlag == 0 && isDataCmd(cmd) == FALSE)
            IC += 4; /* 16 */


        lineNum++;
    }

    ICF = IC;
    DCF = DC;

    if (dataTable->head != NULL) updateDataListICF(dataTable);
    if (symTable->head != NULL) updateSymListICF(symTable);
}

