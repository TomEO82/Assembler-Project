#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "globals.h"

/** Second pass function for second pass algorithm **/
void secondPass(errTableList *errorList, tableList *codeTable, symTableList *symTable, FILE *fd) {

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
    char *cmd = NULL;
    lineNum = 1;
    numCounter = 0;
    rewind(fd); /** Returns to the top of the file from first pass **/

    /** Main loop for passing each line on the file **/
    while (fgets(lineChecker, MAX_LINE_SIZE + 2, fd) != NULL) {
        strcpy(lineCheckerCopy, lineChecker);
        if (lineChecker[0] == '\n' || isCommentCheck(lineChecker) || strtok(lineCheckerCopy, " \t\n") == NULL) {
            lineNum++;
            continue;
        }

        labelParam[0] = '\0';
        labelName = NULL;
        cmd = NULL;

        strncpy(input, lineChecker, MAX_LINE_SIZE - 1);
        strcpy(lblCopy, input);
        strcpy(cmdCopy, input);
        strcpy(paramCopy, input);


        if (isLabel(input) == TRUE) {
            labelName = strtok(lblCopy, ":");
            cmd = cmdNameIdentifier((int) strlen(labelName) + 1, cmdCopy);
        } else cmd = strtok(cmdCopy, " \t");
        if (cmd != NULL && isBackSlashN(cmd))
            cmd[strlen(cmd) - 1] = '\0';

        if (isExtern(input) == ok || isDataCmd(cmd)) {
            lineNum++;
            continue;
        }

        if (isEntry(input) == ok) { /* [6] */
            labelName = labelExEnIdentifier(input, FALSE, TRUE);
            if (addEntryToSymList(symTable, labelName) == FALSE) {
                errorCode = entryLabelNotDefined;
                addErrorNode(errorList, lineNum, errorCode);
            }
        } else { /* This is a command line [7] */
            if (strcmp(cmd, "stop") != 0) {
                if (isParamsContainLabel(paramCopy, cmd) && labelName == NULL)
                    strcpy(labelParam, labelParamIdentifier(paramCopy, cmd));
                else if (isParamsContainLabel(paramCopy, cmd) && labelName != NULL)
                    strcpy(labelParam, labelParamIdentifier(paramCopy + strlen(labelName) + 1, cmd));
            }
            if (isJCmd(cmd)) {
                if (jCmdFixer(codeTable, symTable, labelParam, lineNum) == FALSE) {
                    errorCode = cannotInsertUnknownAddress;
                    addErrorNode(errorList, lineNum, errorCode);
                }
            }
            if (isIConditions(cmd)) {
                if (labelAddressSymTableFinder(symTable, labelParam) == errorInImmed) {
                    errorCode = labelIsDefinedAsExtern;
                    addErrorNode(errorList, lineNum, errorCode);
                } else if (iCmdFixer(codeTable, symTable, labelParam, lineNum) == FALSE) {
                    errorCode = cannotInsertUnknownAddress;
                    addErrorNode(errorList, lineNum, errorCode);
                }
            }
        }
        lineNum++;
    }
}
