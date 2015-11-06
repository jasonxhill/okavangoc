//=============================================================================
// SyntaxTree.h
//=============================================================================
#ifndef SRC_SYNTAXTREE_H_
#define SRC_SYNTAXTREE_H_
#include "MemoryPool.h"
#include "CharStream.h"
//-----------------------------------------------------------------------------
struct Statement;
//-----------------------------------------------------------------------------
typedef enum StatementComponentType {
  curlyBrace = 0,
  squareBracket = 1,
  parentheses = 2,
  fileBracket,
  singleLineComment,
  multiLineComment,
  token,
  unknown
} StatementComponentType;
//-----------------------------------------------------------------------------
STRUCT {
  StatementComponentType type;
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
  string value;
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