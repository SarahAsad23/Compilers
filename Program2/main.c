// main.c - SubC Compiler - Jim Hogg, 2020
//
// Prog       => Fun+
// Fun        => "int"  Nam    "(" Pars ")" Body
//             | "int"  "main" "("      ")" Body
// Pars       => ( Par ( "," Par )* ) ?
// Par        => "int" Nam
//
// Body       => "{" Var* Stm+ "}"
// Var        => "int" Nam ";"
// Stm        => If | Asg | Ret | While
// If         => "if" "(" Exp ")" Block
// Asg        => Nam "=" AsgExp ";"
// Ret        => "return" Exp ";"
// While      => "while" "(" Exp ")" Block
// Block      => "{" Stm+ "}"
//
// NamNum     => Nam | Num
// Exp        => NamNum | NamNum Bop NamNum
// Bop        => "+" | "-" | "*" | "<" | "<=" | "!=" | "==" | ">=" | ">"
// AsgExp     => Exp | Call
//
// Call       => Nam "(" Args ")"
// Args       => ( Arg ( "," Arg )* ) ?
// Arg        => NamNum | Str
//
// Num        => [0-9]+
// Str        => "\"" NonQuotes "\""
// Nam        => Alpha AlphaNum*
// Alpha      => [a-zA-Z]
// AlphaNum   => [a-zA-Z0-9]
//
// Execution starts with the function "main".
// Type "int" is a 32-bit C int

#include "main.h"

void usage() { printf("\n\nUsage: subc <file.subc> \n\n"); }

int main(int argc, char* argv[]) {
  if (argc < 2) { usage(); exit(-1); }

  char* prog = utReadFile(argv[1]);       // raw chars

  ///for (int i = 0; i < strlen(prog); ++i) {        // DEBUG
  ///  char c = prog[i];                             // DEBUG
  ///  printf("i = %3d  c = %3d   %c \n", i, c, c);  // DEBUG
  ///}                                               // DEBUG

  Lex* lex = lexNew(prog);
  Toks* toks = lexAll(lex);
  toksDump(toks);                         // DEBUG: dump Tokens to TokenDump.txt
  toksRewind(toks);

  rexProg(toks);                          // Recognize

  utPause();
  return 0;
}
