/*
 * This module is meant to handle the parsing of commands received from the user.
 * It receives the commands from the game.c module and interprets them to see if they are valid,
 * have the right number of parameters and that the parameters are in the correct range.
 * If the command is valid, it runs the matching command by calling to the right function from the game.c module.
 * If it is not valid, it outputs an informative message to the user, saying what ought to be fixed.
 */

#include <stdio.h>
#include <string.h>
#include "game.h"
#include "main_aux.h"
#include "util/board_manager.h"

/*
 * This function checks that the token received is an integer.
 * If *token == '\0', it returns 1.
 */
int isAllDigits(char *token){
    char tav;
    tav = *token;

    if (tav == '\0'){
        return 1;
    }
    if ((tav != '-') && (tav < '0' || tav > '9') ){
        return 0;
    }
    token++;
    while ((tav = *token) != '\0'){
        if (tav < '0' || tav > '9'){
            return 0;
        }
        token++;
    }
    return 1;
}

/*
 * This function checks that the token received is a float.
 */
int isAFloat(char *token){
    char tav;
    tav = *token;

    if (tav == '-'){
        token++;
        tav = *token;
        if (tav < '0' || tav > '9')
            return 0;
    }
    else{
        if (tav < '0' || tav > '9')
            return 0;
    }

    token++;
    while((tav = *token) != '.'){
        if (tav == '\0'){ /* if it is an integer - only digits */
            return 1;
        }
        if (tav < '0' || tav > '9'){
            return 0;
        }
        token++;
    }
    /* tav == '.' */
    token++;
    tav = *token;
    if ((tav == '\0') || (tav < '0') || (tav > '9')){
        /* we require to have digits after the decimal point */
        return 0;
    }
    token++;
    return isAllDigits(token);
}

/*
 * This method assumes the command entered is solve,
 * checks the validity of the rest of the command and executes it.
 * Available in every mode.
 * It returns 0 if we need to continue, and -1 if we need to terminate.
 */
int interpretSolve(char *token, struct sudokuManager **pBoard){

    token = strtok(NULL, " \t\r\n");

    if(token == NULL){ /* not enough parameters */
        printFewParams(1, 0);
        return 0;
    }
    else{
        if (strtok(NULL, " \t\r\n") != NULL){ /* too many parameters */
           printExtraParams(1, 0);
           return 0;
        }
        else{
            return solve(pBoard, token);
        }
    }
}

/*
 * This method assumes the command entered is edit,
 * checks the validity of the rest of the command and executes it.
 * Available in every mode.
 * It returns 0 if we need to continue, and -1 if we need to terminate.
 */
int interpretEdit(char *token, struct sudokuManager **pBoard){
    int arrNumOfParams[2] = {0, 1};

    token = strtok(NULL, " \t\r\n");

    if (strtok(NULL, " \t\r\n") != NULL){ /* too many parameters */
        printExtraParamsExtend(arrNumOfParams, 2, 1);
        return 0;
    }
    else {
        /* sending NULL in token if no parameters were entered. */
        return edit(pBoard, token);
    }
}

/*
 * This method assumes the command entered is mark_errors,
 * checks the validity of the rest of the command and executes it.
 * Available in Solve mode only.
 * It returns 0.
 */
int interpretMarkErrors(char *token, enum Mode mode){
    enum Mode availableModes[1] = {Solve};
    int input, check;
    if (mode != Solve){
        printUnavailableMode(2, mode, availableModes, 1);
        return 0;
    }
    token = strtok(NULL, " \t\r\n");
    if(token == NULL){
        printFewParams(1, 2);
        return 0;
    }
    else{
        if (strtok(NULL, " \t\r\n") != NULL){
            /* too many parameters */
            printExtraParams(1, 2);
            return 0;
        }
        else{
            check = isAllDigits(token);
            if (!check){
                printNotANumber(1);
                return 0;
            }
            check = sscanf(token, "%d", &input);
            if (check == 1){ /* sanity check */
                if (input == 0 || input == 1){
                    markErrors(input);
                    return 0;
                }
                else{
                    printWrongRangeInt(2, input, 1);
                    printf("The parameter should be only 1 or 0.\n");
                    return 0;
                }
            }
            printNotANumber(1);
            return 0;
        }
    }
}

/*
 * This method receives a token - the name of the command,
 * the current mode, the index of the command,
 * its available modes and the length of availableModes.
 * If the current mode is unavailable, it returns -1.
 * If there are any other arguments it also returns -1.
 * Otherwise, it returns 0.
 */
int interpretNoArguments(char *token, enum Mode mode, int indexCommand,
                         enum Mode *availableModes, int len){
    int i;
    int isAvailable = 0;

    for (i = 0; i < len; i++){
        if (availableModes[i] == mode){
            isAvailable = 1;
        }
    }

    if (isAvailable == 0){
        printUnavailableMode(indexCommand, mode, availableModes, len);
        return -1;
    }

    token = strtok(NULL, " \t\r\n");

    if (token != NULL){ /* too many parameters */
        printExtraParams(0, indexCommand);
        return -1;
    }

    return 0;
}

/*
 * This method assumes the command entered is print_board,
 * checks the validity of the rest of the command and executes it.
 * Available in Solve and Edit modes.
 * It returns 0.
 */
int interpretPrintBoard(char *token, struct sudokuManager *board, enum Mode mode) {
    enum Mode availableModes[2] = {Solve, Edit};

    if (interpretNoArguments(token, mode, 3, availableModes, 2) == -1){
        return 0;
    }
    else{
        printBoard(board);
        return 0;
    }
}

/*
 * This method assumes the command entered is set, checks the validity of the rest of the command and executes it.
 * Available in Solve and Edit modes.
 * It returns 0 if we need to continue, and -1 if we need to terminate.
 */
int interpretSet(char *token, struct sudokuManager *board, enum Mode mode){
    int i = 0;
    int arrInput[3], arrCheck[3];
    /* arrCheck saves whether we succeeded in converting
     * the string into a number for every parameter */
    int len;
    enum Mode availableModes[2] = {Solve, Edit};
    if (mode == Init) {
        printUnavailableMode(4, mode, availableModes, 2);
        return 0;
    }
    while ((token = strtok(NULL, " \t\r\n")) != NULL && i < 3){
        arrCheck[i] = isAllDigits(token);
        sscanf(token, "%d", &arrInput[i]);
        i++;
    }
    if (i < 3){ /* Not enough parameters */
        printFewParams(3, 4);
        return 0;
    }
    else {
        if (token != NULL) { /* Too many parameters */
            printExtraParams(3, 4);
            return 0;
        } else {
            len = boardLen(board);
            if (!arrCheck[0]) {
                printNotANumber(1);
                return 0;
            } else {
                if (!((arrInput[0] - 1 >= 0) && (arrInput[0] - 1 < len))) {
                    printWrongRangeInt(4, arrInput[0], 1);
                    printRangeInt(1, len, "positive");
                    return 0;
                } else {
                    if (!arrCheck[1]) {
                        printNotANumber(2);
                        return 0;
                    } else {
                        if (!((arrInput[1] - 1 >= 0) && (arrInput[1] - 1 < len))) {
                            printWrongRangeInt(4, arrInput[1], 2);
                            printRangeInt(1, len, "positive");
                            return 0;
                        } else {
                            if (!arrCheck[2]) {
                                printNotANumber(2);
                                return 0;
                            } else {
                                if (!(isLegalCellValue(board, arrInput[2]))) {
                                    printWrongRangeInt(4, arrInput[2], 3);
                                    printRangeInt(0, len, "non-negative");
                                    return 0;
                                } else { /* all parameters are valid */
                                    return set(board, arrInput[0], arrInput[1], arrInput[2]);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

/*
 * This method assumes the command entered is validate,
 * checks the validity of the rest of the command and executes it.
 * Available in Solve and Edit modes.
 * It returns 0 if we need to continue, and -1 if we need to terminate.
 */
int interpretValidate(char *token, struct sudokuManager *board, enum Mode mode) {
    enum Mode availableModes[2] = {Solve, Edit};
    if (interpretNoArguments(token, mode, 5, availableModes, 2) == -1){
        return 0;
    }
    else{
        return validate(board);
    }
}

/*
 * This method assumes the command entered is guess,
 * checks the validity of the rest of the command and executes it.
 * Available only in Solve mode.
 * It returns 0 if we need to continue, and -1 if we need to terminate.
 */
int interpretGuess(char *token, struct sudokuManager *board, enum Mode mode) {
    enum Mode availableModes[1] = {Solve};
    int check;
    float input;

    if (mode != Solve) {
        printUnavailableMode(6, mode, availableModes, 1);
        return 0;
    }
    token = strtok(NULL, " \t\r\n");

    if (token == NULL){ /* not enough parameters */
        printFewParams(1, 6);
        return 0;
    }
    else{
        if (strtok(NULL, " \t\r\n") != NULL){ /* too many parameters */
            printExtraParams(1, 6);
            return 0;
        }
        else{
            check = isAFloat(token);
            if (!check){ /* not a float */
                printNotAFloat(1);
                return 0;
            }
            check = sscanf(token, "%f", &input);
            if (check != 1){ /* sanity check */
                printNotAFloat(1);
                return 0;
            }
            else{
                if (!(input >=0 && input <= 1)){ /* not in the correct range */
                    printWrongRangeFloat(6, input, 1);
                    printf("The parameter should be a non-negative float between 0 and 1.\n");
                    return 0;
                }
                else{
                    return guess(board, input);
                }
            }
        }
    }
}

/*
 * This method assumes the command entered is generate,
 * checks the validity of the rest of the command and executes it.
 * Available only in Edit mode.
 * It returns 0 if we need to continue, and -1 if we need to terminate.
 */
int interpretGenerate(char *token, struct sudokuManager **pBoard, enum Mode mode) {
    int i = 0;
    enum Mode availableModes[1] = {Edit};
    int arrInput[2], arrCheck[2];
    /* arrCheck saves whether we succeeded in converting
     * the string into a number for every parameter */

    if (mode != Edit) {
        printUnavailableMode(7, mode, availableModes, 1);
        return 0;
    }

    while ((token = strtok(NULL, " \t\r\n")) != NULL && i < 2) {
        arrCheck[i] = isAllDigits(token);
        sscanf(token, "%d", &arrInput[i]);
        i++;
    }

    if (i < 2) { /* Not enough parameters*/
        printFewParams(2, 7);
        return 0;
    } else {
        if (token != NULL) { /*Too many parameters*/
            printExtraParams(2, 7);
            return 0;
        } else {
            if (!arrCheck[0]) {
                printNotANumber(1);
                return 0;
            } else {
                if (arrInput[0] < 0 || arrInput[0] > boardArea(*pBoard)) {
                    printWrongRangeInt(7, arrInput[0], 1);
                    printRangeInt(0, boardArea(*pBoard), "non-negative");
                    return 0;
                } else {
                    if ((*pBoard)->emptyCells < arrInput[0]) { /* checking for number of empty cells */
                        printGenerateInputError((*pBoard)->emptyCells, arrInput[0]);
                        return 0;
                    } else {
                        if (!arrCheck[1]) {
                            printNotANumber(2);
                            return 0;
                        } else {
                            if (arrInput[1] <= 0 || arrInput[1] > boardArea(*pBoard)) {
                                printWrongRangeInt(7, arrInput[1], 2);
                                printRangeInt(0, boardArea(*pBoard), "positive");
                                return 0;
                            } else { /* All parameters are valid */
                                return generate(pBoard, arrInput[0], arrInput[1]);
                            }
                        }
                    }
                }
            }
        }
    }
}

/*
 * This method assumes the command entered is undo,
 * checks the validity of the rest of the command and executes it.
 * Available in Solve and Edit modes.
 * It returns 0.
 */
int interpretUndo(char *token, struct sudokuManager *board, enum Mode mode) {
    enum Mode availableModes[2] = {Solve, Edit};

    if (interpretNoArguments(token, mode,8, availableModes, 2) == -1){
        return 0;
    }
    else{
        undo(board);
        return 0;
    }
}

/*
 * This method assumes the command entered is redo,
 * checks the validity of the rest of the command and executes it.
 * Available in Solve and Edit modes.
 * It returns 0.
 */
int interpretRedo(char *token, struct sudokuManager *board, enum Mode mode) {
    enum Mode availableModes[2] = {Solve, Edit};
    if (interpretNoArguments(token, mode, 9, availableModes, 2) == -1){
        return 0;
    }
    else{
        redo(board);
        return 0;
    }
}

/*
 * This method assumes the command entered is save,
 * checks the validity of the rest of the command and executes it.
 * Available in Solve and Edit modes.
 * It returns -1 if we need to terminate, and 0 otherwise.
 */
int interpretSave(char *token, struct sudokuManager *board, enum Mode mode) {
    enum Mode availableModes[2] = {Solve, Edit};
    if (mode == Init) {
        printUnavailableMode(9, mode, availableModes, 2);
        return 0;
    }
    token = strtok(NULL, " \t\r\n");

    if(token == NULL){ /* Not enough parameters */
        printFewParams(1, 10);
        return 0;
    }
    else{
        if (strtok(NULL, " \t\r\n") != NULL){ /* Too many parameters */
            printExtraParams(1, 10);
            return 0;
        }
        else{
            return save(board, token);
        }
    }
}

/*
 * This method assumes the command entered is hint or guess_hint,
 * checks the validity of the rest of the command and executes it.
 * Available in Solve mode only.
 * It receives a boolean parameter saying if it is hint or guess_hint.
 * It returns 0 if we need to continue, and -1 if we need to terminate.
 */
int interpretHintOrGuessHint(char *token, struct sudokuManager *board, int isHint, enum Mode mode){
    int i = 0;
    int arrInput[2], arrCheck[2];
    /* arrCheck saves whether we succeeded in converting
     * the string into a number for every parameter */
    int len;
    enum Mode availableModes[1] = {Solve};
    if (mode != Solve) {
        printUnavailableMode(11, mode, availableModes, 1);
        return 0;
    }
    while ((token = strtok(NULL, " \t\r\n")) != NULL && i < 2){
        arrCheck[i] = isAllDigits(token);
        sscanf(token, "%d", &arrInput[i]);
        i++;
    }
    if (i < 2){ /* Not enough parameters*/
        printFewParams(2, 11);
        return 0;
    }
    else {
        if (token != NULL) { /*Too many parameters*/
            printExtraParams(2, 11);
            return 0;
        } else {
            len = boardLen(board);
            if (!arrCheck[0]) {
                printNotANumber(1);
                return 0;
            } else {
                if (!(arrInput[0] - 1 >= 0 && arrInput[0] - 1 < len)) {
                    printWrongRangeInt(11, arrInput[0], 1);
                    printRangeInt(1, len, "positive");
                    return 0;
                } else {
                    if (!arrCheck[1]) {
                        printNotANumber(2);
                        return 0;
                    } else {
                        if (!(arrInput[1] - 1 >= 0 && arrInput[1] - 1 < len)) {
                            printWrongRangeInt(11, arrInput[1], 2);
                            printRangeInt(1, len, "positive");
                            return 0;
                        } else { /* all parameters are valid */
                            if (isHint){
                                return hint(board, arrInput[0], arrInput[1]);
                            }
                            else{
                                return guessHint(board, arrInput[0], arrInput[1]);
                            }
                        }
                    }
                }
            }
        }
    }
}

/*
 * This method assumes the command entered is num_solutions,
 * checks the validity of the rest of the command and executes it.
 * Available in Solve and Edit modes.
 * It returns 0 if we need to continue, and -1 if we need to terminate.
 */
int interpretNumSolutions(char *token, struct sudokuManager *board, enum Mode mode) {
    enum Mode availableModes[2] = {Solve, Edit};
    if (interpretNoArguments(token, mode, 13, availableModes, 2) == -1){
        return 0;
    }
    else{
        return numSolutions(board);
    }
}

/*
 * This method assumes the command entered is autofill,
 * checks the validity of the rest of the command and executes it.
 * Available in Solve and Edit modes.
 * It returns 0 if we need to continue, and -1 if we need to terminate.
 */
int interpretAutofill(char *token, struct sudokuManager *board, enum Mode mode) {
    enum Mode availableModes[2] = {Solve};
    if (interpretNoArguments(token, mode, 14, availableModes, 1) == -1){
        return 0;
    }
    else{
        return autofill(board);
    }
}

/*
 * This method assumes the command entered is reset,
 * checks the validity of the rest of the command and executes it.
 * Available in Solve and Edit modes.
 * It returns 0.
 */
int interpretReset(char *token, struct sudokuManager *board, enum Mode mode) {
    enum Mode availableModes[2] = {Solve, Edit};
    if (interpretNoArguments(token, mode, 15, availableModes, 2) == -1){
        return 0;
    }
    else{
        reset(board);
        return 0;
    }
}

/*
 * This method assumes the command entered is exit,
 * checks the validity of the rest of the command and executes it.
 * Available in Solve and Edit modes.
 * It returns 0 if the command is invalid. Otherwise, it returns exitGame's return value = 2.
 */
int interpretExit(char *token, struct sudokuManager *board, enum Mode mode) {
    enum Mode availableModes[3] = {Init, Solve, Edit};
    if (interpretNoArguments(token, mode, 16, availableModes, 3) == -1){
        return 0;
    }
    else{
        return exitGame(board);
    }
}

/*
 * This function skips characters until we reach the next line or EOF.
 */
void skipLine(){
    char tav;
    tav = getc(stdin);
    while (tav != EOF && tav != '\n'){
        tav = getc(stdin);
    }
}

/*
 * This method interprets the command the user entered and
 * executes it by calling to the proper method in Game.
 * It returns -1 if we need to terminate.
 * If exit command has been received, it returns 2.
 * Otherwise, it returns 0.
 */
int interpret(char *command, struct sudokuManager **pBoard, enum Mode mode){
    char *token;
    struct sudokuManager *board = *pBoard;
    int index;
    int len = strlen(command);
    if (len == LENGTH - 1){
        if (command[len - 1] != '\n'){
            printf("Error: too many characters in a single line.\n"
                   "A line should contain up to 256 characters.\nPlease enter a new command.\n");
            skipLine();
            return 0;
        }
    }

    token = strtok(command, " \t\r\n");
    if (token != NULL) { /* First token exists*/

        index = commandNum(token);

        switch (index){
            case 0:
                return interpretSolve(token, pBoard);
            case 1:
                return interpretEdit(token, pBoard);
            case 2:
                return interpretMarkErrors(token, mode);
            case 3:
                return interpretPrintBoard(token, board, mode);
            case 4:
                return interpretSet(token, board, mode);
            case 5:
                return interpretValidate(token, board, mode);
            case 6:
                return interpretGuess(token, board, mode);
            case 7:
                return interpretGenerate(token, pBoard, mode);
            case 8:
                return interpretUndo(token, board, mode);
            case 9:
                return interpretRedo(token, board, mode);
            case 10:
                return interpretSave(token, board, mode);
            case 11:
                return interpretHintOrGuessHint(token, board, 1, mode);
            case 12:
                return interpretHintOrGuessHint(token, board, 0, mode);
            case 13:
                return interpretNumSolutions(token, board, mode);
            case 14:
                return interpretAutofill(token, board, mode);
            case 15:
                return interpretReset(token, board, mode);
            case 16:
                return interpretExit(token, board, mode);
            default:
                printInvalidCommand();
                return 0;
        }
    }
    return 0;
}


