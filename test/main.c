#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/SyntaxTree.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define TEST(METHOD) printf("[Running Test %s]\n", #METHOD); METHOD()

static String _assertEquals(const string a, const String b, const char* const file, const unsigned int line)
{
  if(strcmp(a, b.str) == 0)
    return b;

  printf(ANSI_COLOR_RED "[assertEquals] expected [%s] actual [%s] @ file: %s line: %i\n" ANSI_COLOR_RESET, a, b.str, file, line);
  exit(EXIT_FAILURE);
}

#define assertEquals(A, B) _assertEquals(A, B, __FILE__,__LINE__)


static void testCopyString();
static void testAppendString();
static void testInMemoryCharStream();
static void testTokenStreamEOF();
static void testTokenStream();

int main(const int argc, const char* const argv[])
{
  TEST(testCopyString);
  TEST(testAppendString);
  TEST(testInMemoryCharStream);
  TEST(testTokenStreamEOF);
  TEST(testTokenStream);
  return EXIT_SUCCESS;
}

static void testCopyString()
{
  {
    String s = toString("A test string");
    assertEquals("A test string", s);
    deleteString(s);
  }
  {
    String s = copyString(toString("A test string"), 5);
    assertEquals("A tes", s);
    deleteString(s);
  }
  {
    String s = copyString(toString("A test string"), 13);
    assertEquals("A test string", s);
    deleteString(s);
  }
}

static void testAppendString()
{
  String s = toString("A test string");
  s = appendChar(s, '!', 0);
  assertEquals("A test string!", s);
  s = appendChar(s, '-', 0);
  assertEquals("A test string!-", s);
}

static void testInMemoryCharStream()
{
  InMemoryCharStream istream = getInMemoryCharStream("Hello World!");
  CharStream* const stream = &(istream.stream);

  assert(NEXT(stream) == 'H');
  assert(NEXT(stream) == 'e');
  assert(NEXT(stream) == 'l');
  assert(NEXT(stream) == 'l');
  assert(NEXT(stream) == 'o');
  assert(NEXT(stream) == ' ');
  assert(NEXT(stream) == 'W');
  assert(NEXT(stream) == 'o');
  assert(NEXT(stream) == 'r');
  assert(NEXT(stream) == 'l');
  assert(NEXT(stream) == 'd');
  assert(NEXT(stream) == '!');
  assert(NEXT(stream) == END_STREAM);
  assert(NEXT(stream) == END_STREAM);
}

static void testTokenStreamEOF()
{
  {
    InMemoryCharStream istream = getInMemoryCharStream("a !! test stream");
    TokenStream stream = getTokenStream(&(istream.stream));
    String s;

    int i = 0;

    while((s = stream.next(&stream)).str[0] != END_STREAM) {
      deleteString(s);
      assert(i++ < 10);
    }
  }
}

static void testTokenStream()
{
  {
    InMemoryCharStream istream = getInMemoryCharStream("a test stream");
    TokenStream stream = getTokenStream(&(istream.stream));
    deleteString(assertEquals("a", stream.next(&stream)));
    deleteString(assertEquals("test", stream.next(&stream)));
    deleteString(assertEquals("stream", stream.next(&stream)));
    deleteString(assertEquals("", stream.next(&stream)));
    deleteString(assertEquals("", stream.next(&stream)));
  }
  {
    InMemoryCharStream istream = getInMemoryCharStream(" a $ test ^ stream }");
    TokenStream stream = getTokenStream(&(istream.stream));
    deleteString(assertEquals("a", stream.next(&stream)));
    deleteString(assertEquals("$", stream.next(&stream)));
    deleteString(assertEquals("test", stream.next(&stream)));
    deleteString(assertEquals("^", stream.next(&stream)));
    deleteString(assertEquals("stream", stream.next(&stream)));
    deleteString(assertEquals("}", stream.next(&stream)));
    deleteString(assertEquals("", stream.next(&stream)));
    deleteString(assertEquals("", stream.next(&stream)));
  }
  {
    InMemoryCharStream istream = getInMemoryCharStream(" a$test^\tstream\n\r} ");
    TokenStream stream = getTokenStream(&(istream.stream));
    deleteString(assertEquals("a", stream.next(&stream)));
    deleteString(assertEquals("$", stream.next(&stream)));
    deleteString(assertEquals("test", stream.next(&stream)));
    deleteString(assertEquals("^", stream.next(&stream)));
    deleteString(assertEquals("stream", stream.next(&stream)));
    deleteString(assertEquals("}", stream.next(&stream)));
    deleteString(assertEquals("", stream.next(&stream)));
    deleteString(assertEquals("", stream.next(&stream)));
  }
  {
    InMemoryCharStream istream = getInMemoryCharStream("ABC_DE");
    TokenStream stream = getTokenStream(&(istream.stream));
    deleteString(assertEquals("ABC_DE", stream.next(&stream)));
  }
}
