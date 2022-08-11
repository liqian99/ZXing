#ifndef __CODE_93_READER_H__
#define __CODE_93_READER_H__
#include "../BitArray.h"

int Code93DecodeRow(int rowNumber,BitArray *pRow,char *pResult,int *pResultLen,int *pType);

#endif