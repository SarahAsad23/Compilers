// pse.c - parser for SubC Compiler - converts tokens into AST - Jim Hogg, 2020

#include "pse.h"

// ============================================================================
// Append Ast 'a' onto the chain of Asts 'as', linked via the 'next'
// pointer in the Ast struct
// ============================================================================
void pseAppend(Ast* as, Ast* a) {
  Ast** p = &as->next;              // p is alias
  while (*p) p = &((*p)->next);
  *p = a;
}

// ============================================================================
// Arg => Nam | Num | Str
// ============================================================================
AstArg* pseArg(Toks* toks) {

  Tok* tok = toksCurr(toks);
  if (tok->kind == TOKRPAREN) return NULL;    // eg: sayl();

  tok = pseMust(toks, 3, TOKNAM, TOKNUM, TOKSTR);
  if (tok->kind == TOKNAM) {
    AstNam* nam = astNewNam(tok->lex);
    return astNewArg((Ast*) nam);
  } else if (tok->kind == TOKNUM) {
    AstNum* num = astNewNum(tok->num);
    return astNewArg((Ast*) num);
  } else if (tok->kind == TOKSTR) {
    AstStr* str = astNewStr(tok->lex);
    return astNewArg((Ast*) str);
  } else {
    return astNewArg(NULL);
  }
}

// ============================================================================
// Args => ( Arg ( "," Arg )* ) ?
// ============================================================================
AstArg* pseArgs(Toks* toks) {
  AstArg* args = pseArg(toks);
  if (args == NULL) return args;      // eg: sayl();

  Tok* tok = toksCurr(toks);
  while (tok->kind == TOKCOMMA) {
    tok = toksNext(toks);             // eat TOKCOMMA
    AstArg* arg = pseArg(toks);
    pseAppend((Ast*) args, (Ast*) arg);
    tok = toksCurr(toks);
  }
  return args;
}

// ============================================================================
// Asg => Nam "=" (Exp | Call) ";"
// ============================================================================
AstAsg* pseAsg(Toks* toks) {
  Tok* tok = pseMust(toks, 1, TOKNAM);            // eg: x
  pseMust(toks, 1, TOKEQ);                        // eg: =
  Ast* eoc = NULL;                                // Exp or Call
  if (pseIsCall(toks)) {
    eoc = (Ast*) pseCall(toks);
  } else {
    eoc = (Ast*) pseExp(toks);
  }
  AstNam* nam = astNewNam(tok->lex);
  pseMust(toks, 1, TOKSEMI);                      // ;
  return astNewAsg(nam, eoc);
}

// ============================================================================
// Block => "{" Stm+ "}"
// ============================================================================
AstBlock* pseBlock(Toks* toks) {
  pseMust(toks, 1, TOKLBRACE);
  AstStm* stms = pseStms(toks);
  pseMust(toks, 1, TOKRBRACE);
  return astNewBlock(stms);
}

// ============================================================================
// Body => "{" Var* Stm+ "}"
// ============================================================================
AstBody* pseBody(Toks* toks) {
  pseMust(toks, 1, TOKLBRACE);
  AstVar* vars = pseVars(toks);
  AstStm* stms = pseStms(toks);
  pseMust(toks, 1, TOKRBRACE);
  return astNewBody(vars, stms);
}

// ============================================================================
// Call => Nam "(" Args ")"
//
// Eg: add3(x, 15, y)
// ============================================================================
AstCall* pseCall(Toks* toks) {
  // nam
  Tok* tok = pseMust(toks, 1, TOKNAM);
  AstNam* nam = astNewNam(tok->lex); 
  // left paren 
  pseMust(toks, 1, TOKLPAREN); 
  // args 
  AstArg* arg = pseArgs(toks); 
  // right paren 
  pseMust(toks, 1, TOKRPAREN); 

  // return call node 
  return astNewCall(nam, arg); 
}

// ============================================================================
// Exp => NamNum | NamNum Bop NamNum
// ============================================================================
AstExp* pseExp(Toks* toks) {
  AstExp* exp = astNewExp(NULL, BOPNONE, NULL);

  Tok* tok = toksCurr(toks);

  if (tok->kind == TOKNUM) {                  // eg: 42
    exp->lhs = (Ast*) pseNum(toks);
  } else if (tok->kind == TOKNAM) {           // eg: abc
    exp->lhs = (Ast*) pseNam(toks);
  } else {
    utDie2Str("pseExp", "Invalid expression");
  }

  tok = toksCurr(toks);
  if (tok->kind == TOKSEMI) return exp;

  if (pseIsBop(tok->kind)) {                  // eg: +
    exp->bop = pseTOKtoBOP(tok->kind);
    tok = toksNext(toks);
    if (tok->kind == TOKNUM) {                // eg: 99
      exp->rhs = (Ast*) pseNum(toks);
    } else if (tok->kind == TOKNAM) {         // eg: xyz
      exp->rhs = (Ast*) pseNam(toks);
    } else {
      utDie2Str("pseExp", "Invalid expression");
    }
  }

  return exp;
}

// ============================================================================
// Fun => "int" Nam "(" Pars ")" Body
// ============================================================================
AstFun* pseFun(Toks* toks) {
  pseMust(toks, 1, TOKINT);                             // "int"
  Tok* tok = pseMust(toks, 1, TOKNAM);                  // eg: cat
  AstNam* astnam = astNewNam(tok->lex);

  pseMust(toks, 1, TOKLPAREN);
  AstPar* pars = psePars(toks);                         // eg: int a, int b
  pseMust(toks, 1, TOKRPAREN);

  AstBody* body = pseBody(toks);

  return astNewFun(astnam, pars, body);
}

// ============================================================================
// If => "if" "(" Exp ")" Block
// ============================================================================
AstIf* pseIf(Toks* toks) {
  // if
  pseMust(toks, 1, TOKIF); 
  // left paren
  pseMust(toks, 1, TOKLPAREN); 
  // expression 
  AstExp* exp = (AstExp*) pseExp(toks);
  // right paren 
  pseMust(toks, 1, TOKRPAREN);  
  // block 
  AstBlock* block = (AstBlock*) pseBlock(toks); 

  //return if node 
  return astNewIf(exp, block);
}

// ============================================================================
// Check whether the current name token (TOKNAM) is the start of an
// assignment: Asg => Nam "=" Exp ";"
// (It might equally well be the start of a function call)
// ============================================================================
int pseIsAsg(Toks* toks) {
  Tok* tok = toksPeek(toks);
  return tok->kind == TOKEQ;
}

// ============================================================================
// Check whether the Token kind specified by 'k' is legal binary operator
// ============================================================================
int pseIsBop(TokKind k) {
  return (k == TOKADD || k == TOKSUB || k == TOKMUL ||
          k == TOKLE  || k == TOKLT  || k == TOKNE  || k == TOKEEQ ||
          k == TOKGE  || k == TOKGT  || k == TOKLE  || k == TOKLT  ||
          k == TOKNE  || k == TOKEEQ || k == TOKGE  || k == TOKGT);
}

// ============================================================================
// Check whether the current name token (TOKNAM) is the start of a function
// call, such as "cmp(3, 4)"
// ============================================================================
int pseIsCall(Toks* toks) {
  Tok* tok = toksPeek(toks);
  return tok->kind == TOKLPAREN;
}

// ============================================================================
// Check that the current Token within 'toks' matches any of the 'numk'
// kinds in the varargs list.  If yes, advance to the next Token in 'toks'.
// If not, abort the program
//
// eg: pseMust(toks, 2, TOKNAM, TOKNUM)
// In this example, if the current Token does not match TOKNAM or TOKNUM,
// we build a diagnostic text in 'msg' and call dieStrTokStr to print a
// message like:
//    "ERROR: pseFun: Found TOKLET but expecting {TOKNAM, TOKNUM}"
// ============================================================================
Tok* pseMust(Toks* toks, int numk, ...) {

  // Prepare a failure message, just in case

  va_list argp;
  va_start(argp, numk);
  char msg[300] = "{";
  TokKind k;

  for (int i = 1; i <= numk - 1; ++i) {
    k = va_arg(argp, TokKind);
    strcat(msg, tokStr(k));   strcat(msg, ", ");
  }
  k = va_arg(argp, TokKind);
  strcat(msg, tokStr(k));     strcat(msg, "}");
  va_end(argp);

  // Now process the actual request

  if (toksAtEnd(toks)) {
    Tok* tok = tokNew(TOKBAD, "No more tokens", 0, NULL, 999, 999);
    utDieStrTokStr("pseMust", tok, msg);
  }

  Tok* tok = toksCurr(toks);

  va_start(argp, numk);                   // number of TokKind's
  for (int i = 1; i <= numk; ++i) {
    k = va_arg(argp, TokKind);
    if (tok->kind == k) {
      toksNext(toks);
      return tok;
    }
  }
  va_end(argp);

  // Failed to find a match.  So emit the diagnostic

  utDieStrTokStr("pseMust", tok, msg);
  return NULL;
}

// ============================================================================
// Nam => Alpha AlphaNum*
// ============================================================================
AstNam* pseNam(Toks* toks) {
  Tok* tok = pseMust(toks, 1, TOKNAM);
  return astNewNam(tok->lex);
}

// ============================================================================
// Num => [0-9]+
// ============================================================================
AstNum* pseNum(Toks* toks) {
  Tok* tok = pseMust(toks, 1, TOKNUM);
  return astNewNum(tok->num);
}

// ============================================================================
// Par => "int" Nam
// ============================================================================
AstPar* psePar(Toks* toks) {
  Tok* tok = toksCurr(toks);
  if (tok->kind == TOKRPAREN) return NULL;     // no parameters

  pseMust(toks, 1, TOKINT);
  AstNam* astnam = pseNam(toks);

  return  astNewPar(astnam);
}

// ============================================================================
// Pars => ( Par ( "," Par )* ) ?
// eg: ( int i , int j )
// ============================================================================
AstPar* psePars(Toks* toks) {
  AstPar* pars = psePar(toks);
  Tok* tok = toksCurr(toks);
  while (tok->kind == TOKCOMMA) {
    tok = toksNext(toks);             // eat TOKCOMMA
    AstPar* par = psePar(toks);
    pseAppend((Ast*) pars, (Ast*) par);
    tok = toksCurr(toks);
  }
  return pars;
}

// ============================================================================
// Prog => Fun+
// ============================================================================
AstProg* pseProg(Toks* toks) {
  AstFun* fun = pseFun(toks);                 // first function
  AstProg* prog = astNewProg(fun);
  while (toks->tokNum <= toks->hiTokNum) {
    AstFun* funNext = pseFun(toks);           // next function
    pseAppend((Ast*) fun, (Ast*) funNext);    // append onto funs chain
    fun = funNext;                            // move along chain
  }
  return prog;
}

// ============================================================================
// Reporter function for debugging
// ============================================================================
void pseRep(Toks* toks, char* s) {
  printf("%s", s);
  Tok* tok = toksCurr(toks);
  printf("%s %s \n", tokStr(tok->kind), tok->lex);
}

// ============================================================================
// Ret => "return" Exp ";"
// ============================================================================
AstRet* pseRet(Toks* toks) {
  pseMust(toks, 1, TOKRET);
  AstExp* exp = pseExp(toks);
  pseMust(toks, 1, TOKSEMI);
  return astNewRet(exp);
}

// ============================================================================
// Stm => If | Asg | Ret | While
// ============================================================================
AstStm* pseStm(Toks* toks) {
  Tok* tok = toksCurr(toks);
  TokKind k = tok->kind;
  if (k == TOKIF) {
    return (AstStm*) pseIf(toks);
  } else if (k == TOKNAM) {
    return (AstStm*) pseAsg(toks);
  } else if (k == TOKRET) {
    return (AstStm*) pseRet(toks);
  } else if (k == TOKWHILE) {
    return (AstStm*) pseWhile(toks);
  }
  utDieStrTokStr("pseStm", tok, "a statement");
  return NULL;
}

// ============================================================================
// Stms => Stm+
// ============================================================================
AstStm* pseStms(Toks* toks) {
  AstStm* stms = pseStm(toks);

  Tok* tok = toksCurr(toks);
  while (tok->kind != TOKRBRACE) {
    AstStm* stm = pseStm(toks);
    pseAppend((Ast*)stms, (Ast*)stm);
    tok = toksCurr(toks);
  }
  return stms;
}

// ============================================================================
// Parse a string literal, such as "hello world"
// ============================================================================
AstStr* pseStr(Toks* toks) {
  Tok* tok = pseMust(toks, 1, TOKSTR);
  return astNewStr(tok->lex);
}

// ============================================================================
// Var => "int" Nam ";"
// ============================================================================
AstVar* pseVar(Toks* toks) {
  Tok* tok = toksCurr(toks);
  if (tok->kind != TOKINT) return NULL;

  pseMust(toks, 1, TOKINT);
  Tok* tokNam = pseMust(toks, 1, TOKNAM);           // eg: count
  pseMust(toks, 1, TOKSEMI);                        // ";"
  AstNam* astnam = astNewNam(tokNam->lex);
  return astNewVar(astnam);                         // eg: count, int
}

// ============================================================================
// Vars => Var*
// ============================================================================
AstVar* pseVars(Toks* toks) {
  AstVar* vars = pseVar(toks);
  if (vars == NULL) return NULL;

  AstVar* var = pseVar(toks);
  while (var != NULL) {
    pseAppend((Ast*) vars, (Ast*) var);
    var = pseVar(toks);
  }
  return vars;
}

// ============================================================================
// While => "while" "(" Exp ")" Block
//
// eg: while (n < 10) { n = n + 1 ; }
// ============================================================================
AstWhile* pseWhile(Toks* toks) {
  // while
  pseMust(toks, 1, TOKWHILE); 
  // left paren
  pseMust(toks, 1, TOKLPAREN); 
  // expression 
  AstExp* exp = (AstExp*) pseExp(toks);
  // right paren 
  pseMust(toks, 1, TOKRPAREN); 
  // block 
  AstBlock* block = (AstBlock*) pseBlock(toks); 

  //return while node
  return astNewWhile(exp, block); 
}

// ============================================================================
// Convert TOK* to corresponding BOP*
// ============================================================================
BOP pseTOKtoBOP(TokKind k) {
  switch(k) {
    case TOKADD: return BOPADD;
    case TOKSUB: return BOPSUB;
    case TOKMUL: return BOPMUL;
    case TOKLE:  return BOPLE;
    case TOKLT:  return BOPLT;
    case TOKNE:  return BOPNE;
    case TOKEEQ: return BOPEEQ;
    case TOKGE:  return BOPGE;
    case TOKGT:  return BOPGT;
    default:     utDie2Str("TOKtoBOP", "Invalid input token");
  }
  return BOPBAD;               // pacify the compiler
}