// ast.c - Abstract Syntax Tree - Jim Hogg, 2020

#include "ast.h"

// ============================================================================
// Search the program AST rooted at 'prog' looking for the function
// called 'nam'.  If not found, return NULL
// ============================================================================
AstFun* astFindFun(AstProg* prog, char* lex) {
  assert(prog->kind == ASTPROG);
  AstFun* fun = prog->funs;
  while (fun) {
    if (strcmp(fun->nam->lex, lex) == 0) return fun;
    fun = (AstFun*) fun->next;
  }
  return NULL;
}

AstArg* astNewArg(Ast* nns) {
  AstArg* a = calloc(sizeof(AstArg), 1);
  a->kind = ASTARG;
  a->nns = nns;     // Nam, Num or Str
  return a;
}

AstAsg* astNewAsg(AstNam* nam, Ast* eoc) {
  AstAsg* a = calloc(sizeof(AstAsg), 1);
  a->kind = ASTASG; a->nam = nam; a->eoc = eoc;
  return a;
}

AstBlock* astNewBlock(AstStm* stms) {
  AstBlock* a = calloc(sizeof(AstBlock), 1);
  a->kind = ASTBLOCK; a->stms = stms;
  return a;
}

AstBody* astNewBody(AstVar* vars, AstStm* stms) {
  AstBody* a = calloc(sizeof(AstBody), 1);
  a->kind = ASTBODY; a->vars = vars; a->stms = stms;
  return a;
}

AstCall* astNewCall(AstNam* nam, AstArg* args) {
  AstCall* a = calloc(sizeof(AstCall), 1);
  a->kind = ASTCALL; a->nam = nam; a->args = args;
  return a;
}

AstExp* astNewExp(Ast* lhs, BOP bop, Ast* rhs) {
  AstExp* a = calloc(sizeof(AstExp), 1);
  a->kind = ASTEXP; a->lhs = lhs; a->bop = bop; a->rhs = rhs;
  return a;
}

AstFun* astNewFun(AstNam* nam, AstPar* pars, AstBody* body) {
  AstFun* a = calloc(sizeof(AstFun), 1);
  a->kind = ASTFUN; a->nam = nam; a->pars = pars; a->body = body;
  return a;
}

AstIf* astNewIf(AstExp* exp, AstBlock* block) {
  AstIf* a = calloc(sizeof(AstIf), 1);
  a->kind = ASTIF; a->exp = exp; a->block = block;
  return a;
}

AstNam* astNewNam(char* lex) {
  AstNam* a = calloc(sizeof(AstNam), 1);
  a->kind = ASTNAM; a->lex = lex;
  return a;
}

AstNum* astNewNum(int val) {
  AstNum* a = calloc(sizeof(AstNum), 1);
  a->kind = ASTNUM; a->val = val;
  return a;
}

AstPar* astNewPar(AstNam* nam) {
  AstPar* a = calloc(sizeof(AstPar), 1);
  a->kind = ASTPAR; a->next = 0; a->nam = nam;
  return a;
}

AstProg* astNewProg(AstFun* funs) {
  AstProg* a = calloc(sizeof(AstProg), 1);
  a->kind = ASTPROG; a->funs = funs;
  return a;
}

AstRet* astNewRet(AstExp* exp) {
  AstRet* a = calloc(sizeof(AstRet), 1);
  a->kind = ASTRET; a->exp = exp;
  return a;
}

AstStr* astNewStr(char* txt) {
  AstStr* a = calloc(sizeof(AstStr), 1);
  a->kind = ASTSTR; a->txt = txt;
  return a;
}

AstVar* astNewVar(AstNam* nam) {
  AstVar* a = calloc(sizeof(AstVar), 1);
  a->kind = ASTVAR; a->next = 0; a->nam = nam;
  return a;
}

AstWhile* astNewWhile(AstExp* exp, AstBlock* block) {
  AstWhile* a = calloc(sizeof(AstWhile), 1);
  a->kind = ASTWHILE; a->exp = exp; a->block = block;
  return a;
}

char* astBOPtoStr(BOP bop) {
  switch(bop) {
    case BOPADD:   return "BOPADD";
    case BOPBAD:   return "BOPBAD";
    case BOPDIV:   return "BOPDIV";
    case BOPEEQ:   return "BOPEEQ";
    case BOPGE:    return "BOPGE";
    case BOPGT:    return "BOPGT";
    case BOPLE:    return "BOPLE";
    case BOPLT:    return "BOPLT";
    case BOPMUL:   return "BOPMUL";
    case BOPNE:    return "BOPNE";
    case BOPNONE:  return "BOPNONE";
    case BOPSUB:   return "BOPSUB";
    default:       return "BOPBAD";
  }
}

char* astTYPtoStr(TYP typ) {
  switch(typ) {
    case TYPFRAME: return "TYPFRAME";
    case TYPFUN:   return "TYPFUN";
    case TYPINT:   return "TYPINT";
    case TYPNONE:  return "TYPNONE";
    case TYPSTR:   return "TYPSTR";
    case TYPUNK:   return "TYPUNK";
    default:       return "TYPUNK";
  }
}
