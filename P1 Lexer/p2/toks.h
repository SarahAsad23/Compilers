// toks.h - collection of Tokens - Jim Hogg, 2020

#pragma once

#include "tok.h"            // Tok
#include "ut.h"             // ut*

typedef struct _Toks {
  #define MAXTOKNUM 999
  int tokNum;               // current Tok number (iterator)
  int hiTokNum;             // hightest Tok number in current Toks object
  Tok tok[MAXTOKNUM + 1];
} Toks;


void  toksAdd(Toks* toks, Tok* tok);
int   toksAtEnd(Toks* toks);
Tok*  toksCurr(Toks* toks);
void  toksDump(Toks* toks);
Toks* toksNew();
Tok*  toksNext(Toks* toks);
Tok*  toksPrev(Toks* toks);
void  toksRewind(Toks* toks);