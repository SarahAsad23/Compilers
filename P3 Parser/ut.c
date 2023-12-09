// ut.c - Utility functions for the SubC Compiler - Jim Hogg, 2020

#include "ut.h"

void utDie2Str(char* func, char* msg) {
  printf("\n\nERROR: %s: %s \n\n", func, msg);
  utPause();
}

void utDie3Str(char* func, char* msg1, char*msg2) {
  printf("\n\nERROR: %s: %s %s \n\n", func, msg1, msg2);
  utPause();
}

void utDie4Str(char* func, char* msg1, char* msg2, char* msg3) {
  printf("\n\nERROR: %s: %s %s %s \n\n", func, msg1, msg2, msg3);
  utPause();
}

void utDie2StrCharLC(char* func, char* msg, char c, int linNum, int colNum) {
  printf("\n\nERROR: %s %s %c at (%d, %d) \n\n",
    func, msg, c, linNum, colNum);
  utPause();
}

void utDieStrTokStr(char* func, Tok* tok, char* msg) {
  printf("\n\nERROR: %s: Found %s but expecting %s at (%d, %d) \n\n",
    func, tokStr(tok->kind), msg, tok->linNum, tok->colNum);
  utPause();
}

void utPause() {
  printf("Hit any key to finish");
  getchar();
  exit(0);
}

char* utReadFile(char* filePath) {
  FILE* file = fopen(filePath, "r");

  if (!file) utDie2Str("readFile: Cannot open input source file: ", filePath);

  // Find the size of the input file.  Note: on Windows, each line is terminated
  // by 2 chars - CR, LF.  'fileSize', calculated below, includes these chars.
  // However, the 'fread' call below silently replaces each (CR, LF) pair with a
  // single '\n' char.  So how do we find where 'prog' really ends?  We allocate
  // it to be all-zeroes, ahead of populating it using 'fread'.

  fseek(file, 0L, SEEK_END);
  int fileSize = ftell(file);
  fseek(file, 0L, SEEK_SET);

  // Allocate a buffer, zero-filled, to hold the file contents.

  char* prog = (char*) calloc(1 + fileSize, 1);

  // Read the entire file

  fread(prog, 1, fileSize, file);

  return prog;

}

char* utStrndup(char* s, int len) {
  char* copy = malloc(len + 1);
  strncpy(copy, s, len);
  copy[len] = '\0';
  return copy;
}
