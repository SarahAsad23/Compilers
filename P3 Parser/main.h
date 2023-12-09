// main.h - SubC Compiler - Jim Hogg, 2020

#pragma once

#include <stdio.h>      // printf, FILE
#include <stdlib.h>     // exit

#include "ast.h"        // AstProg
#include "lex.h"        // Lex
#include "pse.h"        // parProg
#include "ut.h"         // utReadFile
#include "visit.h"      // visit* functions

int main(int argc, char* argv[]);
void usage();