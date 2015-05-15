#ifndef SRC_STRING_H_
#define SRC_STRING_H_

#include "Util.h"

STRUCT {
  string str;
  unsigned long size;
} String;

String toString(const character*);
String newString(unsigned long size);
String copyString(const String, unsigned long size);
String concatString(const String, const String);
String appendChar(String, const character, unsigned long increaseBy);
String joinStrings(const String, const String);
String trimString(String);
void deleteString(String);

#endif /* SRC_STRING_H_ */
