#ifndef __CODABAR_READER_H__
#define __CODABAR_READER_H__
#include "../BitArray.h"
int CodaBarDecodeRow(int rowNumber,BitArray *row,char *pResult,int *pResultLen,int *pType);
#endif