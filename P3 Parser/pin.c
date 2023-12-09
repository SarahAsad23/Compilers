// pin.c - Indentation - Jim Hogg, 2020

#include <stdio.h>      // printf
#include "pin.h"        // pin*

int g_indentation = 0;

// ============================================================================
// pin : print 'indentation' spaces
// ============================================================================
void pin() {
  for (int i = 1; i <= g_indentation; ++i) printf(" ");
}

// ============================================================================
// pinLess : reduce g_indentation
// ============================================================================
void pinLess() { g_indentation -= INDENT; }

// ============================================================================
// pinMore : increase g_indentation
// ============================================================================

void pinMore() { g_indentation += INDENT; }
