#ifndef SRC_TOKENSTREAM_H_
#define SRC_TOKENSTREAM_H_

#include "CharStream.h"
#include "String.h"

STRUCT TokenStream {
  String (*const next)(struct TokenStream*);
  CharStream* const charStream;
  character lastChar;
} TokenStream;

TokenStream getTokenStream(CharStream* charStream);

#endif /* SRC_TOKENSTREAM_H_ */
