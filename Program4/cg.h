// cg.h - Code Generator - Jim Hogg, 2020

#pragma once

#include <assert.h>     // assert

#include "ast.h"        // Ast*
#include "emit.h"       // Emit Buffer
#include "lay.h"        // Layout of stack frames
#include "ut.h"         // ut*

////#define LINESIZE 100

typedef struct {
  Lay*  lay;
  Emit* emit;
} Cg;

void  cgAsg   (Cg* cg, char* funnam, char* varnam);
void  cgAsgExp(Cg* cg, char* funnam, AstExp* astexp);
void  cgBlock (Cg* cg, char* funnam, AstBlock* astblock);
void  cgBody  (Cg* cg, char* funnam, AstBody* astbody);
void  cgBop   (Cg* cg, BOP bop);
void  cgBranch(Cg* cg, char* cond);
void  cgCall  (Cg* cg, char* funnam, AstCall* astcall);
void  cgEpilog(Cg* cg, char* funnam);
void  cgExp   (Cg* cg, char* funnam, AstExp* astexp);
void  cgFun   (Cg* cg, AstFun* astfun);
void  cgIf    (Cg* cg, char* funnam, AstIf* astif);
char* cgLabel();
void  cgNam   (Cg* cg, char* funnam, AstNam* astnam, char* reg);
Cg*   cgNew();
void  cgNum   (Cg* cg, AstNum* astnum, char* reg);
void  cgPar   (Cg* cg, AstPar* par);
void  cgProg  (Cg* cg, AstProg* astprog);
void  cgProlog(Cg* cg, char* funnam);
void  cgStm   (Cg* cg, char* funnam, AstStm* aststm);
void  cgStms  (Cg* cg, char* funnam, AstStm* aststm);
void  cgWhile (Cg* cg, char* funnam, AstWhile* astwhile);
