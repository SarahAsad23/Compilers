// pse.h - Parser - Jim Hogg, 2020

#pragma once

#include <stdarg.h>

#include "ast.h"        // AstBody, etc
#include "toks.h"       // Toks

AstArg*    pseArg    (Toks* toks);
AstArg*    pseArgs   (Toks* toks);
AstAsg*    pseAsg    (Toks* toks);
AstBlock*  pseBlock  (Toks* toks);
AstBody*   pseBody   (Toks* toks);
AstCall*   pseCall   (Toks* toks);
AstExp*    pseExp    (Toks* toks);
AstFun*    pseFun    (Toks* toks);
AstIf*     pseIf     (Toks* toks);
int        pseIsAsg  (Toks* toks);
int        pseIsBop  (TokKind k);
int        pseIsCall (Toks* toks);
Tok*       pseMust   (Toks* toks, int numk, ...);
AstNam*    pseNam    (Toks* toks);
AstNum*    pseNum    (Toks* toks);
AstPar*    psePar    (Toks* toks);
AstPar*    psePars   (Toks* toks);
AstProg*   pseProg   (Toks* toks);
void       pseRep    (Toks* toks, char* s);
AstRet*    pseRet    (Toks* toks);
AstStm*    pseStm    (Toks* toks);
AstStm*    pseStms   (Toks* toks);
AstStr*    pseStr    (Toks* toks);
AstVar*    pseVar    (Toks* toks);
AstVar*    pseVars   (Toks* toks);
AstWhile*  pseWhile  (Toks* toks);

BOP pseTOKtoBOP(TokKind k);

void pseAppend(Ast* as, Ast* a);

