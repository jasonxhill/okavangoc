//=============================================================================
// TokenStream.h
//=============================================================================
#ifndef SRC_TOKENSTREAM_H_
#define SRC_TOKENSTREAM_H_

#include "CharStream.h"
#include "String.h"
//-----------------------------------------------------------------------------
STRUCT TokenStream {
  String (*const next)(AutoReleasePool*, struct TokenStream*);
  CharStream* const charStream;
  CHAR lastChar;
} TokenStream;
//-----------------------------------------------------------------------------
TokenStream newTokenStream(CharStream* charStream);
//-----------------------------------------------------------------------------
#endif // SRC_TOKENSTREAM_H_
//-----------------------------------------------------------------------------