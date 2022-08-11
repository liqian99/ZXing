#ifndef __CODE128READER_H__
#define __CODE128READER_H__
#include "../BitArray.h"
int Code128DecodeRow(int rowNumber,BitArray *row,char *pResult,int *pResultLen,int *pType);
#endif