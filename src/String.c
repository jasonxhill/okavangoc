#include <string.h>
#include "String.h"

String toString(const character* c)
{
  String s = newString(strlen(c));
  strcpy(s.str, c);
  return s;
}

String newString(const unsigned int size)
{
  String str = {
    .str = calloc(size + 1, sizeof(character)),
    .size = size
  };

  return str;
}

String copyString(const String str, const unsigned int size)
{
  const unsigned int len = strlen(str.str);

  String copy = {
    .str = strncpy(calloc(size, sizeof(character)), str.str, len < size? len : size),
    .size = size
  };

  return copy;
}

String concatString(const String a, const String b)
{
  String copy = copyString(a, strlen(a.str) + strlen(b.str));
  strcpy(copy.str + strlen(a.str), b.str);
  return copy;
}

String appendChar(String s, const character c, const unsigned int increaseBy)
{
  const unsigned int used = strlen(s.str);

  if(used < s.size)
  {
    s.str[used] = c;
    return s;
  }

  String copy = copyString(s, s.size + (increaseBy < 1? 1 : increaseBy));
  deleteString(s);
  copy.str[used] = c;
  return copy;
}

String joinStrings(const String a, const String b)
{
  String copy = concatString(a, b);
  deleteString(a);
  deleteString(b);
  return copy;
}

String trimString(String s)
{
  String copy = copyString(s, s.size);
  deleteString(s);
  return copy;
}

void deleteString(String s)
{
  free(s.str);
}
