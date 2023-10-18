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

  // Read the entire file specified by 'filePath'.  Return its contents.

  //++ Use fopen.  If operation fails, tell user and stop.
  //++ Find how big the file is.  Call it fileSize.  Use fseek, ftell, fseek.
  //++ Allocate a buffer, zero-filled, to hold the file contents.

  char* prog = (char*) calloc(1000, 1);                         //--

  //++ Read entire file into the 'prog' buffer

  strcpy(prog, "int main() { int x; x = 42; sayn(x); }");       //--

  return prog;

}

char* utStrndup(char* s, int len) {
  char* copy = malloc(len + 1);
  strncpy(copy, s, len);
  copy[len] = '\0';
  return copy;
}
