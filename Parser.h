
#include "solver.h"

#ifndef SOFTWAREPROJECTFINALPROJECT_PARSER_H
#define SOFTWAREPROJECTFINALPROJECT_PARSER_H

/*
 * The function prints a message to the user that the command is invalid.
 */
void printInvalidCommand();

/*
 * The function reads a command line and interprets it.
 * The function then executes the proper command by calling to the command in Game.
 */
int interpret(char *command, struct Board **board);

#endif
