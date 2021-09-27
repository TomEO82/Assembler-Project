#ifndef MMN14_GLOBALS_H
#define MMN14_GLOBALS_H

#define TRUE 1
#define FALSE 0

/* Defines the maximum line length from the file */
#define MAX_LINE_SIZE 81

/* Defines the maximum line length from the file */
#define MAX_LABEL_SIZE 31

/* Total number of commands */
#define CODE_COMMANDS_LENGTH 27
/* Total number of commands */
#define DATA_COMMANDS_LENGTH 4

/* Max length of a command */
#define MAX_CMD_SIZE 6

/* The address that the code and data memory starts at */
#define IC_START_ADDRESS 100
#define DC_START_ADDRESS 0

/* Acts as a question mark for the first run */
#define UNKNOWN '0'

/* Color codes for the errors */
#define RED   "\x1B[31m"
#define RESET "\x1B[0m"

/* Global variables */
int numCounter;
int DC, ICF, DCF, dataCount;

/* Max numbers for data lines */

#define MAX_HALF_WORD_NUM_POS 32768
#define MAX_BYTE_NUM_POS 128


#define MAX_HALF_WORD_NUM_NEG (-32769)
#define MAX_BYTE_NUM_NEG (-129)


#define DOLLAR_SIGN 606348324

/** Basic array structure for command array **/
typedef struct commandStruct {
    char *name;
    char type;
    int funct;
    int opcode;
} commandStruct;

/** List of all errors **/
typedef enum errors {
    lineExceededLimit = 0,
    undefinedCmdName = 1,
    incorrectParamAmount = 2,
    incorrectOperandForCmd = 3,
    registerNumNotDefined = 4,
    undefinedLabel = 5,
    labelAlreadyDefined = 6,
    numberOutOfBounds = 7,
    labelTooLarge = 8,
    illegalComma = 9,
    multipleConsecCommas = 10,
    errorInLineSyntax = 11,
    labelIsDefinedAsExtern = 12,
    jmpTooBig = 13,
    codeTableLineNotBuilt = 14,
    dataTableLineNotBuilt = 15,
    entryLabelNotDefined = 16,
    cannotInsertUnknownAddress = 17,
    labelIsBuiltInCmd = 18,
    illegalCharacter = 19,
    errorInImmed = 98,
    checkForExternEntry = 99,
    ok = 100
} errors;

/** Basic structures for each type of command **/
typedef struct rCmd {
    unsigned int opcode: 6;
    unsigned int rs: 5;
    unsigned int rt: 5;
    unsigned int rd: 5;
    unsigned int funct: 5;
    unsigned int notUsed: 6;
} rCmd;

typedef struct iCmd {
    unsigned int opcode: 6;
    unsigned int rs: 5;
    unsigned int rt: 5;
    unsigned int immed: 16;
} iCmd;

typedef struct jCmd {
    unsigned int opcode: 6;
    unsigned int reg: 1;
    unsigned int address: 25;
} jCmd;

typedef struct db {
    unsigned int binary: 8;
} db;

typedef struct dh {
    unsigned int binary: 16;
} dh;

typedef struct dw {
    unsigned int binary: 32;
} dw;

typedef struct asciz {
    unsigned int binary: 8;
} asciz;

/*** Main table structs ***/
/* Defines a row, as a node, in the main table */
typedef struct tableNode {
    int lineNum;
    int address;
    char *srcCode;
    void *binaryCode;
    char cmdType;
    struct tableNode *next;
} tableNode;

/* The List of the table nodes */
typedef struct tableList {
    tableNode *head;
} tableList;


/*** Symbol table structs ***/
/* Defines a row, as a node, in the symbol table */
typedef struct symTableNode {
    char *symbol;
    int value;
    char *attributes;
    struct symTableNode *next;
} symTableNode;

/* The List of the table nodes in the symbol table */
typedef struct symTableList {
    symTableNode *head;
} symTableList;


/*** Error table structs ***/
/* Defines a row, as a node, in the error table */
typedef struct errTableNode {
    char *lineNum;
    char *err;
    struct errTableNode *next;
} errTableNode;

/* The List of the table nodes in the error table */
typedef struct errTableList {
    errTableNode *head;
    int count;
} errTableList;


/****************************** FUNCTIONS ******************************/
void firstPass(errTableList *errorList, tableList *mainTable, tableList *dataTable, symTableList *symTable, FILE *fd);

void secondPass(errTableList *errorList, tableList *codeTable, symTableList *symTable, FILE *fd);

/****************************** HELPER.C FUNCTIONS ******************************/
void structArrayInit(commandStruct arr[]);

char *cmdNameIdentifier(int length, char *cmdCopy);

char *labelExEnIdentifier(char *input, int isExtern, int isEntry);

char *paramsIdentifier(char *cmd, char *paramCopy);

int *registerIdentifier(char *params);

int *numbersIdentifier(char *params, errTableList *errList, int lineNum);

char *labelParamIdentifier(char *params, char *cmd);

void rCmdBinaryToHex(rCmd line, FILE *f);

void iCmdBinaryToHex(iCmd line, FILE *f);

void jCmdBinaryToHex(jCmd line, FILE *f);

void ascizCmdBinaryToHex(asciz line, FILE *f);

int dhCmdBinaryToHex(dh line, FILE *f, int tempIC);

int dwCmdBinaryToHex(dw line, FILE *f, int tempIC);


/****************************** PARSER.C FUNCTIONS ******************************/
void commandListInit();

void addErrorNode(errTableList *list, int lineNumber, int errorCode);

char *errorText(int errCode);

void printErrorList(errTableList *list, char *fileName);

void printSymbolList(symTableList *list);

void printCodeList(tableList *codeList);

void printDataList(tableList *dataList);

int addCodeNode(tableList *codeList, symTableList *symList, errTableList *errList, char labelParam[MAX_LABEL_SIZE],
                char *line, char *cmd, int *regs, int *nums, int IC, int lineNum);

int addDataNode(tableList *dataList, errTableList *errList, char *line, char *cmd, int *nums,
                int lineNum, char *ascizStr);

int addSymNode(symTableList *symList, int IC, char *labelName, char *attr);

int isLabelExist(char *label, symTableList *symList);

int isLabel(char *input);

int lineSizeException(int length);

int labelChecker(char *label);

int cmdChecker(char *cmd);

int isCommaF(char *str);

int isComma(char *str);

int mulCommaChecker(char *input);

int isExtern(char *input);

int isEntry(char *input);

int isBackSlashN(char *str);

int entryExternSyntaxChecker(char *input, char *labelName);

int onlyContains(char *haystack, char *needle);

int containsRegSign(char *str);

int countNums(const int *nums);

int isParamsContainLabel(char *params, char *cmd);

char *labelTrimmer(char *label);

int checkJCmds(char *cmd);

int containNums(char *params);

int isComment(char *input);

int isCommentCheck(char *input);

int isDataCmd(char *cmd);

int regsCounter(const int *regs);

int paramCmdCheck(char *cmd, int *regs, const int *nums);

int cmdOpCodeFinder(char *cmd);

int cmdFunctCodeFinder(char *cmd);

char cmdTypeFinder(char *cmd);

rCmd *rCmdBuilder(int *regs, char *cmd);

void printRCmd(rCmd *r);

iCmd *
iCmdBuilder(errTableList *errList, symTableList *list, char labelParam[MAX_LABEL_SIZE], int *regs, int *nums, char *cmd,
            int IC, int lineNum);

void printICmd(iCmd *node);

int labelAddressSymTableFinder(symTableList *list, char *label);

jCmd *jCmdBuilder(symTableList *list, char labelParam[MAX_LABEL_SIZE], int *regs, char *cmd);

void printJCmd(jCmd *node);

void updateDataListICF(tableList *dataList);

void updateSymListICF(symTableList *symList);

int fileExtensionChecker(char *fileName);

int entryCounter(symTableList *symList);

int externCounter(symTableList *symList);

int containsAlphabet(char *input);

int lineCheckerFunc(char *cmd, char *line);

/*************************************************************** SECOND PASS FUNCTIONS ***************************************************************/

int addEntryToSymList(symTableList *symList, char *label);

int isJCmd(char *cmd);

int isIConditions(char *cmd);

int jCmdFixer(tableList *codeTable, symTableList *symList, char *labelParam, int lineNum);

int iCmdFixer(tableList *codeTable, symTableList *symList, char *labelParam, int lineNum);

/*************************************************************** OUTPUTFILES FUNCTIONS ***************************************************************/


void obFile(tableList *codeList, tableList *dataList, char *fileName);

void entryOutFile(symTableList *list, char *fileName);

void externOutFile(symTableList *symList, tableList *codeList, char *fileName);

#endif
