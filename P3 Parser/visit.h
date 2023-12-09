// visit.h - Visit every node of the AST - Jim Hogg, 2020

#pragma once

#include "ast.h"

#define INDENT 3

AstArg* visitArg(AstArg* ast);
void visitArgs(AstArg* ast);
void visitAsg(AstAsg* ast);
void visitBlock(AstBlock* ast);
void visitCall(AstCall* ast);
void visitExp(AstExp* ast);
void visitFun(AstFun* ast);
void visitFuns(AstFun* ast);
void visitIf(AstIf* ast);
void visitNam(AstNam* ast);
void visitNum(AstNum* ast);
void visitPar(AstPar* ast);
void visitProg(AstProg* ast);
void visitRet(AstRet* ast);
void visitStms(Ast* ast);
void visitStr(AstStr* ast);
void visitVar(AstVar* ast);
void visitVars(AstVar* ast);
void visitWhile(AstWhile* ast);
