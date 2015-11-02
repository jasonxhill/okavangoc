//=============================================================================
// SyntaxTreeTest.c
//=============================================================================
#include "TestUtil.h"
#include "../src/SyntaxTree.h"
//-----------------------------------------------------------------------------
static void testSyntaxTree();
//-----------------------------------------------------------------------------
void mainSyntaxTreeTests()
{
  TEST(testSyntaxTree);
}
//-----------------------------------------------------------------------------
static string bracketToString(MemoryPool*, Bracket*);
static string statementToString(MemoryPool*, Statement*);
static string statementComponentTypeToString(StatementComponentType);
//-----------------------------------------------------------------------------
static void testSyntaxTree()
{
  MemoryPool pool = newMemoryPool();

  string code =
    "statement A;\n"
    "{\n"
    "  statement B;\n"
    "  statement C[1];\n"
    "  statement D(r,x) + 20;\n"
    "};\n";

  InMemoryCharStream charStream = newInMemoryCharStream(code);
  SyntaxTree tree = newSyntaxTree(&pool, &charStream.stream);

  string result = bracketToString(&pool, tree.root);
  result = joinStrings(&pool, "\n", result);
  result = joinStrings(&pool, result, "\n");

  assertEquals(string,
    "\n"
    "START_BRACKET(fileBracket)\n"
    "START_STATEMENT\n"
    "statement A\n"
    "END_STATEMENT\n"
    "START_STATEMENT\n"
    "\n\n"
    "START_BRACKET(curlyBrace)\n"
    "START_STATEMENT\n"
    "\n  statement B\n"
    "END_STATEMENT\n"
    "START_STATEMENT\n"
    "\n  statement C\n"
    "START_BRACKET(squareBracket)\n"
    "START_STATEMENT\n"
    "1\n"
    "END_STATEMENT\n"
    "END_BRACKET(squareBracket)\n"
    "END_STATEMENT\n"
    "START_STATEMENT\n"
    "\n  statement D\n"
    "START_BRACKET(parentheses)\n"
    "START_STATEMENT\n"
    "r,x\n"
    "END_STATEMENT\n"
    "END_BRACKET(parentheses)\n"
     " + 20\n"
    "END_STATEMENT\n"
    "START_STATEMENT\n"
    "\n\n"
    "END_STATEMENT\n"
    "END_BRACKET(curlyBrace)\n"
    "END_STATEMENT\n"
    "START_STATEMENT\n"
    "\n\n"
    "END_STATEMENT\n"
    "END_BRACKET(fileBracket)\n",
    result);

  pool.drain(&pool);
}
//-----------------------------------------------------------------------------
static string bracketToString(MemoryPool* const pool, Bracket* const bracket)
{
  string str = "START_BRACKET(";
  str = joinStrings(pool, str, statementComponentTypeToString(bracket->statementComponent.type));
  str = joinStrings(pool, str, ")\n");

  for(Statement** s = bracket->statements; *s != NULL; s++)
    str = joinStrings(pool, str, statementToString(pool, *s));

  str = joinStrings(pool, str, "END_BRACKET(");
  str = joinStrings(pool, str, statementComponentTypeToString(bracket->statementComponent.type));
  str = joinStrings(pool, str, ")");
  return str;
}
//-----------------------------------------------------------------------------
static string statementToString(MemoryPool* const pool, Statement* const statement)
{
  string str = "";
  str = joinStrings(pool, str, "START_STATEMENT\n");

  for(StatementComponent** s = statement->components; *s != NULL; s++)
  {
    string component = (*s)->type == token? ((Token*) *s)->value : bracketToString(pool, (Bracket*) *s);
    str = joinStrings(pool, str, component);
    str = joinStrings(pool, str, "\n");
  }

  str = joinStrings(pool, str, "END_STATEMENT\n");
  return str;
}
//-----------------------------------------------------------------------------
static string statementComponentTypeToString(StatementComponentType type)
{
  switch(type)
  {
    case curlyBrace:
      return "curlyBrace";
    case squareBracket:
      return "squareBracket";
    case parentheses:
      return "parentheses";
    case fileBracket:
      return "fileBracket";
    case token:
      return "token";
    case unknown:
      return "unknown";
    default:
      return "undefined";
  }
}
//-----------------------------------------------------------------------------