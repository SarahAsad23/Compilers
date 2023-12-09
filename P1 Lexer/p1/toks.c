// toks.c - container of Tokens 

#include <stddef.h>       // offsetof
#include <stdlib.h>       // malloc
#include "toks.h"

// ============================================================================
// Append 'tok' to the 'toks' array, checking for overflow
// ============================================================================
void toksAdd(Toks* toks, Tok* tok) {
  if (toks->tokNum < MAXTOKNUM) {
    ++toks->tokNum;
    ++toks->hiTokNum;
    toks->tok[toks->tokNum] = *tok;
  } else {
    utDie2Str("toksAdd", "Too many tokens!");
  }
}

// ============================================================================
// Check whether we are "at the end" of the Toks array.  That's to say, we
// have already processed all the Toks
// ============================================================================
int toksAtEnd(Toks* toks) {
  return toks->tokNum > toks->hiTokNum;
}

// ============================================================================
// Return the current Tok (ie, the one at the toks->tokNum 'cursor')
// ============================================================================
Tok* toksCurr(Toks* toks) {
  if (toksAtEnd(toks)) {
    utDie2Str("toksCur", "Attempt to access a Tok beyond end of Toks array");
  } else {
    return &toks->tok[toks->tokNum];
  }
  return NULL;            // unreachable; pacify compiler
}

// ============================================================================
// Dump all of the tokens in 'toks' to the screen.  Only used for debugging
// ============================================================================
void toksDump(Toks* toks) {
  FILE* f = fopen("ToksDump.txt", "w");
  for (int t = 0; t <= toks->hiTokNum; ++t) {
    Tok* tok = &toks->tok[t];
    fprintf(f,"[%3d] %3d  %10s %10s  %ld (%d, %d) \n",
      t, tok->kind, tokStr(tok->kind), tok->lexeme, tok->num,
      tok->linNum, tok->colNum);
  }
  fclose(f);
}

// ============================================================================
// Create a new Toks container
// ============================================================================
Toks* toksNew() {
  Toks* toks = malloc(sizeof(Toks));
  toks->tokNum = toks->hiTokNum = -1;
  return toks;
}

// ============================================================================
// Move the toks->tokNum 'cursor' forward one step.  But do not access the
// entry: at the end of the program, the cursor will point just beyond the end
// of the Toks array (ie, at entry toks->hiTokNum + 1)
// ============================================================================
Tok* toksNext(Toks* toks) {
  ++toks->tokNum;
  return toksCurr(toks);
}

// ============================================================================
// Return the previous Tok (ie, the one just before the toks->tokNum 'cursor')
// ============================================================================
Tok* toksPrev(Toks* toks) {
  --toks->tokNum;
  return toksCurr(toks);
}

// ============================================================================
// Rewind the Toks container so that 'toksCurr' will retrieve the first Tok
// ============================================================================
void toksRewind(Toks* toks) { toks->tokNum = 0; }
