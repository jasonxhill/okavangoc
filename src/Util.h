#ifndef SRC_UTIL_H_
#define SRC_UTIL_H_

#include <stdlib.h>

#ifndef BOOL
#define BOOL int
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define STRUCT typedef struct
typedef char character;
typedef character* string;

#define INVOKE(OBJECT, FUNCTION) OBJECT->FUNCTION(OBJECT)

#endif /* SRC_UTIL_H_ */
