//=============================================================================
// TokenStream.h
//=============================================================================
#ifndef SRC_TOKENSTREAM_H_
#define SRC_TOKENSTREAM_H_

#include "CharStream.h"
#include "String.h"
//-----------------------------------------------------------------------------
STRUCT TokenStream {
  String (*next)(struct TokenStream*);
  MemoryPool* memoryPool;
  CharStream* charStream;
  CHAR lastChar;
} TokenStream;
//-----------------------------------------------------------------------------
TokenStream newTokenStream(MemoryPool*, CharStream*);
//-----------------------------------------------------------------------------
#endif // SRC_TOKENSTREAM_H_
//-----------------------------------------------------------------------------