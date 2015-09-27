//=============================================================================
// CharStream.c
//=============================================================================
#include "CharStream.h"
#include <stdio.h>
//-----------------------------------------------------------------------------

//=============================================================================
// InMemoryCharStream
//=============================================================================
static CHAR nextInMemoryChar(CharStream* const stream);
//-----------------------------------------------------------------------------
InMemoryCharStream newInMemoryCharStream(const string str)
{
  InMemoryCharStream stream = {
    .stream = {.next = *nextInMemoryChar},
    .nextChar = str
  };

  return stream;
}
//-----------------------------------------------------------------------------
static CHAR nextInMemoryChar(CharStream* const stream)
{
  InMemoryCharStream* const istream = (InMemoryCharStream*) stream;
  const CHAR c = istream->nextChar[0];

  if(c == END_STREAM)
    return END_STREAM;

  istream->nextChar++;
  return c;
}
//-----------------------------------------------------------------------------

//=============================================================================
// InMemoryCharStream
//=============================================================================
static CHAR nextFileChar(CharStream*);
static void closeFile(FileCharStream*);
//-----------------------------------------------------------------------------
FileCharStream newFileCharStream(const string filePath)
{
  FileCharStream stream = {
    .stream = {.next = *nextFileChar},
    .file = fopen(filePath, "r"),
    .close = &closeFile,
    .index = FILE_CHAR_STREAM_BUFFER_SIZE,
    .buffer = {0}
  };

  return stream;
}
//-----------------------------------------------------------------------------
static CHAR nextFileChar(CharStream* const stream)
{
  FileCharStream* const fstream = (FileCharStream*) stream;

  if(fstream->file == NULL)
    return END_STREAM;

  if(fstream->index >= FILE_CHAR_STREAM_BUFFER_SIZE)
  {
    const unsigned int readCount = fread(fstream->buffer, sizeof(char), FILE_CHAR_STREAM_BUFFER_SIZE, fstream->file);
    fstream->index = 0;

    if(readCount < FILE_CHAR_STREAM_BUFFER_SIZE)
      fstream->buffer[readCount] = END_STREAM;
  }

  const CHAR c = fstream->buffer[fstream->index];

  if(c == END_STREAM)
  {
    fstream->close(fstream);
    return END_STREAM;
  }

  fstream->index++;
  return c;
}
//-----------------------------------------------------------------------------
static void closeFile(FileCharStream* const fstream)
{
  if(fstream->file == NULL)
    return;

  fclose(fstream->file);
  fstream->file = NULL;
}
//-----------------------------------------------------------------------------