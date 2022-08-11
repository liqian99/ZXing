#include "EAN8Reader.h"
#include "UPCEANReader.h"
#include "../DiyMalloc.h"

int EAN8ReaderDecodeMiddle(BitArray *row,
						   OneDRange *startRange,
						   char *result,int *resultLen)
{
	int countersSize = 4,endx;
	int counters[4] = {0};
	int end = BitArrayGetSize(row);
	int rowOffset = startRange->data[1];
	int x,bestMatch,i,qLen=0,ret=0;
    char *q = result;
	OneDRange middleRange;
    int **ppL_PATTERNS_ = NULL;

    MALLOC(ppL_PATTERNS_,int*,L_PATTERNS_LEN);
	for(i=0; i<L_PATTERNS_LEN; i++)
	{
		ppL_PATTERNS_[i] = (int *)L_PATTERNS_+i*4; 
	}

	for (x = 0; x < 4 && rowOffset < end; x++) {
		bestMatch =UPCEANReaderDecodeDigit(row, counters,4,rowOffset,(int **)ppL_PATTERNS_,L_PATTERNS_LEN);
		if(bestMatch < 0)
		{ 
			ret = RT_ERR_DECODEEAN8;
			goto ERROR_EXIT;
		}
		//result.append(1, (char) ('0' + bestMatch));
		*q++ = (char)('0' + bestMatch);
		qLen++;
		for (i = 0, endx = countersSize; i < endx; i++) {
			rowOffset += counters[i];
		}
	}
	
	diy_memset(&middleRange,0,sizeof(OneDRange));
	ret = UPCEANReaderFindGuardPattern(row, rowOffset, TRUE, (int *)MIDDLE_PATTERN_,MIDDLE_PATTERN_LEN,&middleRange);
	if(bestMatch < 0)
	{ 
		ret = RT_ERR_DECODEEAN8;
		goto ERROR_EXIT;
	}
	rowOffset = middleRange.data[1];
	for (x = 0; x < 4 && rowOffset < end; x++) {
		bestMatch = UPCEANReaderDecodeDigit(row, counters,4,rowOffset,(int **)ppL_PATTERNS_,L_PATTERNS_LEN);
		if(bestMatch < 0)
		{ 
			ret = RT_ERR_DECODEEAN8;
			goto ERROR_EXIT;
		}
		//result.append(1, (char) ('0' + bestMatch));
		*q++ = (char)('0' + bestMatch);
		qLen++;
		for (i = 0, endx = countersSize; i < endx; i++) {
			rowOffset += counters[i];
		}
	}
	ret = rowOffset;
    *resultLen = qLen;
ERROR_EXIT:
	FREE(ppL_PATTERNS_);
	return ret;
}