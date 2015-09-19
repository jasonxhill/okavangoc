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
  AutoReleasePool* autoReleasePool;
  CharStream* charStream;
  CHAR lastChar;
} TokenStream;
//-----------------------------------------------------------------------------
TokenStream newTokenStream(AutoReleasePool*, CharStream*);
//-----------------------------------------------------------------------------
#endif // SRC_TOKENSTREAM_H_
//-----------------------------------------------------------------------------