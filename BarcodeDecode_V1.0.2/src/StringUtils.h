#ifndef __STRINGUTILS_H__
#define __STRINGUTILS_H__

#include "BaseTypeDef.h"
#include "DecodeHints.h"

const char *StringUtilsGuessEncoding(char *bytes,int length,DecodeHints *phints);

#endif