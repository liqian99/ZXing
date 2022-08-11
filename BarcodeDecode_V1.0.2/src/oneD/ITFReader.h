#ifndef __ITFREADER_H__
#define __ITFREADER_H__
#include "OneDReader.h"
int ITFReaderDecodeMiddle(BitArray *row,
                          int payloadStart,
                          int payloadEnd,
                          char *result,int *resultLen);

int ITFReaderDecodeRow(int rowNumber, BitArray *row,char *result,int *resultLen);


#endif