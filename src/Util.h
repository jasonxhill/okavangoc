#ifndef SRC_UTIL_H_
#define SRC_UTIL_H_

#include <stdlib.h>

#define STRUCT typedef struct
typedef char character;
typedef character* string;

#define INVOKE(OBJECT, FUNCTION) OBJECT->FUNCTION(OBJECT)

#endif /* SRC_UTIL_H_ */
