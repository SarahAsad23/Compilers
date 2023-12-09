// main.c - SubC Compiler - Jim Hogg, 2020
//
// Prog       => Fun+
// Fun        => "int"   Nam     "(" Pars ")" Body
//             | "int"   "main"  "("      ")" Body
// Pars       => ( Par ( "," Par )* ) ?
// Par        => "int" Nam
//
// Body       => "{" Var* Stm+ "}"
// Var        => "int" Nam ";"
// Stm        => If | Asg | Ret | While
// If         => "if" "(" Exp ")" Block
// Asg        => Nam "=" (Exp | Call) ";"
// Ret        => "return" Exp ";"
// While      => "while" "(" Exp ")" Block
// Block      => "{" Stm+ "}"
//
// NamNum     => Nam | Num
// Exp        => NamNum | NamNum Bop NamNum
// Bop        => "+" | "-" | "*" | "<" | "<=" | "!=" | "==" | ">=" | ">"
//
// Call       => Nam "(" Args ")"
// Args       => ( Arg ( "," Arg )* ) ?
// Arg        => Nam | Num | Str
//
// Num        => [0-9]+
// Str        => "\"" NonQuotes "\""
// Nam        => Alpha AlphaNum*
// Alpha      => [a-zA-Z]
// AlphaNum   => [a-zA-Z0-9]

#include "main.h"

void usage() { printf("\n\nUsage: subc <file.subc> \n\n"); }

int main(int argc, char* argv[]) {
  if (argc < 2) { usage(); exit(-1); }

  char* prog = utReadFile(argv[1]);       // raw chars

  Lex* lex = lexNew(prog);
  Toks* toks = lexAll(lex);
  toksDump(toks);                         // DEBUG: dump Tokens to TokenDump.txt
  toksRewind(toks);
  AstProg* astProg = pseProg(toks);       // parse tokens, build AST
  visitProg(astProg);                     // DEBUG : walk every node in AST

  utPause();
  return 0;
}