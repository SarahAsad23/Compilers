// ast.h - Abstract Syntax Trees - Jim Hogg, 2020

#pragma once

#include <assert.h>         // assert
#include <ctype.h>          // isspace, isalpha, isdigit, isalnum
#include <stdio.h>          // printf
#include <stdlib.h>         // calloc
#include <string.h>         // strncpy

#include "tok.h"            // TokKind
#include "toks.h"           // Toks
#include "ut.h"             // ut*

typedef enum {
  TYPAST = 1, TYPFRAME, TYPINT, TYPFUN, TYPNONE, TYPSTR, TYPUNK
} TYP;
char* astTYPtoStr(TYP typ);

typedef enum {                                          // Binary Operators
  BOPADD = 1, BOPSUB, BOPMUL, BOPDIV, BOPNONE, BOPBAD,  // Arithmetic
  BOPLT, BOPLE, BOPNE, BOPEEQ, BOPGE, BOPGT             // Relational
} BOP;
char* astBOPtoStr(BOP bop);

typedef enum {
  ASTASG = 1, ASTARG, ASTBLOCK, ASTBODY, ASTCALL, ASTEXP, ASTFUN, ASTIF,
  ASTNAM, ASTNUM, ASTPAR, ASTPROG, ASTRET, ASTSTR, ASTVAR, ASTWHILE
} AST;

// ============================================================================
// Common 'super' struct for all ASTs
// ============================================================================
typedef struct Ast_ {
  AST    kind;
  struct Ast_* next;
} Ast;


struct AstCall_;    typedef struct AstCall_   AstCall;   // forward declaration
struct AstExp_;     typedef struct AstExp_    AstExp;    // forward declaration
struct AstNam_;     typedef struct AstNam_    AstNam;    // forward declaration
struct AstNum_;     typedef struct AstNum_    AstNum;    // forward declaration
struct AstPar_;     typedef struct AstPar_    AstPar;    // forward declaration
struct AstStm_;     typedef struct AstStm_    AstStm;    // forward declaration
struct AstStr_;     typedef struct AstStr_    AstStr;    // forward declaration
struct AstVar_;     typedef struct AstVar_    AstVar;    // forward declaration

// ============================================================================
// Arg => Nam | Num | Str
// ============================================================================
typedef struct {
  AST     kind;             // ASTARG
  Ast*    next;
  Ast*    nns;              // Nam, Num or Str
} AstArg;
AstArg* astNewArg(Ast* nns);

// ============================================================================
// Asg => Nam "=" (Exp | Call) ";"
// ============================================================================
typedef struct {
  AST     kind;             // ASTASG
  Ast*    next;
  AstNam* nam;
  Ast*    eoc;              // Exp or Call
} AstAsg;
AstAsg* astNewAsg(AstNam* nam, Ast* eoc);

// ============================================================================
// Block => "{" Stm+ "}"
// ============================================================================
typedef struct {
  AST     kind;             // ASTBLOCK
  Ast*    next;
  AstStm* stms;
} AstBlock;
AstBlock* astNewBlock(AstStm* stms);

// ============================================================================
// Body => Var* Stm+
// ============================================================================
typedef struct {
  AST     kind;             // ASTBODY
  Ast*    next;
  AstVar* vars;
  AstStm* stms;
} AstBody;
AstBody* astNewBody(AstVar* vars, AstStm* stms);

// ============================================================================
// Call => Nam "(" Args ")"
// ============================================================================
typedef struct AstCall_ {
  AST     kind;             // ASTCALL
  Ast*    next;
  AstNam* nam;
  AstArg* args;
} AstCall;
AstCall* astNewCall(AstNam* nam, AstArg* args);

// ============================================================================
// Exp => NamNum | NamNum Bop NamNum
// ============================================================================
typedef struct AstExp_ {
  AST  kind;                // ASTEXP
  Ast* next;
  Ast* lhs;
  BOP  bop;
  Ast* rhs;
} AstExp;
AstExp* astNewExp(Ast* lhs, BOP bop, Ast* rhs);

// ============================================================================
// Fun => "int" Nam "(" Pars ")" Body
// ============================================================================
typedef struct {
  AST       kind;           // ASTFUN
  Ast*      next;
  AstNam*   nam;
  AstPar*   pars;
  AstBody*  body;
} AstFun;
AstFun* astNewFun(AstNam* nam, AstPar* pars, AstBody* body);

// ============================================================================
// If => "if" "(" Exp ")" Block
// ============================================================================
typedef struct {
  AST       kind;           // ASTIF
  Ast*      next;
  AstExp*   exp;
  AstBlock* block;
} AstIf;
AstIf* astNewIf(AstExp* exp, AstBlock* block);

// ============================================================================
// An AST node that represent a name.  Typically, this is a reference to some
// variable, such as "counter" in the body of a function.  At that point, we
// do not know its Type.
// ============================================================================
typedef struct AstNam_ {
  AST   kind;               // ASTNAM
  Ast*  next;
  char* lex;                // lexeme
} AstNam;
AstNam* astNewNam(char* lex);

// ============================================================================
// An AST node that represent a simple, literal integer.  Eg: 42
// ============================================================================
typedef struct AstNum_ {
  AST    kind;              // ASTNUM
  Ast*   next;
  int    val;
} AstNum;
AstNum* astNewNum(int val);

// ============================================================================
// Par => "int" Nam
// ============================================================================
typedef struct AstPar_ {
  AST     kind;             // ASTPAR
  Ast*    next;
  AstNam* nam;
} AstPar;
AstPar* astNewPar(AstNam* nam);

// ============================================================================
// Prog => Fun+
// ============================================================================
typedef struct {
  AST     kind;             // ASTPROG
  Ast*    next;
  AstFun* funs;
} AstProg;
AstProg* astNewProg(AstFun* funs);

// ============================================================================
// Ret => "return" Exp ";"
// ============================================================================
typedef struct {
  AST     kind;             // ASTRET
  Ast*    next;
  AstExp* exp;
} AstRet;
AstRet* astNewRet(AstExp* exp);

// ============================================================================
// Stm => If | Asg | Ret | While
// ============================================================================
typedef struct AstStm_ {
  AST  kind;                // ASTIF | ASTASG | ASTRET | ASTWHILE
  Ast* next;
  Ast* stm;
} AstStm;
AstStm* AstNewStm(Ast* stm);

// ============================================================================
// An AST node that represent a simple, literal string.  Eg: "hello world"
// ============================================================================
typedef struct AstStr_ {
  AST   kind;               // ASTSTR
  Ast*  next;
  char* txt;
} AstStr;
AstStr* astNewStr(char* txt);

// ============================================================================
// Var => "int" Nam ";"
// ============================================================================
typedef struct AstVar_ {
  AST     kind;             // ASTVAR
  Ast*    next;
  AstNam* nam;
} AstVar;
AstVar* astNewVar(AstNam* nam);

// ============================================================================
// While => "while" "(" Exp ")" Block
// ============================================================================
typedef struct {
  AST       kind;           // ASTWHILE
  Ast*      next;
  AstExp*   exp;
  AstBlock* block;
} AstWhile;
AstWhile* astNewWhile(AstExp* exp, AstBlock* block);

AstFun* astFindFun(AstProg* astProg, char* nam);