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
static void testBracketStreamStart(BracketVisitor* const visitor, CHAR type)
{
  TestBracketVisitor* v = (TestBracketVisitor*) visitor;

  if(type != END_STREAM)
  {
    String s = joinStrings(&(v->pool), v->str, StringOf("BEGIN~"));
    s = appendChar(&(v->pool), s, type, 1);
    v->str = joinStrings(&(v->pool), s, StringOf("~\n"));
  }
  else
    v->str = joinStrings(&(v->pool), v->str, StringOf("BEGIN~EOF~\n"));
}
//-----------------------------------------------------------------------------
static void testBracketStreamEnd(BracketVisitor* const visitor, CHAR type)
{
  TestBracketVisitor* v = (TestBracketVisitor*) visitor;

  if(type != END_STREAM)
  {
    String s = joinStrings(&(v->pool), v->str, StringOf("END~"));
    s = appendChar(&(v->pool), s, type, 1);
    v->str = joinStrings(&(v->pool), s, StringOf("~\n"));
  }
  else
    v->str = joinStrings(&(v->pool), v->str, StringOf("END~EOF~\n"));
}
//-----------------------------------------------------------------------------
static void testBracketStreamString(BracketVisitor* const visitor, String s)
{

}
//-----------------------------------------------------------------------------
static TestBracketVisitor newTestBracketVisitor()
{
  TestBracketVisitor visitor = {
    .self = {
      .visitBracketStart = &testBracketStreamStart,
      .visitBracketEnd = testBracketStreamEnd,
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
                "END~EOF~\n");

  CHECK_RESULTS("[]",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~]~\n"
                "END~]~\n"
                "END~EOF~\n");

  CHECK_RESULTS("()",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~)~\n"
                "END~)~\n"
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

  CHECK_RESULTS("{}{};",
                "BEGIN~EOF~\n"
                "BEGIN~;~\n"
                "BEGIN~}~\n"
                "END~}~\n"
                "BEGIN~}~\n"
                "END~}~\n"
                "END~;~\n"
                "END~EOF~\n");
}
//-----------------------------------------------------------------------------