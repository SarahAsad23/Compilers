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


// Read the entire file specified by 'filePath'.  Return its contents.
char* utReadFile(char* filePath) {
  FILE* f; 
  f = fopen(filePath, "r"); 
  
  if(f == NULL){
    //unable to open file - exit
    printf("Unable to open file"); 
    return NULL; 
  }

  //move to the end of the file using fseek()
  fseek(f, 0, SEEK_END); 

  //use ftell to get the current position and store it in fileSize
  int fileSize = ftell(f); 

  if(fileSize == -1){
    printf("error getting file size"); 
    //close the file and exit 
    fclose(f); 
    return NULL; 
  }

  //Allocate a buffer, zero-filled, to hold the file contents.
  char* prog = (char*) calloc(fileSize + 1, 1);                        

  //Read entire file into the 'prog' buffer
  //move back to the beginning of the file 
  fseek(f, 0, SEEK_SET); 

  //copy contents from file to prog
  fread(prog, 1, fileSize, f); 

  //null terminate it 
  prog[fileSize] = '\0'; 

  //close the file 
  fclose(f); 

  return prog;
}

char* utStrndup(char* s, int len) {
  char* copy = malloc(len + 1);
  strncpy(copy, s, len);
  copy[len] = '\0';
  return copy;
}
