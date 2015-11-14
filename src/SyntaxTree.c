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
  AbstractBracketComponent* component;
  Bracket* bracket;
  Statement* statement;
  StringBuffer token;
  StreamPosition tokenStart;
  StreamPosition tokenEnd;
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

  firstStackFrame.list = newLinkedList(&firstStackFrame.pool);
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

  if(sc.c == ';')
  {
    stackFrame->statement = visitor->pool->calloc(visitor->pool, 1, sizeof(Statement));
    stackFrame->component = &stackFrame->statement->super;
    stackFrame->statement->parent = stackFrame->parent->bracket;
  }
  else
  {
    stackFrame->bracket = visitor->pool->calloc(visitor->pool, 1, sizeof(Bracket));
    stackFrame->component = &stackFrame->bracket->super.super;
    stackFrame->bracket->super.parent = stackFrame->parent->statement;

    if(strcmp("", stackFrame->parent->token.str) != 0)
    {
      stackFrame->parent->list.append(&stackFrame->parent->list, newToken(visitor, stackFrame->parent));
      stackFrame->parent->token = newStringBuffer(&stackFrame->parent->pool, "");
      stackFrame->parent->tokenStart.line = 0;
    }
  }

  stackFrame->component->type = sc.type;
  stackFrame->parent->list.append(&stackFrame->parent->list, stackFrame->component);
  stackFrame->component->startPosition = sc.position;
  stackFrame->list = newLinkedList(&stackFrame->pool);
  stackFrame->token = newStringBuffer(&stackFrame->pool, "");
}
//-----------------------------------------------------------------------------
static void visitBracketEnd(SyntaxTreeVisitor* const visitor, const StreamChar sc)
{
  SyntaxTreeStackFrame* const stackFrame = visitor->stackFrame;
  visitor->stackFrame = visitor->stackFrame->parent;
  stackFrame->component->endPosition = sc.position;

  if(stackFrame->bracket != NULL)
  {
    if(strcmp("", stackFrame->token.str) != 0)
      stackFrame->bracket->super.value = stackFrame->token.toString(&stackFrame->token, visitor->pool);

    stackFrame->bracket->statements = (Statement**) stackFrame->list.toArray(&stackFrame->list, visitor->pool);
  }
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
  visitor->stackFrame->component->endTokenMissing = TRUE;
  visitBracketEnd(visitor, type);
}
//-----------------------------------------------------------------------------
static void visitChar(SyntaxTreeVisitor* const visitor, const StreamChar sc)
{
  visitor->stackFrame->token.appendChar(&visitor->stackFrame->token, sc.c);
  visitor->stackFrame->tokenEnd = sc.position;

  if(visitor->stackFrame->tokenStart.line == 0)
    visitor->stackFrame->tokenStart = sc.position;
}
//-----------------------------------------------------------------------------
static Token* newToken(SyntaxTreeVisitor* const visitor, SyntaxTreeStackFrame* const stackFrame)
{
  Token* const t = visitor->pool->calloc(visitor->pool, 1, sizeof(Token));
  t->super.super.type = token;
  t->super.parent = stackFrame->parent->statement;
  t->super.value = stackFrame->token.toString(&stackFrame->token, visitor->pool);
  t->super.super.startPosition = stackFrame->tokenStart;
  t->super.super.endPosition = stackFrame->tokenEnd;
  return t;
}
//-----------------------------------------------------------------------------