// main.h - SubC Compiler 

#pragma once

#include <stdio.h>      // printf, FILE
#include <stdlib.h>     // exit

#include "lex.h"        // Lex
#include "ut.h"         // utReadFile

int main(int argc, char* argv[]);
void usage();