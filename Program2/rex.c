// rex.c - Recognizer for SubC Compiler - Jim Hogg, 2020

#include "rex.h"

// ============================================================================
// Arg => NamNum | Str
// ============================================================================
void rexArg(Toks* toks) {
  rexMust3(toks, TOKNAM, TOKNUM, TOKSTR);
}

// ============================================================================
// Args => ( Arg ( "," Arg )* ) ?
// ============================================================================
void rexArgs(Toks* toks) {
  rexArg(toks);
  Tok* tok = toksCurr(toks);
  while (tok->kind == TOKCOMMA) {
    tok = toksNext(toks);               // eat TOKCOMMA
    rexArg(toks);
    tok = toksCurr(toks);
  }
}

// ============================================================================
// Asg => Nam "=" AsgExp ";"
// eg:    x = 42;
// eg:    x = add2(y);
// ============================================================================
void rexAsg(Toks* toks) {
  rexMust(toks, TOKNAM);      // eg: counter
  rexMust(toks, TOKEQ);       // eg: =
  rexAsgExp(toks);            // eg: 42, add2(x, 3)
  rexMust(toks, TOKSEMI);     // eg: ;
}

// ============================================================================
// AsgExp => Exp | Call
// Call   => Nam "(" Args ")"
// eg:       42  | add2(x, 3)
// ============================================================================
void rexAsgExp(Toks* toks) {
  if (rexIsCall(toks)) {
    rexMust(toks, TOKNAM);      // eg: add2
    rexMust(toks, TOKLPAREN);   // eg: (
    rexArgs(toks);              // eg: x, 3
    rexMust(toks, TOKRPAREN);   // eg: )
  } else {
    rexExp(toks);               // eg: y + 42
  }
}

// ============================================================================
// Block => "{" Stm+ "}"
// ============================================================================
void rexBlock(Toks* toks) {
  rexMust(toks, TOKLBRACE);
  rexStms(toks);
  rexMust(toks, TOKRBRACE);
}

// ============================================================================
// Body => "{" Var* Stm+ "}"
// ============================================================================
void rexBody(Toks* toks) {
  rexMust(toks, TOKLBRACE);
  rexVars(toks);
  rexStms(toks);
  rexMust(toks, TOKRBRACE);
}

// ============================================================================
// Call => Nam "(" Args ")"
//
// Eg: add3(x, 15, y)
//
// At the point this function is called, its caller has already consumed
// the token for Nam
// ============================================================================
void rexCall(Toks* toks) {
  // rexMust(toks, TOKNAM);                         // eg: "add3"
  rexMust(toks, TOKLPAREN);                         // eg: "("
  rexArgs(toks);                                    // eg: "x, 15, y"
  rexMust(toks, TOKRPAREN);                         // eg: ")"
}

// ============================================================================
// Exp => NamNum | NamNum Bop NamNum
//
// Eg: 42 | count | x + 23
// ============================================================================
void rexExp(Toks* toks) {
  rexMust3(toks, TOKNAM, TOKNUM, TOKSTR);
  Tok* tok1 = toksPrev(toks);             // grab back NAM or NUM

  if (tok1->kind == TOKSTR) {             // eg: "hello"
    rexStr(toks);
    return;
  }

  Tok* tok2 = toksNext(toks);
  if (tok2->kind == TOKSEMI) return;      // eg: 23; or count;

  if (rexIsBop(tok2->kind)) {             // eg: 23 +
    toksNext(toks);                       // eg: count
    rexMust2(toks, TOKNAM, TOKNUM);
  }
}

// ============================================================================
// Fun => "int" Nam    "(" Pars ")" Body
//      | "int" "main" "("      ")" Body
//
// Eg: int add3(int a, int b, int c)
//
// Note that we treat function "main" as a special case - int return
// type and no params.  Ie: "int main()"
// ============================================================================
void rexFun(Toks* toks) {
  rexType(toks);
  rexMust(toks, TOKNAM);                    // eg: add
  Tok* tok = toksPrev(toks);                // retrieve TOKNAM
  if (strcmp(tok->lexeme, "main") == 0) {   // "main" function
    tok = toksNext(toks);                   // should be (
    rexMust(toks, TOKLPAREN);               // (
    rexMust(toks, TOKRPAREN);               // )
  } else {
    tok = toksNext(toks);                   // should be (
    rexMust(toks, TOKLPAREN);               // (
    rexPars(toks);                          // eg: int a, int b, int c
    rexMust(toks, TOKRPAREN);               // )
  }
  rexBody(toks);
}

// ============================================================================
// Typ => "int"
// ============================================================================
void rexType(Toks* toks) {
  rexMust(toks, TOKINT);
}

// ============================================================================
// If => "if" "(" Exp ")" Block
// ============================================================================
void rexIf(Toks* toks) {

  //++ Insert code here to recognize a valid SubC 'if' construct.
  //++ This will require 5 lines of code.  Be guided by the grammar
  //++ snippet in the comment above this function.

}

// ============================================================================
// Check whether the Token kind specified by 'k' is legal binary operator
// ============================================================================
int rexIsBop(TokKind k) {
  return (k == TOKADD || k == TOKSUB || k == TOKMUL ||
          k == TOKLE  || k == TOKLT  || k == TOKNE  || k == TOKEEQ ||
          k == TOKGE  || k == TOKGT  || k == TOKLE  || k == TOKLT  ||
          k == TOKNE  || k == TOKEEQ || k == TOKGE  || k == TOKGT);
}

// ============================================================================
// Check whether the current name token (TOKNAM) is the start of a Call.
// Eg: "add2(x, 3)".  (It might equally well be the start of an expression
// such as "x + 3"
// ============================================================================
int rexIsCall(Toks* toks) {
  Tok* tok = toksPeek(toks);
  return tok->kind == TOKLPAREN;
}

// ============================================================================
// Check that the kind of the current Token within 'toks' matches 'k1'.
// If yes, advance to the next Token in 'toks'.  If not, abort the program.
// ============================================================================
void rexMust(Toks* toks, TokKind k1) {
  Tok* tok = toksCurr(toks);              // current token
  TokKind k = tok->kind;
  if (k == k1) {                          // success!
    toksNext(toks);                       // advance to next token
    return;
  }

  char msg[150];
  sprintf(msg, "but expecting {%s}", tokStr(k1));
  utDieStrTokStr("rexMust", tok, msg);
}


// ============================================================================
// Check that the kind of the current Token within 'toks' matches 'k1' or 'k2'.
// If yes, advance to the next Token in 'toks'.  If not, abort the program.
// ============================================================================
void rexMust2(Toks* toks, TokKind k1, TokKind k2) {
  Tok* tok = toksCurr(toks);              // current token
  TokKind k = tok->kind;
  if (k == k1 || k == k2) {               // success!
    toksNext(toks);                       // advance to next token
    return;
  }

  char msg[150];
  sprintf(msg, "%s%s, %s}", "but expecting {", tokStr(k1), tokStr(k2));
  utDieStrTokStr("rexMust", tok, msg);
}

// ============================================================================
// Check that the kind of the current Token within 'toks' matches 'k1' or 'k2'
// or 'k3'.  If yes, advance to the next Token in 'toks'.  If not, abort
// the program.
// ============================================================================
void rexMust3(Toks* toks, TokKind k1, TokKind k2, TokKind k3) {
  Tok* tok = toksCurr(toks);              // current token
  TokKind k = tok->kind;
  if (k == k1 || k == k2 || k == k3) {    // success!
    toksNext(toks);                       // advance to next token
    return;
  }

  char msg[150];
  sprintf(msg, "%s%s, %s, %s}", "but expecting {", tokStr(k1), tokStr(k2), tokStr(k3));
  utDieStrTokStr("rexMust", tok, msg);
}

// ============================================================================
// Check that the kind of the current Token within 'toks' matches 'k1' thru
// 'k4'.  If yes, advance to the next Token in 'toks'.  If not, abort
// the program.
// ============================================================================
void rexMust4(Toks* toks, TokKind k1, TokKind k2, TokKind k3, TokKind k4) {
  Tok* tok = toksCurr(toks);                        // current token
  TokKind k = tok->kind;
  if (k == k1 || k == k2 || k == k3 || k == k4) {   // success!
    toksNext(toks);                                 // advance to next token
    return;
  }

  char msg[150];
  sprintf(msg, "%s%s, %s, %s, %s}", "but expecting {", tokStr(k1), tokStr(k2), tokStr(k3), tokStr(k4));
  utDieStrTokStr("rexMust", tok, msg);
}

// ============================================================================
// NamNum => Nam | Num
// ============================================================================
void rexNamNum(Toks* toks) {
  rexMust2(toks, TOKNAM, TOKNUM);
}

// ============================================================================
// Par => "int" Nam
// eg:    int a
// ============================================================================
void rexPar(Toks* toks) {
  rexType(toks);                    // eg: int
  rexMust(toks, TOKNAM);            // eg: counter
}

// ============================================================================
// Pars  => ( Par ( "," Par )* ) ?
// eg:       int a, int b, int c
// ============================================================================
void rexPars(Toks* toks) {
  Tok* tok = toksCurr(toks);
  while (tok->kind != TOKRPAREN) {
    rexPar(toks);
    tok = toksCurr(toks);
    if (tok->kind == TOKCOMMA) tok = toksNext(toks);
  }
}

// ============================================================================
// Prog => Fun+
// ============================================================================
void rexProg(Toks* toks) {
  rexFun(toks);                               // first function
  toksCurr(toks);                             // next Tok
  while (!toksAtEnd(toks)) {
    rexFun(toks);                             // next function
    toksCurr(toks);                           // next Tok
  }
  printf("INFO: Recognizer: Input Program is valid \n\n");
}

// ============================================================================
// Reporter function for debugging
// ============================================================================
void rexRep(Toks* toks, char* s) {
  printf("%s", s);
  Tok* tok = toksCurr(toks);
  printf("%s %s \n", tokStr(tok->kind), tok->lexeme);
}

// ============================================================================
// Ret => "return" Exp ";"
// ============================================================================
void rexRet(Toks* toks) {
  rexMust(toks, TOKRET);
  rexExp(toks);
  rexMust(toks, TOKSEMI);
}

// ============================================================================
// Stm    => If | Asg | Ret | While
// If     => "if" "(" Exp ")" Block
// Asg    => Nam "=" AsgExp ";"
// Ret    => "return" Exp ";"
// While  => "while" "(" Exp ")" Block
// ============================================================================
void rexStm(Toks* toks) {
  Tok* tok = toksCurr(toks);
  TokKind k = tok->kind;
  if (k == TOKIF) {
    rexIf(toks);
  } else if (k == TOKRET) {
    rexRet(toks);
  } else if (k == TOKNAM) {
    rexAsg(toks);
  } else if (k == TOKRET) {
    rexRet(toks);
  } else if (k == TOKWHILE) {
    rexWhile(toks);
  } else {
    utDieStrTokStr("rexStm", tok, "a statement");
  }
}

// ============================================================================
// Stms => Stm+
// ============================================================================
void rexStms(Toks* toks) {
  rexStm(toks);

  TokKind k = toksCurr(toks)->kind;
  while (k != TOKRBRACE) {
    rexStm(toks);
    Tok* tok = toksCurr(toks);
    k = tok->kind;
  }
}

// ============================================================================
void rexStr(Toks* toks) {
  rexMust(toks, TOKSTR);
}

// ============================================================================
// Var  => "int" Nam ";"
// eg:      int count;
// ============================================================================
void rexVar(Toks* toks) {
  rexType(toks);                        // eg: int
  rexMust(toks, TOKNAM);                // eg: count
  rexMust(toks, TOKSEMI);               // eg: ;
}

// ============================================================================
// Vars => Var*
// ============================================================================
void rexVars(Toks* toks) {
  Tok* tok = toksCurr(toks);
  if (tok->kind != TOKINT) return;      // no Vars in current function

  rexVar(toks);                         // first "var"
  tok = toksCurr(toks);
  while (tok->kind == TOKINT) {         // subsequent "var"s
    rexVar(toks);
    tok = toksCurr(toks);
  }
}

// ============================================================================
// While => "while" "(" Exp ")" Block
// eg:      while (n < 10) { n = n + 1; }
// ============================================================================
void rexWhile(Toks* toks) {

  //++ Insert code here to recognize a valid SubC 'while' construct.

}
