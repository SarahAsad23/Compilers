// tok.c - functions to handle tokens - Jim Hogg, 2020

#include "tok.h"

Tok* tokNew(int kind, char* lex, int num, char* txt, int linNum, int colNum) {
  Tok* tok = (Tok*) malloc(sizeof(Tok));
  tok->kind   = kind;
  tok->lex    = lex;
  tok->num    = num;
  tok->str    = txt;
  tok->linNum = linNum;
  tok->colNum = colNum;
  return tok;
}

char* tokStr(TokKind kind) {
  switch(kind) {
    case TOKADD:      return "TOKADD";
    case TOKBAD:      return "TOKBAD";
    case TOKCOMMA:    return "TOKCOMMA";
    case TOKEEQ:      return "TOKEEQ";
    case TOKEOF:      return "TOKEOF";
    case TOKEQ:       return "TOKEQ";
    case TOKGE:       return "TOKGE";
    case TOKGT:       return "TOKGT";
    case TOKIF:       return "TOKIF";
    case TOKINT:      return "TOKINT";
    case TOKLBRACE:   return "TOKLBRACE";
    case TOKLE:       return "TOKLE";
    case TOKLPAREN:   return "TOKLPAREN";
    case TOKLT:       return "TOKLT";
    case TOKMUL:      return "TOKMUL";
    case TOKNAM:      return "TOKNAM";
    case TOKNUM:      return "TOKNUM";
    case TOKNE:       return "TOKNE";
    case TOKRBRACE:   return "TOKRBRACE";
    case TOKRET:      return "TOKRET";
    case TOKRPAREN:   return "TOKRPAREN";
    case TOKSEMI:     return "TOKSEMI";
    case TOKSTR:      return "TOKSTR";
    case TOKSUB:      return "TOKSUB";
    case TOKWHILE:    return "TOKWHILE";
    default:          return "TOKBAD";
  }
}