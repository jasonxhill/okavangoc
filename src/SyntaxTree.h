#ifndef SRC_SYNTAXTREE_H_
#define SRC_SYNTAXTREE_H_
#include "TokenStream.h"

typedef string Key;

STRUCT {
  Key key;
} Pair;

STRUCT {
  Pair pair;
  Pair* nextPair;
} PairList;

STRUCT {
  Pair* root;
} SyntaxTree;

SyntaxTree toSyntaxTree(CharStream);

#endif /* SRC_SYNTAXTREE_H_ */
