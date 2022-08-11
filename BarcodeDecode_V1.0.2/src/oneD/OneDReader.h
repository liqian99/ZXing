#ifndef __ONEDR_READER_H__
#define __ONEDR_READER_H__

#include "../BitArray.h"

#define INTEGER_MATH_SHIFT 8

#define PATTERN_MATCH_RESULT_SCALE_FACTOR  (1<<INTEGER_MATH_SHIFT)

typedef struct __OneDRange
{
    int data[3];
}OneDRange;

int OneDRecordPattern(BitArray *pRow,int start,int *pCounters,int nCountersSize);
int OneDReaderDoDecode(unsigned char *pImgGray,int w,int h,char *pResult,int *pResultLen,int *pType); 
int OneDPatternMatchVariance(int *counters, int countersSize,
                             int *pattern,
                             int maxIndividualVariance);


#endif /*__ONEDR_READER_H__*/