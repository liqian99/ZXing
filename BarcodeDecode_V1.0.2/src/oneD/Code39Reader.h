#ifndef __CODE39READER_H__
#define __CODE39READER_H__

#include "../BitArray.h"

int Code39DecodeRow(int rowNumber,BitArray *pRow,char *pResult,int *pResultLen,int *pType);

#endif