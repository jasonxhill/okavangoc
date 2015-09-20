//=============================================================================
// BracketStreamTest.c
//=============================================================================
#include "TestUtil.h"
#include "../src/BracketStream.h"
//-----------------------------------------------------------------------------
static void testBrackets();
//-----------------------------------------------------------------------------
void mainBracketStreamTests()
{
  TEST(testBrackets);
}
//-----------------------------------------------------------------------------

//=============================================================================
// TestBracketVisitor
//=============================================================================
STRUCT TestBracketVisitor {
  BracketVisitor self;
  String str;
  AutoReleasePool pool;
} TestBracketVisitor;
//-----------------------------------------------------------------------------
static void recordBracketVisit(BracketVisitor* const visitor, const CHAR type, const string visitType)
{
  TestBracketVisitor* const v = (TestBracketVisitor*) visitor;
  char sType[2] = {type, 0};
  const String typeString = StringOf(type != END_STREAM? sType : "EOF");

  String s = joinStrings(&(v->pool), v->str, StringOf(visitType));
  s = joinStrings(&(v->pool), s, StringOf("~"));
  s = joinStrings(&(v->pool), s, typeString);
  v->str = joinStrings(&(v->pool), s, StringOf("~\n"));
}
//-----------------------------------------------------------------------------
static void testBracketStreamStart(BracketVisitor* const visitor, const CHAR type)
{
  recordBracketVisit(visitor, type, "BEGIN");
}
//-----------------------------------------------------------------------------
static void testBracketStreamEnd(BracketVisitor* const visitor, const CHAR type)
{
  recordBracketVisit(visitor, type, "END");
}
//-----------------------------------------------------------------------------
static void testBracketStreamEndMissing(BracketVisitor* const visitor, const CHAR type)
{
  recordBracketVisit(visitor, type, "END_MISSING");
}
//-----------------------------------------------------------------------------
static void testBracketStreamString(BracketVisitor* const visitor, const String s)
{

}
//-----------------------------------------------------------------------------
static TestBracketVisitor newTestBracketVisitor()
{
  TestBracketVisitor visitor = {
    .self = {
      .visitBracketStart = &testBracketStreamStart,
      .visitBracketEnd = testBracketStreamEnd,
      .visitBracketEndMissing = &testBracketStreamEndMissing,
      .visitString = &testBracketStreamString
    },

    .str = StringOf(""),
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
  _assertEquals_string(expected, visitor.str.str, file, line);
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
                "BEGIN~}~\n"
                "END~}~\n"
                "END_MISSING~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("[]",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~]~\n"
                "END~]~\n"
                "END_MISSING~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("()",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~)~\n"
                "END~)~\n"
                "END_MISSING~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("{};",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~}~\n"
                "END~}~\n"
                "END~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("[];",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~]~\n"
                "END~]~\n"
                "END~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("();",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~)~\n"
                "END~)~\n"
                "END~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("(;);",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~)~\n"
                "BEGIN~;~\n"
                "END~;~\n"
                "END~)~\n"
                "END~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("(;",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~)~\n"
                "BEGIN~;~\n"
                "END~;~\n"
                "END_MISSING~)~\n"
                "END_MISSING~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("{};{};",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~}~\n"
                "END~}~\n"
                "END~;~\n"
                "BEGIN~;~\n"
                "BEGIN~}~\n"
                "END~}~\n"
                "END~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("{ };{ };",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~}~\n"
                "BEGIN~;~\n"
                "END_MISSING~;~\n"
                "END~}~\n"
                "END~;~\n"
                "BEGIN~;~\n"
                "BEGIN~}~\n"
                "BEGIN~;~\n"
                "END_MISSING~;~\n"
                "END~}~\n"
                "END~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("{}{};",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~}~\n"
                "END~}~\n"
                "BEGIN~}~\n"
                "END~}~\n"
                "END~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("{}{}",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~}~\n"
                "END~}~\n"
                "BEGIN~}~\n"
                "END~}~\n"
                "END_MISSING~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("{[();];};",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~}~\n"
                "BEGIN~;~\n"
                "BEGIN~]~\n"
                "BEGIN~;~\n"
                "BEGIN~)~\n"
                "END~)~\n"
                "END~;~\n"
                "END~]~\n"
                "END~;~\n"
                "END~}~\n"
                "END~;~\n"
                "END~EOF~\n");

  CHECK_RESULTS("{[();;};",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~}~\n"
                "BEGIN~;~\n"
                "BEGIN~]~\n"
                "BEGIN~;~\n"
                "BEGIN~)~\n"
                "END~)~\n"
                "END~;~\n"
                "BEGIN~;~\n"
                "END~;~\n"
                "END_MISSING~]~\n"
                "END_MISSING~;~\n"
                "END~}~\n"
                "END~;~\n"
                "END~EOF~\n");
}
//-----------------------------------------------------------------------------