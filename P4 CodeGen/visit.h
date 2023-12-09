// visit.h - Visit every node of the AST - Jim Hogg, 2020

#pragma once

#include "ast.h"
#include "pin.h"

#define INDENT 3

AstArg* visitArg(AstArg* astarg);
void visitArgs(AstArg* astarg);
void visitAsg(AstAsg* astasg);
void visitBlock(AstBlock* astblock);
void visitCall(AstCall* astcall);
void visitExp(AstExp* astexp);
void visitFun(AstFun* astfun);
void visitFuns(AstFun* astfun);
void visitIf(AstIf* astif);
void visitNam(AstNam* astnam);
void visitNum(AstNum* astnum);
void visitPar(AstPar* astpar);
void visitProg(AstProg* astprog);
void visitRet(AstRet* astret);
void visitStms(Ast* aststm);
void visitStr(AstStr* aststr);
void visitVar(AstVar* astvar);
void visitVars(AstVar* astvar);
void visitWhile(AstWhile* astwhile);
