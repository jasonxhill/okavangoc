//=============================================================================
// SyntaxTree.c
//=============================================================================
#include "SyntaxTree.h"
#include <stdio.h>
#include "BracketStream.h"
#include "LinkedList.h"
#include "StringBuffer.h"
//-----------------------------------------------------------------------------
STRUCT SyntaxTreeStackFrame {
  MemoryPool pool;
  LinkedList list;
  struct SyntaxTreeStackFrame* parent;
  Bracket* bracket;
  Statement* statement;
  StringBuffer token;
} SyntaxTreeStackFrame;
//-----------------------------------------------------------------------------
STRUCT {
  BracketVisitor bracketVisitor;
  MemoryPool* pool;
  SyntaxTreeStackFrame* stackFrame;
} SyntaxTreeVisitor;
//-----------------------------------------------------------------------------
static void visitBracketStart(SyntaxTreeVisitor*, StreamChar type);
static void visitBracketEnd(SyntaxTreeVisitor*, StreamChar type);
static void visitBracketEndMissing(SyntaxTreeVisitor*, StreamChar type);
static void visitChar(SyntaxTreeVisitor*, StreamChar c);
static Token* newToken(SyntaxTreeVisitor*, SyntaxTreeStackFrame*);
//-----------------------------------------------------------------------------
SyntaxTree newSyntaxTree(MemoryPool* const treePool, CharStream* const charStream)
{
  SyntaxTreeStackFrame firstStackFrame = {
    .pool = newMemoryPool()
  };

  firstStackFrame.list = newLinkedList(&firstStackFrame.pool),
      firstStackFrame.token = newStringBuffer(&firstStackFrame.pool, "");

  SyntaxTreeVisitor visitor = {
    .bracketVisitor = {
      .visitBracketStart = (BracketVisitStart) &visitBracketStart,
      .visitBracketEnd = (BracketVisitEnd) &visitBracketEnd,
      .visitBracketEndMissing = (BracketVisitEndMissing) &visitBracketEndMissing,
      .visitChar = (BracketVisitChar) &visitChar
    },

    .pool = treePool,
    .stackFrame = &firstStackFrame
  };

  BracketStream bracketStream = newBracketStream(charStream);
  bracketStream.visit(&bracketStream, &visitor.bracketVisitor);

  SyntaxTree tree = {.root = firstStackFrame.list.head(&firstStackFrame.list)};
  firstStackFrame.pool.drain(&firstStackFrame.pool);
  return tree;
}
//-----------------------------------------------------------------------------
static void visitBracketStart(SyntaxTreeVisitor* const visitor, const StreamChar sc)
{
  SyntaxTreeStackFrame* stackFrame;
  {
    MemoryPool pool = newMemoryPool();
    stackFrame = pool.calloc(&pool, 1, sizeof(SyntaxTreeStackFrame));
    stackFrame->pool = pool;
  }

  stackFrame->parent = visitor->stackFrame;
  visitor->stackFrame = stackFrame;

  stackFrame->list = newLinkedList(&stackFrame->pool);
  stackFrame->token = newStringBuffer(&stackFrame->pool, "");

  if(sc.c == ';')
  {
    stackFrame->statement = visitor->pool->calloc(visitor->pool, 1, sizeof(Statement));
    stackFrame->statement->parent = stackFrame->parent->bracket;
    stackFrame->parent->list.append(&stackFrame->parent->list, stackFrame->statement);
    return;
  }

  if(strcmp("", stackFrame->parent->token.str) != 0)
  {
    stackFrame->parent->list.append(&stackFrame->parent->list, newToken(visitor, stackFrame->parent));
    stackFrame->parent->token = newStringBuffer(&stackFrame->parent->pool, "");
  }

  stackFrame->bracket = visitor->pool->calloc(visitor->pool, 1, sizeof(Bracket));
  stackFrame->bracket->statementComponent.parent = stackFrame->parent->statement;
  stackFrame->parent->list.append(&stackFrame->parent->list, stackFrame->bracket);
  stackFrame->bracket->statementComponent.type = sc.type;
}
//-----------------------------------------------------------------------------
static void visitBracketEnd(SyntaxTreeVisitor* const visitor, const StreamChar type)
{
  SyntaxTreeStackFrame* const stackFrame = visitor->stackFrame;
  visitor->stackFrame = visitor->stackFrame->parent;

  if(stackFrame->bracket != NULL)
    stackFrame->bracket->statements = (Statement**) stackFrame->list.toArray(&stackFrame->list, visitor->pool);
  else if(stackFrame->statement != NULL)
  {
    if(strcmp("", stackFrame->token.str) != 0)
      stackFrame->list.append(&stackFrame->list, newToken(visitor, stackFrame));

    stackFrame->statement->components = (StatementComponent**) stackFrame->list.toArray(&stackFrame->list, visitor->pool);
  }

  stackFrame->pool.drain(&stackFrame->pool);
}
//-----------------------------------------------------------------------------
static void visitBracketEndMissing(SyntaxTreeVisitor* const visitor, const StreamChar type)
{
  visitBracketEnd(visitor, type);
}
//-----------------------------------------------------------------------------
static void visitChar(SyntaxTreeVisitor* const visitor, const StreamChar streamChar)
{
  visitor->stackFrame->token.appendChar(&visitor->stackFrame->token, streamChar.c);
}
//-----------------------------------------------------------------------------
static Token* newToken(SyntaxTreeVisitor* const visitor, SyntaxTreeStackFrame* const stackFrame)
{
  Token* const t = visitor->pool->calloc(visitor->pool, 1, sizeof(Token));
  t->statementComponent.type = token;
  t->statementComponent.parent = stackFrame->parent->statement;
  t->value = stackFrame->token.toString(&stackFrame->token, visitor->pool);
  return t;
}
//-----------------------------------------------------------------------------