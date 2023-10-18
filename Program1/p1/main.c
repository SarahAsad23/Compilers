// main.c - SubC Compiler
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

int main(int argc, char* argv[]) {

  char* prog = utReadFile(argv[1]);       // raw chars

  //make sure prog is not null
  if(prog != NULL){
    int i = 0; 
    //while we have not hit EOF
    while(prog[i] != '\0'){
      char current = prog[i];  //get the current character 
      printf("%d\n", current); //print its decimal 
      printf("%c\n", current); //print its character value 
      i++;                     //increment to the next character 
    }  
  }
  
  Lex*  lex  = lexNew(prog);
  Toks* toks = lexAll(lex);
  toksDump(toks);             // dump Tokens to ToksDump.txt

  utPause();
  return 0;
}