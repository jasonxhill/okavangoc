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
static string bracketTokenTypeToString(BracketTokenType);
//-----------------------------------------------------------------------------
static void testSyntaxTree()
{
  MemoryPool pool = newMemoryPool();

  string code =
    "statement A;\n"
    "{\n"
    "  // A test comment\n"
    "  statement B;\n"
    "  statement C[1];\n"
    "  /*\n"
    "    multiline comment\n"
    "  */\n"
    "  statement D(r,x) + 20;\n"
    "  \"A test string\";\n"
    "  'A test string';\n"
    "  `A test string`;\n"
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
    "\n  \n"
    "START_BRACKET(singleLineComment)\n"
    "// A test comment\n\n"
    "END_BRACKET(singleLineComment)\n"
    "  statement B\n"
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
    "\n  \n"
    "START_BRACKET(multiLineComment)\n"
    "/*\n"
    "    multiline comment\n"
    "  */\n"
    "END_BRACKET(multiLineComment)\n"
    "\n  statement D\n"
    "START_BRACKET(parentheses)\n"
    "START_STATEMENT\n"
    "r,x\n"
    "END_STATEMENT\n"
    "END_BRACKET(parentheses)\n"
     " + 20\n"
    "END_STATEMENT\n"
    "START_STATEMENT\n"
    "\n  \n"
    "START_BRACKET(doubleQuote)\n"
    "\"A test string\"\n"
    "END_BRACKET(doubleQuote)\n"
    "END_STATEMENT\n"
    "START_STATEMENT\n"
    "\n  \n"
    "START_BRACKET(singleQuote)\n"
    "'A test string'\n"
    "END_BRACKET(singleQuote)\n"
    "END_STATEMENT\n"
    "START_STATEMENT\n"
    "\n  \n"
    "START_BRACKET(backtick)\n"
    "`A test string`\n"
    "END_BRACKET(backtick)\n"
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
  str = joinStrings(pool, str, bracketTokenTypeToString(bracket->statementComponent.type));
  str = joinStrings(pool, str, ")\n");

  for(Statement** s = bracket->statements; *s != NULL; s++)
    str = joinStrings(pool, str, statementToString(pool, *s));

  if(bracket->statementComponent.value != NULL)
  {
    str = joinStrings(pool, str, bracket->statementComponent.value);
    str = joinStrings(pool, str, "\n");
  }

  str = joinStrings(pool, str, "END_BRACKET(");
  str = joinStrings(pool, str, bracketTokenTypeToString(bracket->statementComponent.type));
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
    string component = (*s)->type == token? (*s)->value : bracketToString(pool, (Bracket*) *s);
    str = joinStrings(pool, str, component);
    str = joinStrings(pool, str, "\n");
  }

  str = joinStrings(pool, str, "END_STATEMENT\n");
  return str;
}
//-----------------------------------------------------------------------------
#define CASE(VAR) case VAR: return #VAR;

static string bracketTokenTypeToString(const BracketTokenType type)
{
  switch(type)
  {
    CASE(curlyBrace)
    CASE(squareBracket)
    CASE(parentheses)
    CASE(fileBracket)
    CASE(singleLineComment)
    CASE(multiLineComment)
    CASE(doubleQuote)
    CASE(singleQuote)
    CASE(backtick)
    CASE(token)
    CASE(unknown)
    default: return "undefined";
  }
}
#undef CASE
//-----------------------------------------------------------------------------