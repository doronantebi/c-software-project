#include <stdlib.h>
#include <stdio.h>
#include "utilitiesBoardManager.h"
#include "main_aux.h"

#include "game.h"

#ifndef SOFTWAREPROJECTFINALPROJECT_PARSER_H
#define SOFTWAREPROJECTFINALPROJECT_PARSER_H


/*
 * The function reads a command line and interprets it.
 * The function then executes the proper command by calling to the command in Game.
 */
int interpret(char *command, struct sudokuManager **board, enum Mode mode);

#endif
