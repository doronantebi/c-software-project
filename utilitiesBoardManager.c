#include "utilitiesBoardManager.h"
#include <stdlib.h>
#include <stdio.h>

/* GENERAL METHODS */


/*
 * This function calculates the row's lower bound of the block.
 * CHECK!!!!!!!!
 */
int rowLowBound(int m, int row){
    return ((row/m)*m);
}

/*
 * This function calculates the row's higher bound of the block.
 * CHECK!!!!!!!!
 */
int rowHighBound(int m, int row){
    return (((row/m)+1)*m);
}

/*
 * This function calculates the column's lower bound of the block.
 * CHECK!!!!!!!!
 */
int colLowBound(int n, int column){
    return ((column/n)*n);
}

/*
 * This function calculates the column's higher bound of the block.
 * CHECK!!!!!!!!
 */
int colHighBound(int n, int column){
    return (((column/n)+1)*n);
}


/*
 * This method returns the length of the sudoku board
 */
int boardLen(struct sudokuManager *manager){
    return (manager->m)*(manager->n);
}

/*
 * returns the amount of cells in the board
 */
int boardArea(struct sudokuManager *manager){
    return (boardLen(manager)*boardLen(manager));
}

/*
 * This method returns the matrix index in the array of the board
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
 * CHECK!!!!!!!!
 */
int rowContains(int *board, int m, int n, int row, int val){
    int length = n*m, col, count = 0;
    for (col = 0; col < length ; col++) {
        if(board[matIndex(m, n, row, col)] == val){
            count++;
        }
    }
    return count;
}

/*
 * Returns the amount of times that the column contains val
 */
int colContains(int *board, int m, int n, int col, int val){
    int length = n*m, row, count = 0;
    for (row = 0; row < length ; row++) {
        if(board[matIndex(m, n, row, col)] == val){
            count++;
        }
    }
    return count;
}

/*
 * Returns the amount of times that the block of (i,j)
 * contains val.
 */
int blockContains(int* board, int m, int n, int row, int col, int val){
    int blockRowLowBound = rowLowBound(m, row);
    int blockRowHighBound = rowHighBound(m, row);
    int blockColLowBound = colLowBound(n, col);
    int blockColHighBound = colHighBound(n, col);
    int i, j, count = 0;
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
 * This method returns 1 if a row/col/block contains
 * val more than once.
 */
int neighbourContainsTwice(int* board, int m, int n, int row, int col, int val){
    if(rowContains(board, m, n, row, val)>1){
        return 1;
    }
    if(colContains(board, m, n, col, val)>1){
        return 1;
    }
    if(blockContains(board, m, n, row, col, val)>1){
        return 1;
    }
    return 0;
}

/*
 * This method returns 1 if a row/col/block contains
 * val once.
 */
int neighbourContainsOnce(int* board, int m, int n, int i, int j, int val){
    if(rowContains(board, m, n, i, val)>0){
        return 1;
    }
    if(colContains(board, m, n, j, val)>0){
        return 1;
    }
    if(blockContains(board, m, n, i, j, val)>0){
        return 1;
    }
    return 0;
}

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

int updateErroneousRow(int* board, int* erroneous, int m, int n, int j){
    int row, length = n*m, ret = 0;
    for (row = 0; row < length; row++) {
        if(neighbourContainsTwice(board, m, n, row, j, board[matIndex(m, n, row, j)])){
            erroneous[matIndex(m, n, row, j)] = 1;
            ret = 1;
        }
        else {
            erroneous[matIndex(m, n, row, j)] = 0;
        }
    }
    return ret;
}


int updateErroneousCol(int* board, int* erroneous, int m, int n, int i){
    int col, length = n*m, ret = 0;
    for (col = 0; col < length; col++) {
        if(neighbourContainsTwice(board, m, n, i, col, board[matIndex(m, n, i, col)])){
            erroneous[matIndex(m, n, i, col)] = 1;
            ret = 1;
        }
        else {
            erroneous[matIndex(m, n, i, col)] = 0;
        }
    }
    return ret;
}

/*
 * This updates the erroneous board for a sudokuBoard
 * input is a pointer to the board typed *int.
 * Will be used when Loading a file in Edit mode
 * or when loading a file in Solve mode when setting addMarks to be 1
 * UPDATES THE WHOLE BOARD
 * the function returns 1 if one of the cells is illegal
 * and 0 otherwise.
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
 * This function updates the erronous board of a sudokuManager.
 * it will update all row i, column j, and the block that (i,j) is in.
 * the function returns 1 if one of the cells is illegal
 * and 0 otherwise.
 */
int updateErroneousBoardCell(int* board, int* erroneous, int m, int n, int i, int j){
    int ret = 0;
    if (updateErroneousBlock(board, erroneous, m, n, i,j)){
        ret = 1;
    }
    if(updateErroneousCol(board, erroneous, m, n, i)){
        ret = 1;
    }
    if(updateErroneousRow(board, erroneous, m, n, j)){
        ret = 1;
    }
    return ret;
}

/*
 * This method returns if the value of the cell is legal,
 * if it is, returns 1,
 * otherwise, returns 0.
 */
int isErroneous(struct sudokuManager *manager, int i, int j){
    return manager->erroneous[matIndex(manager->m, manager->n, i, j)];
}

/*
 * if one of the cells in the errouneous board is 1, returns 1.
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
 * Checks that the val
 */
int isLegalCellValue(struct sudokuManager *manager, int x){
    return ((x >= 0) && (x <= boardLen(manager)));
}

/*
 * counts the empty cells in the board
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
    pointToFirstMoveInMovesList(board); /* points the move's list to first move */
    killNextMoves(board); /* frees all next moves */
    free(board->fixed);
    free(board->erroneous);
    free(board->board);
    free(board->linkedList);
    free(board);
}


/*
 * This function changes the value in cell <X,Y> to Z
 * assumes all input is legal
 */
void changeCellValue(int *board, int m, int n, int row, int col, int val){
    board[matIndex(m, n,row,col)] = val;
}

/*
 * This method creates a new matrix of integers containing only the fixed values in board.
 * If there is an error allocating memory, it returns NULL and prints a message to the user.
 */
int *copyFixedOnly(struct sudokuManager *board, int *onlyFixed){
    int i, j, index;

    for (i = 0; i < boardLen(board); i++) { /*Row*/
        for (j = 0; j < boardLen(board); j++){ /*Column*/
            if (isFixedCell(board, i, j)){
                index = matIndex(board->m, board->n, i, j);
                changeCellValue(onlyFixed, board->m, board->n, i, j, board->board[index]);
            }
        }
    }

    return onlyFixed;
}

/*
 * This method copies currentGrid to retGrid.
 */
void duplicateBoard(int* currentGrid, int *retGrid, int m, int n){
    int i,j;
    int size = n*m;
    for (i = 0; i < size ; i++) { /* for each row */
        for (j = 0; j < size ; j++) { /* for each column */
            retGrid[matIndex(m, n, i, j)] = currentGrid[matIndex(m, n, i, j)];
        }
    }
}

/*
 * This method checks if the current cell in last is row, assuming length of row is size.
 */
int isLastInRow(int size, int j){
    return j == size - 1;
}

/*
 * This method checks if the current cell is last in column, assuming length of column is size.
 */
int isLastInCol(int size, int i){
    return i == size - 1;
}


/*
* This method checks if the current cell is last in the matrix, assuming the matrix is size X size.
*/
int isLastCellInMatrix(int size, int i, int j){
    return isLastInCol(size, i) && isLastInRow(size, j);
}

/*
 * recieves a board, and index.
 * if the board has only one legal value to fill, it returns it.
 * otherwise, returns 0.
 */
int returnLegalValue(int* board, int m, int n, int row, int col){
    int i, N= m*n,  value = 0;
    if(board[matIndex(m,n,row,col)] != 0){
        return 0; /* cell is not empty - return */
    }
    for(i = 1; i <= N ; i++){
        if (!neighbourContainsOnce(board, m, n, row, col, i)){ /**/
            if(value == 0) {
                value = i;
            } /* we already have updated value, we know there is more than 1 legal value*/
            else{
                return 0;
            }
        }
    }
    return value; /* this is the value we want to set in the board */
}


/*
 * This function updates the board with autofilled values
 */
int updateAutofillValuesBoard(struct sudokuManager *board){
    int* tmp;
    int row, col, val, index;
    int length = boardLen(board), m = board->m, n = board->n;
    if (board->linkedList->next != NULL){
        killNextMoves(board); /*???????????*/
    }
    tmp = calloc(boardArea(board), sizeof(int));
    if(tmp == NULL){
        return -1;
    }
    for(row = 0; row < length ; row++){
        for(col = 0; col < length ; col++){
            tmp[matIndex(m, n, row, col)] = returnLegalValue(board->board,m,n, row, col);
        }
    }
    for(row = 0; row < length ; row++){
        for(col = 0; col < length ; col++){
            index = matIndex(m, n, row, col);
            val = tmp[index];
            if(val != 0){
                board->board[index] = val;
                if(createNextNode(board, command, row, col, val, 0) == -1){
                    free(tmp);
                    return -1;
                }
                goToNextNode(board);
            }
        }
    }
    free(tmp);
    if(board->linkedList->action == separator){ /* no changes made */
        return  0; /*NOT SURE YET IF IT IS A NEW ACTION OR NOT */
    }
    createNextNode(board, separator, 0, 0, 0, 0);
    goToNextNode(board);
    return 0;
}

