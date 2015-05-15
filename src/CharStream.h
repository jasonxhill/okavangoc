#ifndef SRC_CHARSTREAM_H_
#define SRC_CHARSTREAM_H_
#include "Util.h"

#define END_STREAM 0
#define NEXT(STREAM) INVOKE(STREAM, next)

STRUCT CharStream {
  character (* const next)(struct CharStream*);
} CharStream;

STRUCT {
  CharStream stream;
  const character* chars;
} InMemoryCharStream;

InMemoryCharStream getInMemoryCharStream(const character* chars);

#endif /* SRC_CHARSTREAM_H_ */
