//=============================================================================
// SyntaxTree.h
//=============================================================================
#ifndef SRC_SYNTAXTREE_H_
#define SRC_SYNTAXTREE_H_
#include "MemoryPool.h"
#include "BracketStream.h"
//-----------------------------------------------------------------------------
struct Statement;
//-----------------------------------------------------------------------------
STRUCT {
  BracketTokenType type;
  BOOL endTokenMissing;
  StreamPosition startPosition;
  StreamPosition endPosition;
} AbstractBracketComponent;
//-----------------------------------------------------------------------------
STRUCT {
  AbstractBracketComponent super;
  struct Statement* parent;
  string value;
} StatementComponent;
//-----------------------------------------------------------------------------
STRUCT {
  StatementComponent super;
  struct Statement** statements;
} Bracket;
//-----------------------------------------------------------------------------
STRUCT Statement {
  AbstractBracketComponent super;
  Bracket* parent;
  StatementComponent** components;
} Statement;
//-----------------------------------------------------------------------------
STRUCT {
  StatementComponent super;
} Token;
//-----------------------------------------------------------------------------
STRUCT {
  Bracket* root;
} SyntaxTree;
//-----------------------------------------------------------------------------
SyntaxTree newSyntaxTree(MemoryPool*, CharStream*);
//-----------------------------------------------------------------------------
#endif // SRC_SYNTAXTREE_H_
//-----------------------------------------------------------------------------