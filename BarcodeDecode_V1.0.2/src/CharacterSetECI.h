#ifndef __CHARACTERSETECI_H__
#define __CHARACTERSETECI_H__

#include "BaseTypeDef.h"

typedef struct _CharacterSetECI{
	int index;
	char *str[4];
}CharacterSetECI;

char const* CharacterSetECIGetName(CharacterSetECI *pECI);
int CharacterSetECIGetValue(CharacterSetECI *pECI);
CharacterSetECI* CharacterSetECIGetByValue(int val);

#endif