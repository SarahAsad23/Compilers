// visit.c - walk a SubC AST, vising every Node - Jim Hogg, 2020

#include "pin.h"
#include "visit.h"

// ========================================================
// Arg => Nam | Num | Str
// ========================================================
AstArg* visitArg(AstArg* ast) {
  pin(); printf("Arg \n"); pinMore();

  if (ast == NULL) {
    pin(); printf("str = NULL \n"); pinLess();
    return NULL;
  }

  if (ast->nns->kind == ASTNAM) {
    AstNam* nam = (AstNam*) ast->nns;
    pin(); printf("nam = %s \n", nam->lex);
  } else if (ast->nns->kind == ASTNUM) {
    AstNum* num = (AstNum*) ast->nns;
    pin(); printf("num = %d \n", num->val);
  } else {
    AstStr* str = (AstStr*) ast->nns;
    pin(); printf("str = \"%s\" \n", str->txt);
  }
  pinLess();

  return (AstArg*) ast->next;
}

// ========================================================
// Args => ( Arg ( "," Arg )* ) ?
// ========================================================
void visitArgs(AstArg* ast) {
  pin(); printf("Args \n"); pinMore();
  ast = visitArg(ast);
  while (ast != NULL) {
    ast = visitArg(ast);
  }
  pinLess();
}

// ========================================================
// Asg => Nam "=" (Exp | Call) ";"
// ========================================================
void visitAsg(AstAsg* ast) {
  pin(); printf("Asg \n"); pinMore();
  pin(); printf("nam = %s \n", ast->nam->lex);
  if (ast->eoc->kind == ASTEXP) {
    visitExp((AstExp*) ast->eoc);
  } else {
    visitCall((AstCall*) ast->eoc);
  }
  pinLess();
}

// ========================================================
// Block => "{" Stm+ "}"
// ========================================================
void visitBlock(AstBlock* ast) {
  pin(); printf("Block \n"); pinMore();
  visitStms((Ast*) ast->stms);
  pinLess();
}

// ========================================================
// Body => Var* Stm+
// ========================================================
void visitBody(AstBody* ast) {
  pin(); printf("Body \n"); pinMore();
  visitVars(ast->vars);
  visitStms((Ast*)(ast->stms));
  pinLess();
}

// ========================================================
// Call => Nam "(" Args ")"
// ========================================================
void visitCall(AstCall* ast) {
  pin(); printf("Call \n"); pinMore();
  pin(); printf("%s \n", ast->nam->lex);
  visitArgs((AstArg*) ast->args);
  pinLess();
}

// ========================================================
// Exp => NamNum | NamNum Bop NamNum
// ========================================================
void visitExp(AstExp* ast) {
  pin(); printf("Exp \n"); pinMore();

  AST lhsKind = ast->lhs->kind;

  if (lhsKind == ASTNAM) {
    visitNam((AstNam*) ast->lhs);
  } else if (lhsKind == ASTNUM) {
    visitNum((AstNum*) ast->lhs);
  }

  if (ast->bop == BOPNONE) {
    pinLess();
    return;
  }

  pin(); printf("Bop = %s \n", astBOPtoStr(ast->bop));

  AST rhsKind = ast->rhs->kind;
  if (ast->bop != BOPNONE) {                 // NamLit Bop NamLit
    if (rhsKind == ASTNUM) {
      visitNum((AstNum*) ast->rhs);
    } else if (rhsKind == ASTNAM) {
      visitNam((AstNam*) ast->rhs);
    }
  }

  pinLess();
}

// ========================================================
// Fun => "int" Nam "(" Pars ")" Body
// ========================================================
void visitFun(AstFun* ast) {
  pin(); printf("Fun \n"); pinMore();
  pin(); printf("nam = %s \n", ast->nam->lex);
  pin(); printf("typ = int \n");
  AstPar* par = ast->pars;
  while (par != NULL) {
    visitPar(par);
    par = (AstPar*) par->next;
  }
  visitBody(ast->body);
  pinLess();
}

// ========================================================
// Funs => Fun+
// ========================================================
void visitFuns(AstFun* ast) {
  while (ast != NULL) {
    visitFun(ast);
    ast = (AstFun*) ast->next;
  }
}

// ========================================================
// If => "if" "(" Exp ")" Block
// ========================================================
void visitIf(AstIf* ast) {
  pin(); printf("If \n"); pinMore();
  visitExp(ast->exp);
  visitBlock(ast->block);
  pinLess();
}

// ========================================================
// Nam => Alpha AlphaNum*
// ========================================================
void visitNam(AstNam* ast) {
  pin(); printf("nam = %s \n", ast->lex);
}

// ========================================================
// Num => [0-9]+
// ========================================================
void visitNum(AstNum* ast) {
  pin(); printf("num = %d \n", ast->val);
}

// ========================================================
// Par => "int" Nam
// ========================================================
void visitPar(AstPar* ast) {
  pin(); printf("Par \n"); pinMore();
  pin(); printf("nam = %s \n", ast->nam->lex);
  pin(); printf("typ = int\n");
  pinLess();
}

// ========================================================
// Prog => Fun+
// ========================================================
void visitProg(AstProg* astProg) {
  pin(); printf("Prog \n"); pinMore();
  AstFun* ast = astProg->funs;
  while (ast != NULL) {
    visitFun(ast);
    ast = (AstFun*) ast->next;
  }

  pinLess();
  printf("\n\n");
}

// ========================================================
// Ret => "return" Exp ";"
// ========================================================
void visitRet(AstRet* ast) {
  pin(); printf("Ret \n"); pinMore();
  visitExp(ast->exp);
  pinLess();
}

// ========================================================
// Stm => If | Asg | Ret | While
// ========================================================
void visitStms(Ast* ast) {
  while (ast != NULL) {
    switch(ast->kind) {
      case ASTASG:   visitAsg  ((AstAsg*)   ast);   break;
      case ASTIF:    visitIf   ((AstIf*)    ast);   break;
      case ASTRET:   visitRet  ((AstRet*)   ast);   break;
      case ASTWHILE: visitWhile((AstWhile*) ast);   break;
      default:                                      break;
    }
    ast = ast->next;
  }
}

// ========================================================
void visitStr(AstStr* ast) {
  pin(); printf("Str = %s", ast->txt);
}

// ========================================================
// Var => "int" Nam ";"
// ========================================================
void visitVar(AstVar* ast) {
  pin(); printf("Var \n"); pinMore();
  pin(); printf("nam = %s \n", ast->nam->lex);
  pin(); printf("typ = int \n");
  pinLess();
}

// ========================================================
// Vars => Var*
// ========================================================
void visitVars(AstVar* ast) {
  if (ast == NULL) return;      // function has no vars
  pin(); printf("Vars \n"); pinMore();
  visitVar(ast);
  ast = (AstVar*) ast->next;
  while (ast != NULL) {
    visitVar((AstVar*) ast);
    ast = (AstVar*) ast->next;
  }
  pinLess();
}

// ========================================================
// While => "while" "(" Exp ")" Block
// ========================================================
void visitWhile(AstWhile* ast) {
  pin(); printf("While \n"); pinMore();
  visitExp(ast->exp);
  visitBlock(ast->block);
  pinLess();
}