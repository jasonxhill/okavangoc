//=============================================================================
// BracketStreamTest.c
//=============================================================================
#include "TestUtil.h"
#include "../src/BracketStream.h"
//-----------------------------------------------------------------------------
static void testBrackets();
static void testBracketsWithStrings();
static void testBracketsWithQuotedStrings();
static void testLineComments();
static void testMultiLineComments();
//-----------------------------------------------------------------------------
void mainBracketStreamTests()
{
  TEST(testBrackets);
  TEST(testBracketsWithStrings);
  TEST(testBracketsWithQuotedStrings);
  TEST(testLineComments);
  TEST(testMultiLineComments);
}
//-----------------------------------------------------------------------------

//=============================================================================
// TestBracketVisitor
//=============================================================================
STRUCT TestBracketVisitor {
  BracketVisitor self;
  String result;
  String statement;
  AutoReleasePool pool;
} TestBracketVisitor;
//-----------------------------------------------------------------------------
static void recordBracketVisit(BracketVisitor* const visitor, const CHAR type, const string visitType)
{
  TestBracketVisitor* const v = (TestBracketVisitor*) visitor;
  char sType[2] = {type, 0};
  const String typeString = StringOf(type != END_STREAM? sType : "EOF");

  String s = joinStrings(&(v->pool), v->result, StringOf(visitType));
  s = joinStrings(&(v->pool), s, StringOf("~"));
  s = joinStrings(&(v->pool), s, typeString);
  v->result = joinStrings(&(v->pool), s, StringOf("~\n"));
}
//-----------------------------------------------------------------------------
static void appendStatement(TestBracketVisitor* const v)
{
  if(v->statement.size < 1)
    return;

  v->result = joinStrings(&(v->pool), v->result, StringOf("STRING="));
  v->result = joinStrings(&(v->pool), v->result, v->statement);
  v->result = joinStrings(&(v->pool), v->result, StringOf("\n"));
  v->statement = StringOf("");
}
//-----------------------------------------------------------------------------
static void testVisitBracketStart(BracketVisitor* const visitor, const CHAR type)
{
  appendStatement((TestBracketVisitor*) visitor);
  recordBracketVisit(visitor, type, "BEGIN");
}
//-----------------------------------------------------------------------------
static void testVisitBracketEnd(BracketVisitor* const visitor, const CHAR type)
{
  TestBracketVisitor* const v = (TestBracketVisitor*) visitor;
  appendStatement(v);
  recordBracketVisit(visitor, type, "END");
}
//-----------------------------------------------------------------------------
static void testVisitBracketEndMissing(BracketVisitor* const visitor, const CHAR type)
{
  TestBracketVisitor* const v = (TestBracketVisitor*) visitor;
  appendStatement(v);
  recordBracketVisit(visitor, type, "END_MISSING");
}
//-----------------------------------------------------------------------------
static void testVisitBracketChar(BracketVisitor* const visitor, const CHAR c)
{
  TestBracketVisitor* const v = (TestBracketVisitor*) visitor;
  v->statement = appendChar(&(v->pool), v->statement, c, 10);
}
//-----------------------------------------------------------------------------
static TestBracketVisitor newTestBracketVisitor()
{
  TestBracketVisitor visitor = {
    .self = {
      .visitBracketStart = &testVisitBracketStart,
      .visitBracketEnd = testVisitBracketEnd,
      .visitBracketEndMissing = &testVisitBracketEndMissing,
      .visitChar = &testVisitBracketChar
    },

    .result = StringOf(""),
    .statement = StringOf(""),
    .pool = newAutoReleasePool()
  };

  return visitor;
}
//-----------------------------------------------------------------------------
static void checkResults(const string input, const string expected, const string file, const unsigned int line)
{
  TestBracketVisitor visitor = newTestBracketVisitor();
  InMemoryCharStream istream = newInMemoryCharStream(input);
  BracketStream stream = newBracketStream(&(istream.stream));
  stream.visit(&stream, &(visitor.self));
  _assertEquals_string(expected, visitor.result.str, file, line);
  visitor.pool.drain(&(visitor.pool));
}
//-----------------------------------------------------------------------------
#define CHECK_RESULTS(A, B) checkResults(A, B, __FILE__, __LINE__)
//=============================================================================
static void testBrackets()
{
  CHECK_RESULTS("",
                "BEGIN~EOF~\n"
                "END~EOF~\n");

  CHECK_RESULTS(";",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "END~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("{}",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~{~\n"
                "END~{~\n"
                "END_MISSING~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("[]",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~[~\n"
                "END~[~\n"
                "END_MISSING~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("()",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~(~\n"
                "END~(~\n"
                "END_MISSING~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("{};",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~{~\n"
                "END~{~\n"
                "END~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("[];",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~[~\n"
                "END~[~\n"
                "END~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("();",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~(~\n"
                "END~(~\n"
                "END~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("(;);",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~(~\n"
                "BEGIN~;~\n"
                "END~;~\n"
                "END~(~\n"
                "END~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("(;",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~(~\n"
                "BEGIN~;~\n"
                "END~;~\n"
                "END_MISSING~(~\n"
                "END_MISSING~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("{};{};",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~{~\n"
                "END~{~\n"
                "END~;~\n"
                "BEGIN~;~\n"
                "BEGIN~{~\n"
                "END~{~\n"
                "END~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("{}{};",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~{~\n"
                "END~{~\n"
                "BEGIN~{~\n"
                "END~{~\n"
                "END~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("{}{}",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~{~\n"
                "END~{~\n"
                "BEGIN~{~\n"
                "END~{~\n"
                "END_MISSING~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("{[();];};",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~{~\n"
                "BEGIN~;~\n"
                "BEGIN~[~\n"
                "BEGIN~;~\n"
                "BEGIN~(~\n"
                "END~(~\n"
                "END~;~\n"
                "END~[~\n"
                "END~;~\n"
                "END~{~\n"
                "END~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("{[();;};",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~{~\n"
                "BEGIN~;~\n"
                "BEGIN~[~\n"
                "BEGIN~;~\n"
                "BEGIN~(~\n"
                "END~(~\n"
                "END~;~\n"
                "BEGIN~;~\n"
                "END~;~\n"
                "END_MISSING~[~\n"
                "END_MISSING~;~\n"
                "END~{~\n"
                "END~;~\n"
                "END~EOF~\n");
}
//-----------------------------------------------------------------------------
static void testBracketsWithStrings()
{
  CHECK_RESULTS("a test statement;",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "STRING=a test statement\n"
                "END~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("test statement A; test statement B;",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "STRING=test statement A\n"
                "END~;~\n"
                "BEGIN~;~\n"
                "STRING= test statement B\n"
                "END~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("{a test statement;};",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~{~\n"
                "BEGIN~;~\n"
                "STRING=a test statement\n"
                "END~;~\n"
                "END~{~\n"
                "END~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("{a test statement};",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~{~\n"
                "BEGIN~;~\n"
                "STRING=a test statement\n"
                "END_MISSING~;~\n"
                "END~{~\n"
                "END~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("{test statement A; test statement B;};",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~{~\n"
                "BEGIN~;~\n"
                "STRING=test statement A\n"
                "END~;~\n"
                "BEGIN~;~\n"
                "STRING= test statement B\n"
                "END~;~\n"
                "END~{~\n"
                "END~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("{ };{ };",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~{~\n"
                "BEGIN~;~\n"
                "STRING= \n"
                "END_MISSING~;~\n"
                "END~{~\n"
                "END~;~\n"
                "BEGIN~;~\n"
                "BEGIN~{~\n"
                "BEGIN~;~\n"
                "STRING= \n"
                "END_MISSING~;~\n"
                "END~{~\n"
                "END~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("a test {} statement;",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "STRING=a test \n"
                "BEGIN~{~\n"
                "END~{~\n"
                "STRING= statement\n"
                "END~;~\n"
                "END~EOF~\n");
}
//-----------------------------------------------------------------------------
static void testBracketsWithQuotedStrings()
{
  CHECK_RESULTS("'a test statement';",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~'~\n"
                "STRING='a test statement'\n"
                "END~'~\n"
                "END~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("\"a test statement\";",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~\"~\n"
                "STRING=\"a test statement\"\n"
                "END~\"~\n"
                "END~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("{'a test statement';};",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~{~\n"
                "BEGIN~;~\n"
                "BEGIN~'~\n"
                "STRING='a test statement'\n"
                "END~'~\n"
                "END~;~\n"
                "END~{~\n"
                "END~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("{'a \\'test statement';};",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~{~\n"
                "BEGIN~;~\n"
                "BEGIN~'~\n"
                "STRING='a \\'test statement'\n"
                "END~'~\n"
                "END~;~\n"
                "END~{~\n"
                "END~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("{\"a \\\"test statement\";};",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~{~\n"
                "BEGIN~;~\n"
                "BEGIN~\"~\n"
                "STRING=\"a \\\"test statement\"\n"
                "END~\"~\n"
                "END~;~\n"
                "END~{~\n"
                "END~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("{'a \"test statement';};",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~{~\n"
                "BEGIN~;~\n"
                "BEGIN~'~\n"
                "STRING='a \"test statement'\n"
                "END~'~\n"
                "END~;~\n"
                "END~{~\n"
                "END~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("{\"a \\'test statement\";};",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~{~\n"
                "BEGIN~;~\n"
                "BEGIN~\"~\n"
                "STRING=\"a \\'test statement\"\n"
                "END~\"~\n"
                "END~;~\n"
                "END~{~\n"
                "END~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("{'a test } statement';};",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~{~\n"
                "BEGIN~;~\n"
                "BEGIN~'~\n"
                "STRING='a test } statement'\n"
                "END~'~\n"
                "END~;~\n"
                "END~{~\n"
                "END~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("{\"a test } statement\";};",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~{~\n"
                "BEGIN~;~\n"
                "BEGIN~\"~\n"
                "STRING=\"a test } statement\"\n"
                "END~\"~\n"
                "END~;~\n"
                "END~{~\n"
                "END~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("'a test { statement';",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~'~\n"
                "STRING='a test { statement'\n"
                "END~'~\n"
                "END~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("'a test } ] ; ) statement';",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~'~\n"
                "STRING='a test } ] ; ) statement'\n"
                "END~'~\n"
                "END~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("'a test statement;",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~'~\n"
                "STRING='a test statement;\n"
                "END_MISSING~'~\n"
                "END_MISSING~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("{'a test statement;};",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~{~\n"
                "BEGIN~;~\n"
                "BEGIN~'~\n"
                "STRING='a test statement;};\n"
                "END_MISSING~'~\n"
                "END_MISSING~;~\n"
                "END_MISSING~{~\n"
                "END_MISSING~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("\"a test statement;",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~\"~\n"
                "STRING=\"a test statement;\n"
                "END_MISSING~\"~\n"
                "END_MISSING~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("{\"a test statement;};",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~{~\n"
                "BEGIN~;~\n"
                "BEGIN~\"~\n"
                "STRING=\"a test statement;};\n"
                "END_MISSING~\"~\n"
                "END_MISSING~;~\n"
                "END_MISSING~{~\n"
                "END_MISSING~;~\n"
                "END~EOF~\n");
}
//-----------------------------------------------------------------------------
static void testLineComments()
{
  CHECK_RESULTS("//test statement\n",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~/~\n"
                "STRING=//test statement\n\n"
                "END~/~\n"
                "END_MISSING~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("//test statement\\\ny",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~/~\n"
                "STRING=//test statement\\\n\n"
                "END~/~\n"
                "STRING=y\n"
                "END_MISSING~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("//test statement",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~/~\n"
                "STRING=//test statement\n"
                "END_MISSING~/~\n"
                "END_MISSING~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("a; x//test statement\ny;",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "STRING=a\n"
                "END~;~\n"
                "BEGIN~;~\n"
                "STRING= x\n"
                "BEGIN~/~\n"
                "STRING=//test statement\n\n"
                "END~/~\n"
                "STRING=y\n"
                "END~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("a; x//test stat//ement\ny;",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "STRING=a\n"
                "END~;~\n"
                "BEGIN~;~\n"
                "STRING= x\n"
                "BEGIN~/~\n"
                "STRING=//test stat//ement\n\n"
                "END~/~\n"
                "STRING=y\n"
                "END~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("a; 2/x//test statement\ny;",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "STRING=a\n"
                "END~;~\n"
                "BEGIN~;~\n"
                "STRING= 2/x\n"
                "BEGIN~/~\n"
                "STRING=//test statement\n\n"
                "END~/~\n"
                "STRING=y\n"
                "END~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("a; 2/x/3 //test statement\ny;",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "STRING=a\n"
                "END~;~\n"
                "BEGIN~;~\n"
                "STRING= 2/x/3 \n"
                "BEGIN~/~\n"
                "STRING=//test statement\n\n"
                "END~/~\n"
                "STRING=y\n"
                "END~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("\"a; 2/x/3 //test statement\ny;\"",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~\"~\n"
                "STRING=\"a; 2/x/3 //test statement\ny;\"\n"
                "END~\"~\n"
                "END_MISSING~;~\n"
                "END~EOF~\n");
}
//-----------------------------------------------------------------------------
void testMultiLineComments()
{
  CHECK_RESULTS("/* test */",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~*~\n"
                "STRING=/* test */\n"
                "END~*~\n"
                "END_MISSING~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("/** test **/",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~*~\n"
                "STRING=/** test **/\n"
                "END~*~\n"
                "END_MISSING~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("/**\n* test\n**/;",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~*~\n"
                "STRING=/**\n* test\n**/\n"
                "END~*~\n"
                "END~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("/** /test **/",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~*~\n"
                "STRING=/** /test **/\n"
                "END~*~\n"
                "END_MISSING~;~\n"
                "END~EOF~\n");


  CHECK_RESULTS("/**/test **/x",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~*~\n"
                "STRING=/**/\n"
                "END~*~\n"
                "STRING=test **/x\n"
                "END_MISSING~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("/**/test **/{}",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~*~\n"
                "STRING=/**/\n"
                "END~*~\n"
                "STRING=test **/\n"
                "BEGIN~{~\n"
                "END~{~\n"
                "END_MISSING~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("/*/* test **/",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~*~\n"
                "STRING=/*/* test **/\n"
                "END~*~\n"
                "END_MISSING~;~\n"
                "END~EOF~\n");
}
//-----------------------------------------------------------------------------