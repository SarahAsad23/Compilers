// emit.h - Emit ARM Assembly Code - Jim Hogg, 2022

#pragma once

#include <stdio.h>      // sprintf
#include "ut.h"         // ut*

#define LINESIZE 100

typedef struct {
  #define CODESIZE 50000
  char* codeBuf;
  int   codeSize;

  #define DATASIZE 50000
  char* dataBuf;
  int   dataSize;
} Emit;

void  emitCode(Emit* emit, char* line);
void  emitCodeDirective(Emit* emit);
void  emitData(Emit* emit, char* line);
void  emitDataDirective(Emit* emit);
void  emitDump(Emit* emit);
Emit* emitNew();
char* emitNewName(char* sourcePath);
void  emitSave(Emit* emit, char* filePath);
