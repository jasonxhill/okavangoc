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
static void recordBracketVisit(BracketVisitor* const visitor,
                               const StreamChar type,
                               const string visitType)
{
  CHAR posStr[10];
  sprintf(posStr, "%i:%i:", type.line, type.linePosition);

  TestBracketVisitor* const v = (TestBracketVisitor*) visitor;
  char sType[2] = {type.c, 0};
  const string typeString = type.c != END_STREAM? sType : "EOF";

  string s = joinStrings(v->pool, v->result, posStr);
  s = joinStrings(v->pool, s, visitType);
  s = joinStrings(v->pool, s, "~");
  s = joinStrings(v->pool, s, typeString);
  v->result = joinStrings(v->pool, s, "~\n");
}
//-----------------------------------------------------------------------------
static void appendStatement(TestBracketVisitor* const v)
{
  if(strlen(v->statement.str) < 1)
    return;

  v->result = joinStrings(v->pool, v->result, "STRING=");
  v->result = joinStrings(v->pool, v->result, v->statement.str);
  v->result = joinStrings(v->pool, v->result, "\n");
  v->statement = newStringBuffer(v->pool, "");
}
//-----------------------------------------------------------------------------
static void testVisitBracketStart(BracketVisitor* const visitor, const StreamChar type)
{
  appendStatement((TestBracketVisitor*) visitor);
  recordBracketVisit(visitor, type, "BEGIN");
}
//-----------------------------------------------------------------------------
static void testVisitBracketEnd(BracketVisitor* const visitor, const StreamChar type)
{
  TestBracketVisitor* const v = (TestBracketVisitor*) visitor;
  appendStatement(v);
  recordBracketVisit(visitor, type, "END");
}
//-----------------------------------------------------------------------------
static void testVisitBracketEndMissing(BracketVisitor* const visitor, const StreamChar type)
{
  TestBracketVisitor* const v = (TestBracketVisitor*) visitor;
  appendStatement(v);
  recordBracketVisit(visitor, type, "END_MISSING");
}
//-----------------------------------------------------------------------------
static void testVisitBracketChar(BracketVisitor* const visitor, const StreamChar streamChar)
{
  TestBracketVisitor* const v = (TestBracketVisitor*) visitor;
  v->statement.appendChar(&(v->statement), streamChar.c);
}
//-----------------------------------------------------------------------------
static TestBracketVisitor newTestBracketVisitor(MemoryPool* const memPool)
{
  TestBracketVisitor visitor = {
    .self = {
      .visitBracketStart = &testVisitBracketStart,
      .visitBracketEnd = testVisitBracketEnd,
      .visitBracketEndMissing = &testVisitBracketEndMissing,
      .visitChar = &testVisitBracketChar
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
                "STRING=a test statement\n"
                "1:17:END~;~\n"
                "1:17:END~EOF~\n");

  CHECK_RESULTS("test statement A; test statement B;",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "STRING=test statement A\n"
                "1:17:END~;~\n"
                "1:18:BEGIN~;~\n"
                "STRING= test statement B\n"
                "1:35:END~;~\n"
                "1:35:END~EOF~\n");

  CHECK_RESULTS("{a test statement;};",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~{~\n"
                "1:2:BEGIN~;~\n"
                "STRING=a test statement\n"
                "1:18:END~;~\n"
                "1:19:END~{~\n"
                "1:20:END~;~\n"
                "1:20:END~EOF~\n");

  CHECK_RESULTS("{a test statement};",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~{~\n"
                "1:2:BEGIN~;~\n"
                "STRING=a test statement\n"
                "1:18:END_MISSING~;~\n"
                "1:18:END~{~\n"
                "1:19:END~;~\n"
                "1:19:END~EOF~\n");

  CHECK_RESULTS("{test statement A; test statement B;};",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~{~\n"
                "1:2:BEGIN~;~\n"
                "STRING=test statement A\n"
                "1:18:END~;~\n"
                "1:19:BEGIN~;~\n"
                "STRING= test statement B\n"
                "1:36:END~;~\n"
                "1:37:END~{~\n"
                "1:38:END~;~\n"
                "1:38:END~EOF~\n");

  CHECK_RESULTS("{ };{ };",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~{~\n"
                "1:2:BEGIN~;~\n"
                "STRING= \n"
                "1:3:END_MISSING~;~\n"
                "1:3:END~{~\n"
                "1:4:END~;~\n"
                "1:5:BEGIN~;~\n"
                "1:5:BEGIN~{~\n"
                "1:6:BEGIN~;~\n"
                "STRING= \n"
                "1:7:END_MISSING~;~\n"
                "1:7:END~{~\n"
                "1:8:END~;~\n"
                "1:8:END~EOF~\n");

  CHECK_RESULTS("a test {} statement;",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "STRING=a test \n"
                "1:8:BEGIN~{~\n"
                "1:9:END~{~\n"
                "STRING= statement\n"
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
                "STRING='a test statement'\n"
                "1:18:END~'~\n"
                "1:19:END~;~\n"
                "1:19:END~EOF~\n");

  CHECK_RESULTS("\"a test statement\";",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~\"~\n"
                "STRING=\"a test statement\"\n"
                "1:18:END~\"~\n"
                "1:19:END~;~\n"
                "1:19:END~EOF~\n");

  CHECK_RESULTS("{'a test statement';};",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~{~\n"
                "1:2:BEGIN~;~\n"
                "1:2:BEGIN~'~\n"
                "STRING='a test statement'\n"
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
                "STRING='a \\'test statement'\n"
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
                "STRING=\"a \\\"test statement\"\n"
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
                "STRING='a \"test statement'\n"
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
                "STRING=\"a \\'test statement\"\n"
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
                "STRING='a test } statement'\n"
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
                "STRING=\"a test } statement\"\n"
                "1:21:END~\"~\n"
                "1:22:END~;~\n"
                "1:23:END~{~\n"
                "1:24:END~;~\n"
                "1:24:END~EOF~\n");

  CHECK_RESULTS("'a test { statement';",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~'~\n"
                "STRING='a test { statement'\n"
                "1:20:END~'~\n"
                "1:21:END~;~\n"
                "1:21:END~EOF~\n");

  CHECK_RESULTS("'a test } ] ; ) statement';",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~'~\n"
                "STRING='a test } ] ; ) statement'\n"
                "1:26:END~'~\n"
                "1:27:END~;~\n"
                "1:27:END~EOF~\n");

  CHECK_RESULTS("'a test statement;",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~'~\n"
                "STRING='a test statement;\n"
                "1:18:END_MISSING~'~\n"
                "1:18:END_MISSING~;~\n"
                "1:18:END~EOF~\n");

  CHECK_RESULTS("{'a test statement;};",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~{~\n"
                "1:2:BEGIN~;~\n"
                "1:2:BEGIN~'~\n"
                "STRING='a test statement;};\n"
                "1:21:END_MISSING~'~\n"
                "1:21:END_MISSING~;~\n"
                "1:21:END_MISSING~{~\n"
                "1:21:END_MISSING~;~\n"
                "1:21:END~EOF~\n");

  CHECK_RESULTS("\"a test statement;",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~\"~\n"
                "STRING=\"a test statement;\n"
                "1:18:END_MISSING~\"~\n"
                "1:18:END_MISSING~;~\n"
                "1:18:END~EOF~\n");

  CHECK_RESULTS("{\"a test statement;};",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~{~\n"
                "1:2:BEGIN~;~\n"
                "1:2:BEGIN~\"~\n"
                "STRING=\"a test statement;};\n"
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
                "1:2:BEGIN~/~\n"
                "STRING=//test statement\n\n"
                "2:0:END~/~\n"
                "2:0:END_MISSING~;~\n"
                "2:0:END~EOF~\n");

  CHECK_RESULTS("//test statement\\\ny",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:2:BEGIN~/~\n"
                "STRING=//test statement\\\n\n"
                "2:0:END~/~\n"
                "STRING=y\n"
                "2:1:END_MISSING~;~\n"
                "2:1:END~EOF~\n");

  CHECK_RESULTS("//test statement",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:2:BEGIN~/~\n"
                "STRING=//test statement\n"
                "1:16:END_MISSING~/~\n"
                "1:16:END_MISSING~;~\n"
                "1:16:END~EOF~\n");

  CHECK_RESULTS("a; x//test statement\ny;",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "STRING=a\n"
                "1:2:END~;~\n"
                "1:3:BEGIN~;~\n"
                "STRING= x\n"
                "1:6:BEGIN~/~\n"
                "STRING=//test statement\n\n"
                "2:0:END~/~\n"
                "STRING=y\n"
                "2:2:END~;~\n"
                "2:2:END~EOF~\n");

  CHECK_RESULTS("a; x//test stat//ement\ny;",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "STRING=a\n"
                "1:2:END~;~\n"
                "1:3:BEGIN~;~\n"
                "STRING= x\n"
                "1:6:BEGIN~/~\n"
                "STRING=//test stat//ement\n\n"
                "2:0:END~/~\n"
                "STRING=y\n"
                "2:2:END~;~\n"
                "2:2:END~EOF~\n");

  CHECK_RESULTS("a; 2/x//test statement\ny;",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "STRING=a\n"
                "1:2:END~;~\n"
                "1:3:BEGIN~;~\n"
                "STRING= 2/x\n"
                "1:8:BEGIN~/~\n"
                "STRING=//test statement\n\n"
                "2:0:END~/~\n"
                "STRING=y\n"
                "2:2:END~;~\n"
                "2:2:END~EOF~\n");

  CHECK_RESULTS("a; 2/x/3 //test statement\ny;",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "STRING=a\n"
                "1:2:END~;~\n"
                "1:3:BEGIN~;~\n"
                "STRING= 2/x/3 \n"
                "1:11:BEGIN~/~\n"
                "STRING=//test statement\n\n"
                "2:0:END~/~\n"
                "STRING=y\n"
                "2:2:END~;~\n"
                "2:2:END~EOF~\n");

  CHECK_RESULTS("\"a; 2/x/3 //test statement\ny;\"",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:1:BEGIN~\"~\n"
                "STRING=\"a; 2/x/3 //test statement\ny;\"\n"
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
                "1:2:BEGIN~*~\n"
                "STRING=/* test */\n"
                "1:10:END~*~\n"
                "1:10:END_MISSING~;~\n"
                "1:10:END~EOF~\n");

  CHECK_RESULTS("/** test **/",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:2:BEGIN~*~\n"
                "STRING=/** test **/\n"
                "1:12:END~*~\n"
                "1:12:END_MISSING~;~\n"
                "1:12:END~EOF~\n");

  CHECK_RESULTS("/**\n* test\n**/;",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:2:BEGIN~*~\n"
                "STRING=/**\n* test\n**/\n"
                "3:3:END~*~\n"
                "3:4:END~;~\n"
                "3:4:END~EOF~\n");

  CHECK_RESULTS("/** /test **/",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:2:BEGIN~*~\n"
                "STRING=/** /test **/\n"
                "1:13:END~*~\n"
                "1:13:END_MISSING~;~\n"
                "1:13:END~EOF~\n");


  CHECK_RESULTS("/**/test **/x",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:2:BEGIN~*~\n"
                "STRING=/**/\n"
                "1:4:END~*~\n"
                "STRING=test **/x\n"
                "1:13:END_MISSING~;~\n"
                "1:13:END~EOF~\n");

  CHECK_RESULTS("/**/test **/{}",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:2:BEGIN~*~\n"
                "STRING=/**/\n"
                "1:4:END~*~\n"
                "STRING=test **/\n"
                "1:13:BEGIN~{~\n"
                "1:14:END~{~\n"
                "1:14:END_MISSING~;~\n"
                "1:14:END~EOF~\n");

  CHECK_RESULTS("/*/* test **/",
                "1:0:BEGIN~EOF~\n"
                "1:1:BEGIN~;~\n"
                "1:2:BEGIN~*~\n"
                "STRING=/*/* test **/\n"
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
                "STRING=abcd\n"
                "1:5:END~;~\n"
                "2:0:BEGIN~;~\n"
                "STRING=\n"
                "\n"
                "2:1:BEGIN~{~\n"
                "2:2:BEGIN~;~\n"
                "STRING=abcd\n"
                "2:6:END~;~\n"
                "2:7:END~{~\n"
                "2:8:END~;~\n"
                "3:0:BEGIN~;~\n"
                "STRING=\n"
                "\n"
                "3:2:BEGIN~/~\n"
                "STRING=// test;\n"
                "\n"
                "4:0:END~/~\n"
                "4:1:BEGIN~[~\n"
                "4:2:BEGIN~;~\n"
                "STRING=1,2,3,4\n"
                "4:9:END_MISSING~;~\n"
                "4:9:END~[~\n"
                "4:10:END~;~\n"
                "5:0:BEGIN~;~\n"
                "STRING=\n"
                "\n"
                "5:0:END_MISSING~;~\n"
                "5:0:END~EOF~\n");
}
//-----------------------------------------------------------------------------