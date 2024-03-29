/*
 * This module deals with everything that has to do with the sudokuManager struct.
 * It have methods regarding this struct which can be used by any module which includes utilitiesBoardManager.c
 * and uses sudokuManager struct.
 */

#include "board_manager.h"
#include <stdlib.h>

/* GENERAL METHODS */

/*
 * This function calculates the row's lower bound of the block.
 */
int rowLowBound(int m, int row){
    return ((row/m)*m);
}

/*
 * This function calculates the row's higher bound of the block.
 */
int rowHighBound(int m, int row){
    return (((row/m)+1)*m);
}

/*
 * This function calculates the column's lower bound of the block.
 */
int colLowBound(int n, int column){
    return ((column/n)*n);
}

/*
 * This function calculates the column's higher bound of the block.
 */
int colHighBound(int n, int column){
    return (((column/n)+1)*n);
}

/*
 * This method assigns the first cell indices of the block numbered blockNum into (pRow, pCol).
 * 0 <= blockNum < n*m
 */
void getFirstIndexInBlock(int m, int n, int blockNum, int *pRow, int *pCol){
    *pRow = m * (blockNum / m);
    *pCol = n * (blockNum % m);
}

/*
 * This method returns the length of the sudoku board.
 */
int boardLen(struct sudokuManager *manager){
    return (manager->m)*(manager->n);
}

/*
 * returns the amount of cells in the board.
 */
int boardArea(struct sudokuManager *manager){
    return (boardLen(manager)*boardLen(manager));
}

/*
 * This method returns the matrix index in the array of the board.
 */
int matIndex(int m, int n, int row, int col){
    return row*(n*m)+col;
}

/*
 * This method returns if a cell is a fixed cell,
 * if it is, returns 1,
 * otherwise, returns 0.
 */
int isFixedCell(struct sudokuManager *manager, int row, int col){
    return manager->fixed[matIndex(manager->m, manager->n, row, col)];
}

/*
 * Returns the amount of times that the row contains val.
 * If val == 0, then it returns 0.
 */
int rowContains(int *board, int m, int n, int row, int val){
    int length = n*m, col, count = 0;
    if (val == 0){
        return 0;
    }
    for (col = 0; col < length ; col++) {
        if(board[matIndex(m, n, row, col)] == val){
            count++;
        }
    }
    return count;
}

/*
 * Returns the amount of times that the column contains val.
 * If val == 0, then it returns 0.
 */
int colContains(int *board, int m, int n, int col, int val){
    int length = n*m, row, count = 0;
    if (val == 0){
        return 0;
    }
    for (row = 0; row < length ; row++) {
        if(board[matIndex(m, n, row, col)] == val){
            count++;
        }
    }
    return count;
}

/*
 * Returns the amount of times that the block of <i,j> contains val.
 * If val == 0, then it returns 0.
 */
int blockContains(int* board, int m, int n, int row, int col, int val){
    int blockRowLowBound = rowLowBound(m, row);
    int blockRowHighBound = rowHighBound(m, row);
    int blockColLowBound = colLowBound(n, col);
    int blockColHighBound = colHighBound(n, col);
    int i, j, count = 0;

    if (val == 0){
        return 0;
    }
    for(i = blockRowLowBound ; i < blockRowHighBound ; i++){
        for(j = blockColLowBound ; j < blockColHighBound ; j++) {
            if(board[matIndex(m, n, i, j)] == val){
                count++;
            }
        }
    }
    return count;
}

/*
 * This method returns 1 if a row/col/block contains val more than once.
 */
int neighbourContainsTwice(int* board, int m, int n, int row, int col, int val){
    if(rowContains(board, m, n, row, val) > 1){
        return 1;
    }
    if(colContains(board, m, n, col, val) > 1){
        return 1;
    }
    if(blockContains(board, m, n, row, col, val) > 1){
        return 1;
    }
    return 0;
}

/*
 * This method returns 1 if a row/col/block contains val at least once.
 */
int neighbourContainsOnce(int* board, int m, int n, int row, int col, int val){
    if(rowContains(board, m, n, row, val) > 0){
        return 1;
    }
    if(colContains(board, m, n, col, val) > 0){
        return 1;
    }
    if(blockContains(board, m, n, row, col, val) > 0){
        return 1;
    }
    return 0;
}

/*
 * This function updates the errouneous board for all cells in the block,
 * for each cell <row, col> which is erroneous in block,
 * it updates cell <row, col> to be 1 in erroneous.
 * if any cell was erroneous in that block, will return 1, otherwise it returns 0.
 */
int updateErroneousBlock(int* board, int* erroneous, int m, int n, int i, int j){
    int blockRowLowBound = rowLowBound(m, i);
    int blockRowHighBound = rowHighBound(m, i);
    int blockColLowBound = colLowBound(n, j);
    int blockColHighBound = colHighBound(n, j);
    int row, col, ret = 0;
    for(row = blockRowLowBound; row < blockRowHighBound; row++){
        for(col = blockColLowBound ; col < blockColHighBound ; col++) {
            if(neighbourContainsTwice(board, m, n, row, col, board[matIndex(m, n, row, col)])){
                erroneous[matIndex(m, n, row, col)] = 1;
                ret = 1;
            }
            else {
                erroneous[matIndex(m, n, row, col)] = 0;
            }
        }
    }
    return ret;
}

/*
 * This function updates the errouneous board for all cells in the column,
 * for each cell <row, col> which is erroneous in board,
 * it updates cell <row, col> to be 1 in erroneous.
 * if any cell was erroneous in that column, will return 1, otherwise it returns 0.
 */
int updateErroneousCol(int* board, int* erroneous, int m, int n, int col){
    int row, length = n*m, ret = 0;
    for (row = 0; row < length; row++) {
        if(neighbourContainsTwice(board, m, n, row, col, board[matIndex(m, n, row, col)])){
            erroneous[matIndex(m, n, row, col)] = 1;
            ret = 1;
        }
        else {
            erroneous[matIndex(m, n, row, col)] = 0;
        }
    }
    return ret;
}

/*
 * This function updates the errouneous board for all cells in the row,
 * for each cell <row, col> which is erroneous in board,
 * it updates cell <row, col> to be 1 in erroneous.
 * if any cell was erroneous in that row, will return 1, otherwise it returns 0.
 */
int updateErroneousRow(int* board, int* erroneous, int m, int n, int row){
    int col, length = n*m, ret = 0;
    for (col = 0; col < length; col++) {
        if(neighbourContainsTwice(board, m, n, row, col, board[matIndex(m, n, row, col)])){
            erroneous[matIndex(m, n, row, col)] = 1;
            ret = 1;
        }
        else {
            erroneous[matIndex(m, n, row, col)] = 0;
        }
    }
    return ret;
}

/*
 * This updates the erroneous board for a sudokuBoard.
 * input is a pointer to the board typed *int.
 * Will be used when Loading a file in "edit" or "solve" functions.
 * the function returns 1 if one of the cells is erroneous, and 0 otherwise.
 */
int updateErroneousBoard(int* board, int* erroneous, int m, int n){
    int row, col, length = n*m, ret = 0;
    for(row = 0; row < length; row++){
        for (col = 0; col < length; col++) {
            if(neighbourContainsTwice(board, m, n, row, col, board[matIndex(m, n, row, col)])){
                erroneous[matIndex(m, n, row, col)] = 1;
                ret = 1;
            }
            else {
                erroneous[matIndex(m, n, row, col)] = 0;
            }
        }
    }
    return ret;
}

/*
 * This function updates the erroneous board of a sudokuManager.
 * it will update all row, column, and the block that <row, col> is in.
 * the function returns 1 if one of the cells is illegal and 0 otherwise.
 */
int updateErroneousBoardCell(int* board, int* erroneous, int m, int n, int row, int col){
    int ret = 0;
    if (updateErroneousBlock(board, erroneous, m, n, row, col)){
        ret = 1;
    }
    if(updateErroneousCol(board, erroneous, m, n, col)){
        ret = 1;
    }
    if(updateErroneousRow(board, erroneous, m, n, row)){
        ret = 1;
    }
    return ret;
}

/*
 * This method returns if the value of the cell is legal,
 * if it is, returns 1, otherwise, returns 0.
 */
int isErroneous(struct sudokuManager *manager, int i, int j){
    return (manager->erroneous[matIndex(manager->m, manager->n, i, j)]);
}

/*
 * This function checks if the board is erroneous.
 * If one of the cells is erroneous it returns 1, Otherwise, it returns 0.
 */
int isAnyErroneousCell(struct sudokuManager *manager){
    int i, size = boardArea(manager);
    for(i = 0; i < size; i++){
        if(manager->erroneous[i] == 1){
            return 1;
        }
    }
    return 0;
}

/*
 * This method checks that the value x is in the correct range for the current board.
 */
int isLegalCellValue(struct sudokuManager *manager, int x){
    return ((x >= 0) && (x <= boardLen(manager)));
}

/*
 * This method counts the empty cells in the board.
 */
int amountOfEmptyCells(struct sudokuManager *manager){
    int i, count = 0;
    for(i=0; i<boardArea(manager); i++){
        if(manager->board[i] == 0)
            count ++;
    }
    return count;
}

/*
 * This method frees a given board.
 */
void freeBoard(struct sudokuManager *board){
    if (board->linkedList != NULL){
        pointToFirstMoveInMovesList(board, 0); /* points the move's list to first move */
        killNextMoves(board); /* frees all next moves */
    }
    free(board->fixed);
    free(board->erroneous);
    free(board->board);
    free(board->linkedList);
    free(board);
}

/*
 * This function changes the value in cell <row, col> to val,
 * assumes all input is legal.
 */
void changeCellValue(int *board, int m, int n, int row, int col, int val){
    board[matIndex(m, n, row, col)] = val;
}

/*
 * This method creates a new matrix of integers containing only the fixed values in board.
 * If there is an error allocating memory, it returns NULL and prints a message to the user.
 */
int *copyFixedOnly(struct sudokuManager *board, int *onlyFixed){
    int row, col, index;
    for (row = 0; row < boardLen(board); row++) {
        for (col = 0; col < boardLen(board); col++){
            if (isFixedCell(board, row, col)){
                index = matIndex(board->m, board->n, row, col);
                changeCellValue(onlyFixed, board->m, board->n, row, col, board->board[index]);
            }
        }
    }
    return onlyFixed;
}

/*
 * This method copies array from to array to.
 */
void duplicateBoard(int* from, int *to, int m, int n){
    int row, col, index;
    int size = n*m;
    for (row = 0; row < size ; row++) {
        for (col = 0; col < size ; col++) {
            index = matIndex(m, n, row, col);
            to[index] = from[index];
        }
    }
}

/*
 * This method checks if the current cell in last is row, assuming length of row is size.
 */
int isLastInRow(int size, int j){
    return (j == size - 1);
}

/*
 * This method checks if the current cell is last in column, assuming length of column is size.
 */
int isLastInCol(int size, int i){
    return (i == size - 1);
}

/*
* This method checks if the current cell is last in the matrix, assuming the matrix is size X size.
*/
int isLastCellInMatrix(int size, int i, int j){
    return (isLastInCol(size, i) && isLastInRow(size, j));
}

/*
 * This function receives a board, and index.
 * if cell <row, col> has only one legal value to fill, it returns it.
 * if there is no legal value, it returns -1.
 * if there is more than one legal value, it returns 0.
 */
int returnLegalValue(int* board, int m, int n, int row, int col){
    int i, N = m * n, value = 0;
    if(board[matIndex(m, n, row, col)] != 0){
        return 0; /* cell is not empty - return */
    }
    for(i = 1; i <= N ; i++){
        if (!neighbourContainsOnce(board, m, n, row, col, i)){ /* none if neighbours contain value i */
            if(value == 0) {
                value = i; /* this is the value we would like to fill in cell <row, col>,
                            * it there is only one legal option */
            }
            else{ /* we already have updated value, we know there is more than 1 legal value */
                return 0;
            }
        }
    }
    if (value == 0){ /* there is no legal value */
        return -1;
    }
    return value; /* this is the value we want to set in the board */
}

/*
 * This function fills board tmp with the values that has been returned from returnLegalValue function.
 */
void fillSingleLegalValue(struct sudokuManager *board, int *tmp) {
    int row, col, m = board->m, n = board->n, length = boardLen(board);
    int val, index;
    for(row = 0; row < length ; row++){
        for(col = 0; col < length ; col++){
            val = returnLegalValue(board->board, m, n, row, col);
            index = matIndex(m, n, row, col);
            if (val == -1){
                tmp[index] = 0;
            }
            else{
                tmp[index] = val;
            }
        }
    }
}

/*
 * This function receives a board and tmp int pointer that is filled with legal values
 * (if a cell contain value 0 than it is not a cell with a single value to fill)
 * return -1 in case of allocation failure.
 * otherwise, returns 0.
 */
int fillBoardWithSignleLegalValues(struct sudokuManager *board, int *tmp) {
    int row, col, length = boardLen(board), index = 0, val;
    for (row = 0; row < length; row++) {
        for (col = 0; col < length; col++) {
            index = matIndex(board->m, board->n, row, col);
            val = tmp[index];
            if (val != 0) { /* there is only one legal value for <row, col> */
                if (doSet(board, row, col, val) == -1) {
                    free(tmp);
                    return -1;
                }
            }
        }
    }
    return 0;
}

/*
 * This method updates the board with autofilled values
 */
int updateAutofillValuesBoard(struct sudokuManager *board){
    int* tmp;
    /* destroy all next moves in the boards' move list */
    if (board->linkedList->next != NULL){
        killNextMoves(board);
    }
    tmp = calloc(boardArea(board), sizeof(int));
    if(tmp == NULL){
        return -1;
    }
    fillSingleLegalValue(board, tmp);
    if(fillBoardWithSignleLegalValues(board, tmp)== -1){
        free(tmp);
        return -1;
    }
    free(tmp);
    if(createNextNode(board, separator, 0, 0, 0, 0) == -1){
        return -1;
    }
    goToNextNode(board);
    return 0;
}

/*
 * This function updates emptyCells field when necessary.
 */
void updateEmptyCellsSingleSet(struct sudokuManager *manager, int prevVal, int nextVal){
    if(prevVal == 0){
        if(nextVal != 0){ /* change an empty cell to be not empty */
            manager->emptyCells --;
        }
    } /* prevVal != 0 */
    else {
        if(nextVal == 0){ /* change a filled cell to be empty */
            manager->emptyCells ++;
        }
    }
}

/*
 * this method sets Z to <row, col>,
 * is called after verifying that all values are legal.
 */
int doSet(struct sudokuManager *manager, int row, int col, int Z){
    int prevVal = manager->board[matIndex(manager->m, manager->n, row, col)];
    updateEmptyCellsSingleSet(manager, prevVal, Z); /* update the amount of emptyCells field */
    changeCellValue(manager->board, manager->m, manager->n, row, col, Z);
    if (manager->linkedList->next != NULL){
        killNextMoves(manager);
    }
    if (createNextNode(manager, command, row, col, Z, prevVal) == -1){
        return -1;
    }
    goToNextNode(manager);
    return 0;
}


/*
 This method counts the amount of empty cells in boards.
 */
void updateEmptyCellsField(struct sudokuManager *manager){
    manager->emptyCells = amountOfEmptyCells(manager);
}

/*
 * This method sets all pointers in board to NULL and all integers to -1.
 */
void initNullBoard(struct sudokuManager *manager){
    manager->board = NULL;
    manager->linkedList = NULL;
    manager->fixed = NULL;
    manager->erroneous = NULL;
    manager->m = -1;
    manager->n = -1;
    manager->emptyCells = -1;
}

/*
 * This method updated the board values by given parameters.
 */
void initBoardValues(struct sudokuManager *boardToFill, int m, int n, int *board, int *erroneous, int *fixed,
        int emptyCells, struct movesList *list){
    boardToFill->m = m, boardToFill->n = n;
    boardToFill->board = board;
    boardToFill->erroneous = erroneous;
    boardToFill->fixed= fixed;
    boardToFill->emptyCells = emptyCells;
    boardToFill->linkedList = list;
    initList(boardToFill->linkedList);
    boardToFill->linkedList->board = boardToFill;
}

