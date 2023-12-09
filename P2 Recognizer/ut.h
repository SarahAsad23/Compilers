// ut.h - Utility Functions for the SubC Compiler - Jim Hogg, 2020

#pragma once

#include <stdio.h>    // printf
#include <stdlib.h>   // exit
#include <string.h>   // strlen

#include "tok.h"      // Tok

void  utDie2Str(char* func, char* msg);
void  utDie3Str(char* func, char* msg1, char* msg2);
void  utDie4Str(char* func, char* msg1, char* msg2, char*mst3);
void  utDie2StrCharLC(char* func, char* msg, char c, int linNum, int colNum);
void  utDieStrTokStr(char* func, Tok* tok, char* msg);
void  utPause();
char* utReadFile(char* filePath);
char* utStrndup(char* s, int len);