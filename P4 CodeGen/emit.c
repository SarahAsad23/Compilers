// emit.c - Emit ARM Assembly Code - Jim Hogg, 2022

#include "emit.h"

// ============================================================================
// Emit the text in 'line' into the code section of the emit buffer
// called 'emit'
// ============================================================================
void emitCode(Emit* emit, char* line) {
  int varparoff = emit->codeSize;
  emit->codeSize += sprintf(emit->codeBuf + varparoff, "%s \n", line);
}

// ============================================================================
// Emit a .TEXT directive as the first line of the code emit buffer
// ============================================================================
void emitCodeDirective(Emit* emit) {
  char line[LINESIZE];
  sprintf(line, "\t .TEXT");
  emitCode(emit, line);
}

// ============================================================================
// Emit the text in 'line' into the data section of the emit buffer called 'eb'
// ============================================================================
void emitData(Emit* emit, char* line) {
  int varparoff = emit->dataSize;
  emit->dataSize += sprintf(emit->dataBuf + varparoff, "%s \n", line);
}

// ============================================================================
// Emit a .DATA directive as the first line of the data emit buffer
// ============================================================================
void emitDataDirective(Emit* emit) {
  char line[LINESIZE];
  sprintf(line, "\t .DATA");
  emitData(emit, line);
}

// ============================================================================
// Dump the text (codeBuf and dataBuf) currently held in 'emit'
// ============================================================================
void emitDump(Emit* emit) {
  printf("\n\n");
  printf("%s", emit->codeBuf);
  printf("\n");
  printf("%s", emit->dataBuf);
}

// ============================================================================
// Create a new Emit struct
// ============================================================================
Emit* emitNew() {
  Emit* emit = calloc(sizeof(Emit), 1);
  if (emit == NULL) utDie2Str("cgNew", "calloc failed");
  emit->codeBuf  = calloc(CODESIZE, 1);
  emit->codeSize = 0;

  emit->dataBuf  = calloc(DATASIZE, 1);
  emit->dataSize = 0;

  return emit;
}

// ============================================================================
// Devise the name for the output file to hold the assembler code generated by
// the SubC compiler.  For example, if the source file, obtained from main's
// argv[1], holds:
//
//    c:\Users\jimhh\OneDrive\UW\CSS-448-Hogg-Wi21\Tests\test01.subc"
//
// Then extract the filename "test01" and append the extension ".s", to end
// up with "test01.s" as the name of the output assembly file
// ============================================================================
char* emitNewName(char* sourcePath) {

  char* path = calloc(100, 1);              // buffer for output file name
  if (path == NULL) utDie2Str("emitNewName", "calloc failed");

  char* wack = strrchr(sourcePath, '\\');   // find last wack ("\")

  strcpy(path, wack + 1);                   // eg: "test01.subc"
  char* dot = strrchr(path, '.');           // find last dot (".")
  strncpy(dot + 1, "s\0", 4);               // eg: "test01.s"

  return path;
}

// ============================================================================
// Save the current Emit buffer - data and assembler code, generated by the
// SubC Compiler - to a file on disk, specified by 'filePath'
// ============================================================================
void emitSave(Emit* emit, char* filePath) {
  FILE* file = fopen(filePath, "w");

  if (!file) utDie2Str("emitCreateFile: Cannot create output assembly file: ", filePath);

  // Combine the Code and Data sections of the Emit struct into a contiguous
  // block of memory

  int totalSize = emit->dataSize + emit->codeSize;

  char* totalBuf = calloc(totalSize, 1);
  if (totalBuf == NULL) utDie2Str("emitSave", "calloc failed");

  memcpy(totalBuf, emit->dataBuf, emit->dataSize);

  char* codeStart = totalBuf + emit->dataSize;
  memcpy(codeStart, emit->codeBuf, emit->codeSize);

  // Write 'totalBuf' to disk

  int written = fwrite(totalBuf, 1, totalSize, file);
  if (written != totalSize) utDie2Str("emitSave", "fwrite failed");

  fclose(file);

}
