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
  string value;
  struct Statement* parent;
} StatementComponent;
//-----------------------------------------------------------------------------
STRUCT {
  StatementComponent statementComponent;
  struct Statement** statements;
} Bracket;
//-----------------------------------------------------------------------------
STRUCT Statement {
  Bracket* parent;
  StatementComponent** components;
} Statement;
//-----------------------------------------------------------------------------
STRUCT {
  StatementComponent statementComponent;
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