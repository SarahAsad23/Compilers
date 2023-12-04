// lay.c - Layout of a function's Stack Frame - Jim Hogg, 2022

#include "lay.h"

// ============================================================================
// Add a row to the Layout call 'lay'
//
// nam  : name of the par/var
// typ  : type of the par/var (TYPINT|TYPSTR|TYPFUN)
// role : ROLEPAR|ROLEVAR|ROLEFUN|ROLEEND
// off  : byte offset from FP, in the runtime stack frame, for this par/var
// ============================================================================
void layAdd(Lay* lay, char* nam, TYP typ, ROLE role, int off) {
  lay->hiIdx++;
  assert(lay->hiIdx < LAYMAX);
  lay->row[lay->hiIdx].nam  = nam;
  lay->row[lay->hiIdx].typ  = typ;
  lay->row[lay->hiIdx].role = role;
  lay->row[lay->hiIdx].off  = off;
}

// ============================================================================
// Build a Layout for the function defined by 'astfun'
// ============================================================================
void layBuild(Lay* lay, AstFun* astfun) {
  layFun(lay, astfun);                          // ROLEFUN row
  if (astfun->pars) {
    layBuildPars(lay, astfun->pars);            // parameter rows
  }
  if (astfun->body && astfun->body->vars) {
    layBuildVars(lay, astfun->body->vars);      // variable rows
  }
  layEnd(lay, astfun);                          // ROLEND row
  ///layDump(lay);                              // debug
}

// ============================================================================
// Build the Layout for the intrinsic functions says, sayn and sayl
// ============================================================================
void layBuildIntrinsics(Lay* lay) {
  AstNam* funnam = NULL;                        // says, sayn, sayl
  AstNam* parnam = NULL;                        // parameter name
  AstPar* par    = NULL;                        // parameter
  AstFun* fun    = NULL;                        // function

  funnam = astNewNam("says");
  parnam = astNewNam("x");
  par    = astNewPar(parnam);
  fun    = astNewFun(funnam, par, NULL);
  layBuild(lay, fun);

  funnam = astNewNam("sayn");
  parnam = astNewNam("x");
  par    = astNewPar(parnam);
  fun    = astNewFun(funnam, par, NULL);
  layBuild(lay, fun);

  funnam = astNewNam("sayl");
  fun = astNewFun(funnam, NULL, NULL);
  layBuild(lay, fun);
}

// ============================================================================
// Add rows into 'lay' for the list of pars defined by 'astpar'
// ============================================================================
void layBuildPars(Lay* lay, AstPar* astpar) {
  //++ Complete this function
}

// ============================================================================
// Add rows into 'lay' for the list of (local) variables defined by 'astvar'
// ============================================================================
void layBuildVars(Lay* lay, AstVar* astvar) {

  if (astvar == NULL) return;       // function has no local variables

  int off = -4;                     // offset from FP of first variable

  while (astvar) {
    layAdd(lay, astvar->nam->lex, TYPINT, ROLEVAR, off);
    off -= 4;
    astvar = (AstVar*) astvar->next;
  }
}

// ============================================================================
// Count the number of local variables.  'rownum' gives the index, within
// the row[] array, of the ROLEFUN row for the function of interest.
// ============================================================================
int layCountVars(Lay* lay, int rownum) {
  int numvars = 0;                            // # of local variables so far
  ++rownum;                                   // row holding first local variable
  while (lay->row[rownum].typ  != 0 &&        // end of row[] array
         lay->row[rownum].role != ROLEEND) {  // end of this function's rows
    if (lay->row[rownum].role  == ROLEVAR) {
      ++numvars;
    }
    ++rownum;
  }
  return numvars;
}

// ============================================================================
// Dump the contents of 'lay' to the console, for debugging
// ============================================================================
void layDump(Lay* lay) {
  //++ Complete this function
}

// ============================================================================
// End the current new function layout
// ============================================================================
void layEnd(Lay* lay, AstFun* astfun) {
  layAdd(lay, astfun->nam->lex, TYPEND, ROLEEND, 0);
}

// ============================================================================
// Search the rows of 'lay' for the function called 'funnam'.  Return the
// index of the TYPFUN row that matches 'funnam'.  We use a simple, slow,
// linear search.  Abort if not found.
// ============================================================================
int layFindFunIdx(Lay* lay, char* funnam) {
  int rownum = 0;
  while (lay->row[rownum].typ != 0) {                   // end of row[] array
    if (lay->row[rownum].role == ROLEFUN) {             // start of function
      if (strcmp(funnam, lay->row[rownum].nam) == 0) {
        return rownum;
      }
    }
    ++rownum;
  }
  utDie3Str("layFindFunIdx", "Cannot find function ", funnam);
  return 0;                                             // pacify compiler
}

// ============================================================================
// Search the rows of 'lay' for the variable or parameter ("varpar") called
// 'nam', in the function called 'funnam'.  Return the index, within the
// row[] array, of that entry.  Note that variables and parameters must be
// unique in a valid SubC program (although the SubC compiler does not enforce
// this condition).  We use a simple, slow, linear search.  Abort if not found
// ============================================================================
int layFindVarParIdx(Lay* lay, char* funnam, char* nam) {
  int rownum = layFindFunIdx(lay, funnam);            // index of TYPFUN row

  rownum++;                                           // first parvar

  while (lay->row[rownum].typ != 0) {                 // end of row[]
    while (lay->row[rownum].typ != TYPEND) {          // end of function
      char* thisnam = lay->row[rownum].nam;
      if (strcmp(nam, thisnam) == 0) {                // match!
        return rownum;
      }
      ++rownum;
    }
  }
  utDie5Str("layFindVarParIdx", "Cannot find varpar", nam, "in function", funnam);
  return 0;                                           // pacify compiler
}

// ============================================================================
// Start a new function layout
// ============================================================================
void layFun(Lay* lay, AstFun* astfun) {
  layAdd(lay, astfun->nam->lex, TYPFUN, ROLEFUN, 0);
}

// ============================================================================
// Build a new, empty Layout ('nrep' repeats of a Lay struct)
// ============================================================================
Lay* layNew(int nrep) {
  Lay* lay = calloc(nrep * sizeof(Lay), 1);
  lay->hiIdx = -1;                      // no rows
  return lay;
}

// ============================================================================
// Convert a member of the ROLE enum into its display string
// ============================================================================
char* layROLEtoStr(ROLE role) {
  switch(role) {
    case ROLENA:  return "ROLENA";
    case ROLEEND: return "ROLEEND";
    case ROLEFUN: return "ROLEFUN";
    case ROLEPAR: return "ROLEPAR";
    case ROLEVAR: return "ROLEVAR";
    default:      return "ROLENA";
  }
}
