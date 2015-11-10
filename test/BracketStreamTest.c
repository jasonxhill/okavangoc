//=============================================================================
// BracketStreamTest.c
//=============================================================================
#include "TestUtil.h"
#include "../src/BracketStream.h"
#include "../src/StringBuffer.h"
//-----------------------------------------------------------------------------
static void testBrackets();
static void testBracketsWithStrings();
static void testBracketsWithQuotedStrings();
static void testLineComments();
static void testMultiLineComments();
static void testMultiLine();
//-----------------------------------------------------------------------------
void mainBracketStreamTests()
{
  TEST(testBrackets);
  TEST(testBracketsWithStrings);
  TEST(testBracketsWithQuotedStrings);
  TEST(testLineComments);
  TEST(testMultiLineComments);
  TEST(testMultiLine);
}
//-----------------------------------------------------------------------------

//=============================================================================
// TestBracketVisitor
//=============================================================================
STRUCT TestBracketVisitor {
  BracketVisitor self;
  string result;
  StringBuffer statement;
  MemoryPool* pool;
} TestBracketVisitor;
//-----------------------------------------------------------------------------
static string appendPos(MemoryPool* const pool, string s, const StreamChar sc)
{
  CHAR posStr[10];
  sprintf(posStr, "%i:%i:", sc.position.line, sc.position.linePosition);
  return joinStrings(pool, s, posStr);
}
//-----------------------------------------------------------------------------
#define CASE(S,E) case E: typeString = S; break;

static void recordBracketVisit(TestBracketVisitor* const visitor,
                               const StreamChar sc,
                               const string visitType)
{
  string typeString;

  switch(sc.type)
  {
    CASE("{", curlyBrace)
    CASE("[", squareBracket)
    CASE("(", parentheses)
    CASE(";", statement)
    CASE("EOF", fileBracket)
    CASE("\"", doubleQuote)
    CASE("'", singleQuote)
    CASE("`", backtick)
    CASE("/", singleLineComment)
    CASE("*", multiLineComment)
    default: typeString = "UNKNOWN";
  }

  string s = appendPos(visitor->pool, visitor->result, sc);
  s = joinStrings(visitor->pool, s, visitType);
  s = joinStrings(visitor->pool, s, "~");
  s = joinStrings(visitor->pool, s, typeString);
  visitor->result = joinStrings(visitor->pool, s, "~\n");
}
#undef CASE
//-----------------------------------------------------------------------------
static void appendStatement(TestBracketVisitor* const v, const StreamChar type)
{
  if(strlen(v->statement.str) < 1)
    return;

  v->result = joinStrings(v->pool, v->result, "STRING=");
  v->result = joinStrings(v->pool, v->result, v->statement.str);
  v->result = joinStrings(v->pool, v->result, "\n");
  v->statement = newStringBuffer(v->pool, "");
}
//-----------------------------------------------------------------------------
static void testVisitBracketStart(TestBracketVisitor* const visitor, const StreamChar type)
{
  appendStatement(visitor, type);
  recordBracketVisit(visitor, type, "BEGIN");
}
//-----------------------------------------------------------------------------
static void testVisitBracketEnd(TestBracketVisitor* const visitor, const StreamChar type)
{
  appendStatement(visitor, type);
  recordBracketVisit(visitor, type, "END");
}
//-----------------------------------------------------------------------------
static void testVisitBracketEndMissing(TestBracketVisitor* const visitor, const StreamChar type)
{
  appendStatement(visitor, type);
  recordBracketVisit(visitor, type, "END_MISSING");
}
//-----------------------------------------------------------------------------
static void testVisitChar(TestBracketVisitor* const visitor, const StreamChar sc)
{
  if(strcmp(visitor->statement.str, "") == 0)
    visitor->result = appendPos(visitor->pool, visitor->result, sc);

  visitor->statement.appendChar(&(visitor->statement), sc.c);
}
//-----------------------------------------------------------------------------
static TestBracketVisitor newTestBracketVisitor(MemoryPool* const memPool)
{
  TestBracketVisitor visitor = {
    .self = {
      .visitBracketStart = (BracketVisitStart) &testVisitBracketStart,
      .visitBracketEnd = (BracketVisitEnd) testVisitBracketEnd,
      .visitBracketEndMissing = (BracketVisitEndMissing) &testVisitBracketEndMissing,
      .visitChar = (BracketVisitChar) &testVisitChar
    },

    .result = "",
    .statement = newStringBuffer(memPool, ""),
    .pool = memPool
  };

  return visitor;
}
//-----------------------------------------------------------------------------
static void checkResults(const string input, const string expected, const string file, const unsigned int line)
{
  MemoryPool pool = newMemoryPool();

  TestBracketVisitor visitor = newTestBracketVisitor(&pool);
  InMemoryCharStream istream = newInMemoryCharStream(input);
  BracketStream stream = newBracketStream(&(istream.stream));
  stream.visit(&stream, &(visitor.self));
  _assertEquals_string(expected, visitor.result, file, line);
  pool.drain(&pool);
}
//-----------------------------------------------------------------------------
#define CHECK_RESULTS(A, B) checkResults(A, B, __FILE__, __LINE__)
//=============================================================================
static void testBrackets()
{
  CHECK_RESULTS("",
                "1:0:BEGIN~EOF~\n"
                "1:0:END~EOF~\n");

  CHECK_RESULTS(";",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:END~;~\n"
                "1:1:END~EOF~\n");

  CHECK_RESULTS("{}",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~{~\n"
                "1:2:END~{~\n"
                "1:2:END_MISSING~;~\n"
                "1:2:END~EOF~\n");

  CHECK_RESULTS("[]",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~[~\n"
                "1:2:END~[~\n"
                "1:2:END_MISSING~;~\n"
                "1:2:END~EOF~\n");

  CHECK_RESULTS("()",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~(~\n"
                "1:2:END~(~\n"
                "1:2:END_MISSING~;~\n"
                "1:2:END~EOF~\n");

  CHECK_RESULTS("{};",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~{~\n"
                "1:2:END~{~\n"
                "1:3:END~;~\n"
                "1:3:END~EOF~\n");

  CHECK_RESULTS("[];",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~[~\n"
                "1:2:END~[~\n"
                "1:3:END~;~\n"
                "1:3:END~EOF~\n");

  CHECK_RESULTS("();",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~(~\n"
                "1:2:END~(~\n"
                "1:3:END~;~\n"
                "1:3:END~EOF~\n");

  CHECK_RESULTS("(;);",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~(~\n"
                "1:2:BEGIN~;~\n"
                "1:2:END~;~\n"
                "1:3:END~(~\n"
                "1:4:END~;~\n"
                "1:4:END~EOF~\n");

  CHECK_RESULTS("(;",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~(~\n"
                "1:2:BEGIN~;~\n"
                "1:2:END~;~\n"
                "1:2:END_MISSING~(~\n"
                "1:2:END_MISSING~;~\n"
                "1:2:END~EOF~\n");

  CHECK_RESULTS("{};{};",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~{~\n"
                "1:2:END~{~\n"
                "1:3:END~;~\n"
                "1:4:BEGIN~;~\n"
                "1:4:BEGIN~{~\n"
                "1:5:END~{~\n"
                "1:6:END~;~\n"
                "1:6:END~EOF~\n");

  CHECK_RESULTS("{}{};",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~{~\n"
                "1:2:END~{~\n"
                "1:3:BEGIN~{~\n"
                "1:4:END~{~\n"
                "1:5:END~;~\n"
                "1:5:END~EOF~\n");

  CHECK_RESULTS("{}{}",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~{~\n"
                "1:2:END~{~\n"
                "1:3:BEGIN~{~\n"
                "1:4:END~{~\n"
                "1:4:END_MISSING~;~\n"
                "1:4:END~EOF~\n");

  CHECK_RESULTS("{[();];};",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~{~\n"
                "1:2:BEGIN~;~\n"
                "1:2:BEGIN~[~\n"
                "1:3:BEGIN~;~\n"
                "1:3:BEGIN~(~\n"
                "1:4:END~(~\n"
                "1:5:END~;~\n"
                "1:6:END~[~\n"
                "1:7:END~;~\n"
                "1:8:END~{~\n"
                "1:9:END~;~\n"
                "1:9:END~EOF~\n");

  CHECK_RESULTS("{[();;};",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~{~\n"
                "1:2:BEGIN~;~\n"
                "1:2:BEGIN~[~\n"
                "1:3:BEGIN~;~\n"
                "1:3:BEGIN~(~\n"
                "1:4:END~(~\n"
                "1:5:END~;~\n"
                "1:6:BEGIN~;~\n"
                "1:6:END~;~\n"
                "1:7:END_MISSING~[~\n"
                "1:7:END_MISSING~;~\n"
                "1:7:END~{~\n"
                "1:8:END~;~\n"
                "1:8:END~EOF~\n");
}
//-----------------------------------------------------------------------------
static void testBracketsWithStrings()
{
  CHECK_RESULTS("a test statement;",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:STRING=a test statement\n"
                "1:17:END~;~\n"
                "1:17:END~EOF~\n");

  CHECK_RESULTS("test statement A; test statement B;",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:STRING=test statement A\n"
                "1:17:END~;~\n"
                "1:18:BEGIN~;~\n"
                "1:18:STRING= test statement B\n"
                "1:35:END~;~\n"
                "1:35:END~EOF~\n");

  CHECK_RESULTS("{a test statement;};",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~{~\n"
                "1:2:BEGIN~;~\n"
                "1:2:STRING=a test statement\n"
                "1:18:END~;~\n"
                "1:19:END~{~\n"
                "1:20:END~;~\n"
                "1:20:END~EOF~\n");

  CHECK_RESULTS("{a test statement};",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~{~\n"
                "1:2:BEGIN~;~\n"
                "1:2:STRING=a test statement\n"
                "1:18:END_MISSING~;~\n"
                "1:18:END~{~\n"
                "1:19:END~;~\n"
                "1:19:END~EOF~\n");

  CHECK_RESULTS("{test statement A; test statement B;};",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~{~\n"
                "1:2:BEGIN~;~\n"
                "1:2:STRING=test statement A\n"
                "1:18:END~;~\n"
                "1:19:BEGIN~;~\n"
                "1:19:STRING= test statement B\n"
                "1:36:END~;~\n"
                "1:37:END~{~\n"
                "1:38:END~;~\n"
                "1:38:END~EOF~\n");

  CHECK_RESULTS("{ };{ };",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~{~\n"
                "1:2:BEGIN~;~\n"
                "1:2:STRING= \n"
                "1:3:END_MISSING~;~\n"
                "1:3:END~{~\n"
                "1:4:END~;~\n"
                "1:5:BEGIN~;~\n"
                "1:5:BEGIN~{~\n"
                "1:6:BEGIN~;~\n"
                "1:6:STRING= \n"
                "1:7:END_MISSING~;~\n"
                "1:7:END~{~\n"
                "1:8:END~;~\n"
                "1:8:END~EOF~\n");

  CHECK_RESULTS("a test {} statement;",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:STRING=a test \n"
                "1:8:BEGIN~{~\n"
                "1:9:END~{~\n"
                "1:10:STRING= statement\n"
                "1:20:END~;~\n"
                "1:20:END~EOF~\n");
}
//-----------------------------------------------------------------------------
static void testBracketsWithQuotedStrings()
{
  CHECK_RESULTS("'a test statement';",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~'~\n"
                "1:1:STRING='a test statement'\n"
                "1:18:END~'~\n"
                "1:19:END~;~\n"
                "1:19:END~EOF~\n");

  CHECK_RESULTS("\"a test statement\";",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~\"~\n"
                "1:1:STRING=\"a test statement\"\n"
                "1:18:END~\"~\n"
                "1:19:END~;~\n"
                "1:19:END~EOF~\n");

  CHECK_RESULTS("{'a test statement';};",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~{~\n"
                "1:2:BEGIN~;~\n"
                "1:2:BEGIN~'~\n"
                "1:2:STRING='a test statement'\n"
                "1:19:END~'~\n"
                "1:20:END~;~\n"
                "1:21:END~{~\n"
                "1:22:END~;~\n"
                "1:22:END~EOF~\n");

  CHECK_RESULTS("{'a \\'test statement';};",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~{~\n"
                "1:2:BEGIN~;~\n"
                "1:2:BEGIN~'~\n"
                "1:2:STRING='a \\'test statement'\n"
                "1:21:END~'~\n"
                "1:22:END~;~\n"
                "1:23:END~{~\n"
                "1:24:END~;~\n"
                "1:24:END~EOF~\n");

  CHECK_RESULTS("{\"a \\\"test statement\";};",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~{~\n"
                "1:2:BEGIN~;~\n"
                "1:2:BEGIN~\"~\n"
                "1:2:STRING=\"a \\\"test statement\"\n"
                "1:21:END~\"~\n"
                "1:22:END~;~\n"
                "1:23:END~{~\n"
                "1:24:END~;~\n"
                "1:24:END~EOF~\n");

  CHECK_RESULTS("{'a \"test statement';};",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~{~\n"
                "1:2:BEGIN~;~\n"
                "1:2:BEGIN~'~\n"
                "1:2:STRING='a \"test statement'\n"
                "1:20:END~'~\n"
                "1:21:END~;~\n"
                "1:22:END~{~\n"
                "1:23:END~;~\n"
                "1:23:END~EOF~\n");

  CHECK_RESULTS("{\"a \\'test statement\";};",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~{~\n"
                "1:2:BEGIN~;~\n"
                "1:2:BEGIN~\"~\n"
                "1:2:STRING=\"a \\'test statement\"\n"
                "1:21:END~\"~\n"
                "1:22:END~;~\n"
                "1:23:END~{~\n"
                "1:24:END~;~\n"
                "1:24:END~EOF~\n");

  CHECK_RESULTS("{'a test } statement';};",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~{~\n"
                "1:2:BEGIN~;~\n"
                "1:2:BEGIN~'~\n"
                "1:2:STRING='a test } statement'\n"
                "1:21:END~'~\n"
                "1:22:END~;~\n"
                "1:23:END~{~\n"
                "1:24:END~;~\n"
                "1:24:END~EOF~\n");

  CHECK_RESULTS("{\"a test } statement\";};",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~{~\n"
                "1:2:BEGIN~;~\n"
                "1:2:BEGIN~\"~\n"
                "1:2:STRING=\"a test } statement\"\n"
                "1:21:END~\"~\n"
                "1:22:END~;~\n"
                "1:23:END~{~\n"
                "1:24:END~;~\n"
                "1:24:END~EOF~\n");

  CHECK_RESULTS("'a test { statement';",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~'~\n"
                "1:1:STRING='a test { statement'\n"
                "1:20:END~'~\n"
                "1:21:END~;~\n"
                "1:21:END~EOF~\n");

  CHECK_RESULTS("'a test } ] ; ) statement';",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~'~\n"
                "1:1:STRING='a test } ] ; ) statement'\n"
                "1:26:END~'~\n"
                "1:27:END~;~\n"
                "1:27:END~EOF~\n");

  CHECK_RESULTS("'a test statement;",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~'~\n"
                "1:1:STRING='a test statement;\n"
                "1:18:END_MISSING~'~\n"
                "1:18:END_MISSING~;~\n"
                "1:18:END~EOF~\n");

  CHECK_RESULTS("{'a test statement;};",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~{~\n"
                "1:2:BEGIN~;~\n"
                "1:2:BEGIN~'~\n"
                "1:2:STRING='a test statement;};\n"
                "1:21:END_MISSING~'~\n"
                "1:21:END_MISSING~;~\n"
                "1:21:END_MISSING~{~\n"
                "1:21:END_MISSING~;~\n"
                "1:21:END~EOF~\n");

  CHECK_RESULTS("\"a test statement;",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~\"~\n"
                "1:1:STRING=\"a test statement;\n"
                "1:18:END_MISSING~\"~\n"
                "1:18:END_MISSING~;~\n"
                "1:18:END~EOF~\n");

  CHECK_RESULTS("{\"a test statement;};",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~{~\n"
                "1:2:BEGIN~;~\n"
                "1:2:BEGIN~\"~\n"
                "1:2:STRING=\"a test statement;};\n"
                "1:21:END_MISSING~\"~\n"
                "1:21:END_MISSING~;~\n"
                "1:21:END_MISSING~{~\n"
                "1:21:END_MISSING~;~\n"
                "1:21:END~EOF~\n");
}
//-----------------------------------------------------------------------------
static void testLineComments()
{
  CHECK_RESULTS("//test statement\n",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~/~\n"
                "1:1:STRING=//test statement\n\n"
                "2:0:END~/~\n"
                "2:0:END_MISSING~;~\n"
                "2:0:END~EOF~\n");

  CHECK_RESULTS("//test statement\\\ny",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~/~\n"
                "1:1:STRING=//test statement\\\n\n"
                "2:0:END~/~\n"
                "2:1:STRING=y\n"
                "2:1:END_MISSING~;~\n"
                "2:1:END~EOF~\n");

  CHECK_RESULTS("//test statement",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~/~\n"
                "1:1:STRING=//test statement\n"
                "1:16:END_MISSING~/~\n"
                "1:16:END_MISSING~;~\n"
                "1:16:END~EOF~\n");

  CHECK_RESULTS("a; x//test statement\ny;",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:STRING=a\n"
                "1:2:END~;~\n"
                "1:3:BEGIN~;~\n"
                "1:3:STRING= x\n"
                "1:5:BEGIN~/~\n"
                "1:5:STRING=//test statement\n\n"
                "2:0:END~/~\n"
                "2:1:STRING=y\n"
                "2:2:END~;~\n"
                "2:2:END~EOF~\n");

  CHECK_RESULTS("a; x//test stat//ement\ny;",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:STRING=a\n"
                "1:2:END~;~\n"
                "1:3:BEGIN~;~\n"
                "1:3:STRING= x\n"
                "1:5:BEGIN~/~\n"
                "1:5:STRING=//test stat//ement\n\n"
                "2:0:END~/~\n"
                "2:1:STRING=y\n"
                "2:2:END~;~\n"
                "2:2:END~EOF~\n");

  CHECK_RESULTS("a; 2/x//test statement\ny;",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:STRING=a\n"
                "1:2:END~;~\n"
                "1:3:BEGIN~;~\n"
                "1:3:STRING= 2/x\n"
                "1:7:BEGIN~/~\n"
                "1:7:STRING=//test statement\n\n"
                "2:0:END~/~\n"
                "2:1:STRING=y\n"
                "2:2:END~;~\n"
                "2:2:END~EOF~\n");

  CHECK_RESULTS("a; 2/x/3 //test statement\ny;",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:STRING=a\n"
                "1:2:END~;~\n"
                "1:3:BEGIN~;~\n"
                "1:3:STRING= 2/x/3 \n"
                "1:10:BEGIN~/~\n"
                "1:10:STRING=//test statement\n\n"
                "2:0:END~/~\n"
                "2:1:STRING=y\n"
                "2:2:END~;~\n"
                "2:2:END~EOF~\n");

  CHECK_RESULTS("\"a; 2/x/3 //test statement\ny;\"",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~\"~\n"
                "1:1:STRING=\"a; 2/x/3 //test statement\ny;\"\n"
                "2:3:END~\"~\n"
                "2:3:END_MISSING~;~\n"
                "2:3:END~EOF~\n");
}
//-----------------------------------------------------------------------------
void testMultiLineComments()
{
  CHECK_RESULTS("/* test */",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~*~\n"
                "1:1:STRING=/* test */\n"
                "1:10:END~*~\n"
                "1:10:END_MISSING~;~\n"
                "1:10:END~EOF~\n");

  CHECK_RESULTS("/** test **/",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~*~\n"
                "1:1:STRING=/** test **/\n"
                "1:12:END~*~\n"
                "1:12:END_MISSING~;~\n"
                "1:12:END~EOF~\n");

  CHECK_RESULTS("/**\n* test\n**/;",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~*~\n"
                "1:1:STRING=/**\n* test\n**/\n"
                "3:3:END~*~\n"
                "3:4:END~;~\n"
                "3:4:END~EOF~\n");

  CHECK_RESULTS("/** /test **/",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~*~\n"
                "1:1:STRING=/** /test **/\n"
                "1:13:END~*~\n"
                "1:13:END_MISSING~;~\n"
                "1:13:END~EOF~\n");


  CHECK_RESULTS("/**/test **/x",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~*~\n"
                "1:1:STRING=/**/\n"
                "1:4:END~*~\n"
                "1:5:STRING=test **/x\n"
                "1:13:END_MISSING~;~\n"
                "1:13:END~EOF~\n");

  CHECK_RESULTS("/**/test **/{}",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~*~\n"
                "1:1:STRING=/**/\n"
                "1:4:END~*~\n"
                "1:5:STRING=test **/\n"
                "1:13:BEGIN~{~\n"
                "1:14:END~{~\n"
                "1:14:END_MISSING~;~\n"
                "1:14:END~EOF~\n");

  CHECK_RESULTS("/*/* test **/",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~*~\n"
                "1:1:STRING=/*/* test **/\n"
                "1:13:END~*~\n"
                "1:13:END_MISSING~;~\n"
                "1:13:END~EOF~\n");
}
//-----------------------------------------------------------------------------
static void testMultiLine()
{
  CHECK_RESULTS("abcd;\n"
                "{abcd;};\n"
                "// test;\n"
                "[1,2,3,4];\n",

                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:STRING=abcd\n"
                "1:5:END~;~\n"
                "2:0:BEGIN~;~\n"
                "2:0:STRING=\n"
                "\n"
                "2:1:BEGIN~{~\n"
                "2:2:BEGIN~;~\n"
                "2:2:STRING=abcd\n"
                "2:6:END~;~\n"
                "2:7:END~{~\n"
                "2:8:END~;~\n"
                "3:0:BEGIN~;~\n"
                "3:0:STRING=\n"
                "\n"
                "3:1:BEGIN~/~\n"
                "3:1:STRING=// test;\n"
                "\n"
                "4:0:END~/~\n"
                "4:1:BEGIN~[~\n"
                "4:2:BEGIN~;~\n"
                "4:2:STRING=1,2,3,4\n"
                "4:9:END_MISSING~;~\n"
                "4:9:END~[~\n"
                "4:10:END~;~\n"
                "5:0:BEGIN~;~\n"
                "5:0:STRING=\n"
                "\n"
                "5:0:END_MISSING~;~\n"
                "5:0:END~EOF~\n");
}
//-----------------------------------------------------------------------------