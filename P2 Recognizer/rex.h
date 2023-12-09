// rex.h - Recognizer - Jim Hogg, 2020

#pragma once

#include <stdarg.h>

#include "toks.h"       // Toks

void rexArg(Toks* toks);
void rexArgs(Toks* toks);
void rexAsg(Toks* toks);
void rexAsgExp(Toks* toks); 
void rexBlock(Toks* toks);
void rexBody(Toks* toks);
void rexCall(Toks* toks);
void rexExp(Toks* toks);
void rexFun(Toks* toks);
void rexIf(Toks* toks);
int  rexIsBop(TokKind k);
int  rexIsCall(Toks* toks);
void rexMust(Toks* toks, TokKind k1);
void rexMust2(Toks* toks, TokKind k1, TokKind k2);
void rexMust3(Toks* toks, TokKind k1, TokKind k2, TokKind k3);
void rexMust4(Toks* toks, TokKind k1, TokKind k2, TokKind k3, TokKind k4);
void rexNamNum(Toks* toks) ;
void rexPar(Toks* toks);
void rexPars(Toks* toks);
void rexProg(Toks* toks);
void rexRep(Toks* toks, char* s);
void rexRet(Toks* toks);
void rexSayl(Toks* toks);
void rexSayn(Toks* toks);
void rexSays(Toks* toks);
void rexStm(Toks* toks);
void rexStms(Toks* toks);
void rexStr(Toks* toks);
void rexType(Toks* toks);
void rexVar(Toks* toks);
void rexVars(Toks* toks);
void rexWhile(Toks* toks);

