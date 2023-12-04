// lay.h - Layout of function's Stack Frame - Jim Hogg, 2020

#pragma once

#include <assert.h>         // assert

#include "ast.h"            // TYP
#include "string.h"         // strcmp

// The ROLE enum comprises constants for the role, played by different
// identifiers in the Lay table (made up of individual rows)

typedef enum {
  ROLENA  = 0,  // where row.role does not apply
  ROLEEND,      // last row for a function
  ROLEFUN,      // first row for a function
  ROLEPAR,      // identifier is a parameter (not a variable or function)
  ROLEVAR,      // identifier is a variable  (not a parameter or function)
} ROLE;
char* layROLEtoStr(ROLE role);

#define LAYMAX 500

typedef struct {
  int hiIdx;                // index in row[] of last entry so far
  struct {
    char* nam;              // name of parvar
    TYP   typ;              // type of parvar - eg: TYPINT
    ROLE  role;             // ROLEPAR | ROLEVAR | ROLEFUN | ROLEEND
    int   off;              // offset from FP of parvar
  } row[LAYMAX];
} Lay;

void layAdd(Lay* lay, char* nam, TYP typ, ROLE role, int off);
void layBuild(Lay* lay, AstFun* astfun);
void layBuildIntrinsics(Lay* lay);
void layBuildPars(Lay* lay, AstPar* astpar);
void layBuildVars(Lay* lay, AstVar* astvar);
int  layCountVars(Lay* lay, int rownum);
void layDump(Lay* lay);
void layEnd(Lay* lay, AstFun* astfun);
int  layFindFunIdx(Lay* lay, char* funnam);
int  layFindVarParIdx(Lay* lay, char* funnam, char* nam);
void layFun(Lay* lay, AstFun* astfun);
Lay* layNew(int nrep);
void layRem(Lay* lay);
