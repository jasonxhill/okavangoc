#ifndef SRC_STRING_H_
#define SRC_STRING_H_

#include "Util.h"

STRUCT {
  string str;
  unsigned int size;
} String;

String toString(const character*);
String newString(unsigned int size);
String copyString(const String, unsigned int size);
String concatString(const String, const String);
String appendChar(String, const character, unsigned int increaseBy);
String joinStrings(const String, const String);
String trimString(String);
void deleteString(String);

#endif /* SRC_STRING_H_ */
