// tok.h - Tokens - Jim Hogg, 2020

#pragma once

#include <stdio.h>      // printf
#include <stdlib.h>     // malloc
#include <string.h>     // strlen

typedef enum {
  TOKADD = 1, TOKBAD, TOKCOMMA, TOKEEQ, TOKEOF, TOKEQ, TOKGE, TOKGT, TOKIF,
  TOKINT, TOKLBRACE, TOKLE, TOKLPAREN, TOKLT, TOKMUL, TOKNAM, TOKNE, TOKNUM, 
  TOKRBRACE, TOKRET, TOKRPAREN, TOKSEMI, TOKSTR, TOKSUB, TOKWHILE
} TokKind;

char* tokStr(TokKind kind);

typedef struct _Tok {
  TokKind kind;       // eg: TOKNUM
  char*   lexeme;     // eg: "123" for TOKNUM; "hello" for TOKSTR
  long    num;        // eg: 123 for TOKNUM
  int     linNum;     // eg: 14
  int     colNum;     // eg: 8
} Tok;

Tok*  tokNew(int kind, char* lexeme, long num, int linNum, int colNum);
char* tokStr(TokKind kind);
