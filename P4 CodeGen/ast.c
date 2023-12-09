// ast.c - Abstract Syntax Tree - Jim Hogg, 2020

#include "ast.h"

// ============================================================================
// 'astarg' is the head of the list of arguments in the call made to some
// function.  This function counts how many arguments are in the list.
//
// (Clearly, the number of arguments (astCountArgs) should match the number of
// parameters (astCountPars) in that function)
// ============================================================================
int astCountArgs(AstArg* astarg) {
  int numarg = 0;
  while (astarg) {
    ++numarg;
    astarg = (AstArg*) astarg->next;
  }
  return numarg;
}

// ============================================================================
// 'astpar' is the head of the list of parameters in the definition of some
// function.  This function counts how many parameters are in the list.
// ============================================================================
int astCountPars(AstPar* astpar) {
  int numpar = 0;
  while (astpar) {
    ++numpar;
    astpar = (AstPar*) astpar->next;
  }
  return numpar;
}

// ============================================================================
// 'astvar' is the head of the list of local variables in the definition of
// some function.  This function counts how many variables are in the list.
// ============================================================================
int astCountVars(AstVar* astvar) {
  int numvar = 0;
  while (astvar) {
    ++numvar;
    astvar = (AstVar*) astvar->next;
  }
  return numvar;
}

// ============================================================================
// 'astarg' points at the first argument for a function.  Retrieve argument
// number 'argnum' and return to caller (arguments are numbered 0 upwards).
// If not found, abort.
//
// Note: this function is called to retrieve arguments in right-to-left order
// when building the code for a function call.
// ============================================================================
AstArg* astFindArg(AstArg* astarg, int argnum) {
  int curargnum = 1;
  AstArg* curastarg = astarg;
  while (curastarg) {
    if (curargnum == argnum) return curastarg;
    curastarg = (AstArg*) curastarg->next;
    curargnum++;
  }
  utDie2StrInt("astFindArg", "Failed to find argument number", argnum);
  return 0;                                   // pacify compiler
}

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
    case TYPEND:   return "TYPEND";
    case TYPFUN:   return "TYPFUN";
    case TYPINT:   return "TYPINT";
    case TYPSTR:   return "TYPSTR";
    case TYPUNK:   return "TYPUNK";
    default:       return "TYPUNK";
  }
}
