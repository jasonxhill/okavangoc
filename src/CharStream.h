//=============================================================================
// CharStream.h
//=============================================================================
#ifndef SRC_CHARSTREAM_H_
#define SRC_CHARSTREAM_H_
#include "Util.h"
#include <stdio.h>
//-----------------------------------------------------------------------------
#define END_STREAM 0
#define NO_CHAR 0
#define NEXT(STREAM) STREAM->next(STREAM)
//-----------------------------------------------------------------------------
STRUCT CharStream {
  CHAR (*next)(struct CharStream*);
} CharStream;
//-----------------------------------------------------------------------------
STRUCT {
  CharStream stream;
  const CHAR* nextChar;
} InMemoryCharStream;
//-----------------------------------------------------------------------------
InMemoryCharStream newInMemoryCharStream(const string);
#define FILE_CHAR_STREAM_BUFFER_SIZE 1024
//-----------------------------------------------------------------------------
STRUCT FileCharStream {
  CharStream stream;
  FILE* file;
  void (*close)(struct FileCharStream*);
  unsigned int index;
  CHAR buffer[FILE_CHAR_STREAM_BUFFER_SIZE];
} FileCharStream;
//-----------------------------------------------------------------------------
FileCharStream newFileCharStream(const string filePath);
//-----------------------------------------------------------------------------
#endif // SRC_CHARSTREAM_H_
//-----------------------------------------------------------------------------