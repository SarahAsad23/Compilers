// cg.c - Code Generator for SubC ASTs - Jim Hogg, 2022

#include "cg.h"

// ============================================================================
// Asg        => Nam "=" (Exp | Call) ";"
// Exp        => NamNum | NamNum Bop NamNum
// NamNum     => Nam | Num
// Nam        => Alpha AlphaNum*
// Num        => [0-9]+
// Bop        => "+" | "-" | "*" | "<" | "<=" | "!=" | "==" | ">=" | ">"
//
// Generate code to copy the value in R0 to the variable called 'varnam'.
// eg:  STR R0, [FP, #@varnam]
// ============================================================================
void cgAsg(Cg* cg, char* funnam, char* varnam) {
  char line[LINESIZE];

  int varidx = layFindVarParIdx(cg->lay, funnam, varnam);
  assert (varidx != 0);

  int varoff = cg->lay->row[varidx].off;

  sprintf(line, "\t STR \t R0, [FP, #%d]", varoff);
  emitCode(cg->emit, line);
}

// ============================================================================
// Block => "{" Stm+ "}"
// ============================================================================
void cgBlock(Cg* cg, char* funnam, AstBlock* astblock) {
  AstStm* aststm = astblock->stms;
  while(aststm) {
    cgStm(cg, funnam, aststm);
    aststm = (AstStm*)aststm->next;
  }
}

// ============================================================================
// Body => "{" Var* Stm+ "}"
// ============================================================================
void cgBody(Cg* cg, char* funnam, AstBody* astbody) {
  cgStms(cg, funnam, astbody->stms);
}

// ============================================================================
// Generate code for the operation (bop) connecting R0 and R1.  If 'bop' is an
// arithmetic operator (+ - * /) then the answer is generated into R0.
// For example, "a + b" generates:
//
//    LDR  R0, [FP, #@a]
//    LDR  R1, [FP, #@b]
//    ADD  R0, R0, R1
//
// If 'bop' is a comparison operator (< <= == != >= >), R0 will be left
// holding 1 for TRUE and 0 for FALSE.  For example, "a < 5" generates:
//
//      LDR  R0, [FP, #@a]
//      LDR  R1, =5
//      CMP  R0, R1
//      BLT  L20
//      LDR  R0, #0         ; false
//      B    L30
// L20: LDR  R0, #1         ; true
// L30:
//
// ============================================================================
void cgBop(Cg* cg, BOP bop) {
  char line[LINESIZE];

  // First process Arithmetic operators

  if (bop == BOPADD) {
    sprintf(line, "\t ADD \t R0, R0, R1");
    emitCode(cg->emit, line);
    return;
  } else if (bop == BOPMUL) {
    sprintf(line, "\t MUL \t R0, R0, R1");
    emitCode(cg->emit, line);
    return;
  } else if (bop == BOPSUB) {
    sprintf(line, "\t SUB \t R0, R0, R1");
    emitCode(cg->emit, line);
    return;
  }

  // Now process the Boolean operators.

  sprintf(line, "\t CMP \t R0, R1");
  emitCode(cg->emit, line);

  if (bop == BOPLT) {
    cgBranch(cg, "BLT");
  } else if (bop == BOPLE) {
    cgBranch(cg, "BLE");
  } else if (bop == BOPEEQ) {
    cgBranch(cg, "BEQ");
  } else if (bop == BOPNE) {
    cgBranch(cg, "BNE");
  } else if (bop == BOPGE) {
    cgBranch(cg, "BGE");
  } else if (bop == BOPGT) {
    cgBranch(cg, "BGT");
  }

}

// ============================================================================
// Generate a conditional branch.
//
// 'cond' is a conditional branch instruction such as "BNE" or "BGT".
//
// 'bop' is a comparison operator (< <= == != >= >).  Make R0 hold 1 for
// TRUE and 0 for FALSE.  For example, "BLE" generates:
//
//        BLE     L10
//        LDR     R0, =0  ; false
//        B       L20
//  L10:  LDR     R0, =1  ; true
//  L20:
// ============================================================================
void cgBranch(Cg* cg, char* cond) {
  char line[LINESIZE];

  char* truelabel = cgLabel();
  sprintf(line, "\t %s \t %s", cond, truelabel);      // eg: L10
  emitCode(cg->emit, line);

  sprintf(line, "\t %s \t %s", "LDR", "R0, =0");      // FALSE
  emitCode(cg->emit, line);

  char* exitlabel = cgLabel();                        // eg: L20
  sprintf(line, "\t %s \t %s", "B", exitlabel);
  emitCode(cg->emit, line);

  sprintf(line, "%s:", truelabel);                    // eg: L10:
  emitCode(cg->emit, line);

  sprintf(line, "\t %s \t %s", "LDR", "R0, =1");      // TRUE
  emitCode(cg->emit, line);

  sprintf(line, "%s:", exitlabel);                    // eg: L20:
  emitCode(cg->emit, line);
}

// ============================================================================
// Call => Nam "(" Args ")"
//
// Emit code to push arguments (right to left), and to BL to the target
// function.  For example:
//
//    int add2(int a, int b) { ... }
//    int main() { int mx; int my; int ms; ... ms = add2(mx, my); ... }
//
//    caller  =  "main" = caller function name
//    callee  =  "add2" = callee function name
//    argname =  "my"   = name of argument #2 in caller function 'main'
//            => "mx"   = name of argument #1 in caller function 'main'
//    numarg  =  2      = number of arguments to push (mx and my)
//    argnum  =  2      = argument number for "my"
//            => 1      = argument number for "mx"
//
// Needless to say, the number of arguments supplied by "main" in the
// call to "add2" must match the number of parameters in the definition
// of function "add2".  This version of the SubC compiler does NOT include
// any check for this.
//
// "funnam" is the name of the current function - the one that emits the BSR.
//
// For the call ms = add2(mx, my), cgAsg will generate the ARM code:
//
//   MOV  R0, [FP, #@my]  ; load my
//   PUSH {R0}
//   MOV  R0 [FP, #@mx]   ; load mx
//   PUSH {R0}
//   BX   add2            ; call add2
//
// In the above code, "@mx" represents the offset, in bytes, of argument "mx"
// from its Frame Pointer
// ============================================================================
void cgCall(Cg* cg, char* funnam, AstCall* astcall) {
  char line[LINESIZE];
  Lay* lay = cg->lay;                                     // alias

  char* callee = astcall->nam->lex;                       // eg: "add2"

  int numarg = astCountArgs(astcall->args);               // eg: 2

  for (int argnum = numarg; argnum >= 1; --argnum) {
    AstArg* astarg = astFindArg(astcall->args, argnum);
    assert(astarg);

    // What kind of argument is this?  Nam, Num or Str?

    if (astarg->nns->kind == ASTNAM) {                      // var|par
      AstNam* astnam = (AstNam*) astarg->nns;
      char* lex = astnam->lex;                              // eg: "my"

      int argidx = layFindVarParIdx(lay, funnam, lex);      // eg: "main", "my"
      assert (argidx != 0);

      int argoff = lay->row[argidx].off;

      sprintf(line, "\t LDR \t R0, \t [FP, #%d]", argoff);  // eg: LDR R0, [FP, #12]
      emitCode(cg->emit, line);

      sprintf(line, "\t PUSH \t {R0}");                     // eg PUSH {R0}
      emitCode(cg->emit, line);
    } else if (astarg->nns->kind == ASTNUM) {               // literal number
      AstNum* num = (AstNum*) astarg->nns;
      int val = num->val;                                   // eg: 42
      sprintf(line, "\t MOV \t R0, #%d", val);              // eg: LDR R0, #42
      emitCode(cg->emit, line);
      sprintf(line, "\t PUSH \t {R0}");                     // PUSH {R0}
      emitCode(cg->emit, line);
    } else if (astarg->nns->kind == ASTSTR) {               // literal string
      char* datalabel = cgLabel();
      sprintf(line, "%s:", datalabel);                      // eg: L50:
      emitData(cg->emit, line);

      AstStr* str = (AstStr*) astarg->nns;
      char*   txt = str->txt;
      sprintf(line, "\t .ASCIZ \t \"%s\" ", txt);           // eg: .ASCIZ "hello"
      emitData(cg->emit, line);

      sprintf(line, "\t LDR \t R0, =%s", datalabel);
      emitCode(cg->emit, line);

      sprintf(line, "\t PUSH \t {R0}");
      emitCode(cg->emit, line);
    }
  }

  sprintf(line, "\t BL \t %s", callee);                 // eg: "BL add2"
  emitCode(cg->emit, line);

  // Remember to remove the arguments previously pushed onto the stack.
  // Because each stack slot in ARM is a WORD, the number of bytes
  // (numb) to remove is simply 4 * numarg

  int numb = 4 * numarg;

  sprintf(line, "\t ADD \t SP, SP, #%d", numb);
  emitCode(cg->emit, line);

}

// ============================================================================
// Generate the Epilog for the function called 'funnam'
// ============================================================================
void cgEpilog(Cg* cg, char* funnam) {

  Emit* emit = cg->emit;                // alias

  char line[LINESIZE];

  // Remove the stack space previously reserved for local variables

  sprintf(line, "\t MOV \t SP, FP");
  emitCode(emit, line);

  // Pop FP and LR from the stack

  sprintf(line, "\t POP \t {FP, LR}");
  emitCode(emit, line);

  // Return to caller

  sprintf(line, "\t BX \t LR");
  emitCode(emit, line);

}

// ============================================================================
// Exp => NamNum | NamNum Bop NamNum
//
// Suppose Exp = a - 7.  Then cgExp will generate the following arm code:
//
//   MOV R0, [FP, #@a]
//   LDR R1, =7
//   SUB R0, R0, R1
//
// 'funnam' is the name of the function in which this expression occurs.
// ============================================================================
void cgExp(Cg* cg, char* funnam, AstExp* astexp) {

  if (astexp->lhs == NULL) return;

  if (astexp->lhs->kind == ASTNAM) {
    AstNam* astnam = (AstNam*) (astexp->lhs);
    cgNam(cg, funnam, astnam, "R0");
  } else if (astexp->lhs->kind == ASTNUM) {
    AstNum* astnum = (AstNum*) (astexp->lhs);
    cgNum(cg, astnum, "R0");
  }

  if (astexp->rhs == NULL) return;

  if (astexp->rhs->kind == ASTNAM) {
    AstNam* astnam = (AstNam*) (astexp->rhs);
    cgNam(cg, funnam, astnam, "R1");
  } else if (astexp->rhs->kind == ASTNUM) {
    AstNum* astnum = (AstNum*) (astexp->rhs);
    cgNum(cg, astnum, "R1");
  }

  cgBop(cg, astexp->bop);

}

// ============================================================================
// Fun => "int"   Nam     "(" Pars ")" Body
//      | "int"   "main"  "("      ")" Body
//
// Note that we need to devise the frame Layout in order to know where to find
// each Argument and local Variable in the Stack Frame.
// ============================================================================
void cgFun(Cg* cg, AstFun* astfun) {
  layBuild(cg->lay, astfun);                // build layout (par/var offsets)
  char* funnam = astfun->nam->lex;          // name of current function

  // Emit the label that marks the start location of this function.  For
  // example, if 'funnam' = "add2" then emit the line: "add2: "

  char line[LINESIZE];
  sprintf(line, "%s:", funnam);
  emitCode(cg->emit, line);

  // Emit the Prolog code

  cgProlog(cg, funnam);

  // Now generate code for the body of the function

  cgBody(cg, funnam, astfun->body);         // generate code for body

}

// ============================================================================
// If => "if" "(" Exp ")" Block
// ============================================================================
void cgIf(Cg* cg, char* funnam, AstIf* astif) {
  
  // Note that cgExp returns its answer in R0: 0 for FALSE, 1 for TRUE
  char line[LINESIZE];

  char* exitlabel = cgLabel();  // Generate a label for the exit of the if block

  // Evaluate the expression inside the if statement
  cgExp(cg, funnam, astif->exp); 

  // Check the result of the evaluation (in R0)
  sprintf(line, "\t CMP \t R0, #0"); // Check if the condition is false
  emitCode(cg->emit, line);

  // If the condition is false, jump to the exit label
  sprintf(line, "\t BEQ \t %s", exitlabel);
  emitCode(cg->emit, line);

  // Generate code for the if block
  cgBlock(cg, funnam, astif->block);

  // Label for the exit of the if block
  sprintf(line, "%s:", exitlabel);
  emitCode(cg->emit, line);
}

// ============================================================================
// Generate a fresh label.  The sequence generated is L10, L20, L30, etc
// ============================================================================
char* cgLabel() {
  #define LABELINC 10;
  static int labnum = 10;

  char* line = calloc(LINESIZE, 1);
  assert(line);

  labnum += LABELINC;
  sprintf(line, "L%d", labnum);
  return line;
}

// ============================================================================
// Nam => Alpha AlphaNum*
//
// Suppose astnam->nam = "x" and reg = "R1".  Then lookup the offset, from FP,
// of parameter or local variable "x".  If found with offset = 12, for example,
// then emit code: "LDR R1, [FP, #12]
// ============================================================================
void cgNam(Cg* cg, char* funnam, AstNam* astnam, char* reg) {

  char line[LINESIZE];

  int idx = layFindVarParIdx(cg->lay, funnam, astnam->lex);

  int off = cg->lay->row[idx].off;
  if (off == 0) utDie5Str("cgNam", "cgFind failed, looking for symbol",
    astnam->lex, "in function", funnam);

  sprintf(line, "\t LDR \t %s, [FP, #%d]", reg, off);
  emitCode(cg->emit, line);
}

// ============================================================================
// Build a new Cg (CodeGen) struct
// ============================================================================
Cg* cgNew() {
  Cg* cg = calloc(sizeof(Cg), 1);
  assert(cg);

  cg->lay = layNew(LAYMAX);
  cg->emit = emitNew();

  return cg;
}

// ============================================================================
// Num => [0-9]+
//
// Suppose astnum->num = 42, and reg = "R1".  Then emit: "LDR R1, =42"
// ============================================================================
void cgNum(Cg* cg, AstNum* astnum, char* reg) {
  char line[LINESIZE];
  sprintf(line, "\t LDR \t %s, =%d", reg, astnum->val);
  emitCode(cg->emit, line);
}

// ============================================================================
// Prog => Fun+
// ============================================================================
void cgProg(Cg* cg, AstProg* astprog) {
  AstFun* astfun = astprog->funs;

  // Pre-populate the Layout with intrinsics says, sayn and sayl

  layBuildIntrinsics(cg->lay);

  // Generate code for each function we encounter (in lexical order)
  // in the SubC source file

  while (astfun) {
    cgFun(cg, astfun);
    astfun = (AstFun*) (astfun->next);
  }
}

// ============================================================================
// Emit Prolog code for a function.  For example:
//
//    int add2(int a, int b) { ... }
//    int main() { int mx; int my; int ms; ... ms = add2(mx, my); ... }
//
// 'funnam' is the name of the current function
// ============================================================================
void cgProlog(Cg* cg, char* funnam) {
 
  Emit* emit = cg->emit; // Alias for emitting code
  char line[LINESIZE];   // Line buffer for code generation

  // Save the Frame Pointer (FP) and Link Register (LR) to the stack
  sprintf(line, "\t PUSH \t {FP, LR}");
  emitCode(emit, line);

  // Set up the new Frame Pointer (FP) to point to the current Stack Pointer (SP)
  sprintf(line, "\t MOV \t FP, SP");
  emitCode(emit, line);

  // Allocate space for local variables on the stack
  int localVarSpace = 32; 
  sprintf(line, "\t SUB \t SP, SP, #%d", localVarSpace);
  emitCode(emit, line);

  // Emit the label for the start of the function
  sprintf(line, "%s:", funnam); 
  emitCode(emit, line);
}

// ============================================================================
// Stm => If | Asg | Ret | While
// ============================================================================
void cgStm(Cg* cg, char* funnam, AstStm* aststm) {
  switch(aststm->kind) {
    case ASTIF:     { AstIf* astif = (AstIf*) aststm;
                      cgIf(cg, funnam, astif);
                      break;
                    }
    case ASTASG:    { AstAsg* astasg = (AstAsg*) aststm;
                      if (astasg->eoc->kind == ASTCALL) {             // Call
                        AstCall* astcall = (AstCall*) astasg->eoc;
                        cgCall(cg, funnam, astcall);
                      } else {                                        // Exp
                        AstExp* astexp = (AstExp*) astasg->eoc;
                        cgExp(cg, funnam, astexp);
                      }
                      cgAsg(cg, funnam, astasg->nam->lex);
                      break;
                    }
    case ASTRET:    { AstRet* astret = (AstRet*) aststm;
                      cgExp(cg, funnam, astret->exp);
                      cgEpilog(cg, funnam);
                      break;
                    }
    case ASTWHILE:  { AstWhile* astwhile = (AstWhile*) aststm;
                      cgWhile(cg, funnam, astwhile);
                      break;
                    }
    default:        { utDie2Str("cgStm", "Invalid aststm->kind"); }
  }
}

// ============================================================================
// Stms = Stm+
// ============================================================================
void cgStms(Cg* cg, char* funnam, AstStm* aststm) {
  while (aststm) {
    cgStm(cg, funnam, aststm);
    aststm = (AstStm*) aststm->next;
  }
}

// ============================================================================
// While => "while" "(" Exp ")" Block
// ============================================================================
void cgWhile (Cg* cg, char* funnam, AstWhile* astwhile) {
  char line[LINESIZE];

  char* startlabel = cgLabel();                     // eg: "L20"
  sprintf(line, "%s:", startlabel);                 // eg: "L20:"
  emitCode(cg->emit, line);

  char* exitlabel = cgLabel();                      // eg: "L30"

  cgExp(cg, funnam, astwhile->exp);                 // result in R0

  sprintf(line, "\t CMP \t R0, #0");                // is test false?
  emitCode(cg->emit, line);

  sprintf(line, "\t BEQ \t %s", exitlabel);         // eg: BEQ L30
  emitCode(cg->emit, line);

  cgBlock(cg, funnam, astwhile->block);

  sprintf(line, "\t B \t %s", startlabel);          // eg: B L20
  emitCode(cg->emit, line);

  sprintf(line, "%s:", exitlabel);                  // eg: L30:
  emitCode(cg->emit, line);
}
