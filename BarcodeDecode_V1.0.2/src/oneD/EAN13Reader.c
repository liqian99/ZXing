#include "EAN13Reader.h"
#include "UPCEANReader.h"
#include "../DiyMalloc.h"

static const int FIRST_DIGIT_ENCODINGS[10] = {
    0x00, 0x0B, 0x0D, 0xE, 0x13, 0x19, 0x1C, 0x15, 0x16, 0x1A
};


// EAN13Reader::EAN13Reader() : decodeMiddleCounters(4, 0) { }

int EAN13ReaderDetermineFirstDigit(char *resultString, int *resultStringLen, int lgPatternFound)
{
    int d,i=0,len=*resultStringLen;
	for ( d = 0; d < 10; d++) {
		if (lgPatternFound == FIRST_DIGIT_ENCODINGS[d]) {
			//resultString.insert(0, 1, (char) ('0' + d));
			for (i=len;i>0;i--)
			{
                resultString[i] = resultString[i-1];
			}
			resultString[0] = (char)('0' + d);
			len++;
            *resultStringLen = len;
			return 0;
		}
	}
	return -1;
}

int EAN13ReaderDecodeMiddle(BitArray *row,
                            OneDRange *startRange,
                            char *pResult,int *pResultLen) 
{
    int counters[4] = {0},endx;
	char *q = pResult;
	int qLen = 0,ret = 0;
    OneDRange middleRange;
	int end = BitArrayGetSize(row);
	int rowOffset = startRange->data[1];
	int lgPatternFound = 0,x,bestMatch,i;
	int **ppL_PATTERNS_ = NULL;
	int **ppL_AND_G_PATTERNS_ = NULL;

	MALLOC(ppL_PATTERNS_,int*,L_PATTERNS_LEN);
    for(i=0; i<L_PATTERNS_LEN; i++)
	{
	   ppL_PATTERNS_[i] = (int *)L_PATTERNS_+i*4; 
	}
    MALLOC(ppL_AND_G_PATTERNS_,int*,L_AND_G_PATTERNS_LEN)
	for(i=0; i<L_AND_G_PATTERNS_LEN; i++)
	{
	   ppL_AND_G_PATTERNS_[i] = (int *)L_AND_G_PATTERNS_+i*4; 
	}


	diy_memset(&middleRange,0,sizeof(OneDRange));
	for (x = 0; x < 6 && rowOffset < end; x++) {
		bestMatch = UPCEANReaderDecodeDigit(row, counters,4,rowOffset, (int **)ppL_AND_G_PATTERNS_,L_AND_G_PATTERNS_LEN);
		if(bestMatch < 0){
			ret = RT_ERR_DECODEEAN13;
            goto ERROR_EXIT;
		}
		*q++ = (char)('0' + bestMatch % 10);
		qLen++;
		for (i = 0, endx = 4; i <endx; i++) {
			rowOffset += counters[i];
		}
		if (bestMatch >= 10) {
			lgPatternFound |= 1 << (5 - x);
		}
	}
	
    

	ret = EAN13ReaderDetermineFirstDigit(pResult,&qLen,lgPatternFound);
	if(ret < 0) {ret = RT_ERR_DECODEEAN13; goto ERROR_EXIT;}
	q++;
	
	ret = UPCEANReaderFindGuardPattern(row, rowOffset, TRUE, (int *)MIDDLE_PATTERN_,MIDDLE_PATTERN_LEN,&middleRange);
	if(ret < 0) {ret = RT_ERR_DECODEEAN13; goto ERROR_EXIT;}
	rowOffset = middleRange.data[1];
	
	for (x = 0; x < 6 && rowOffset < end; x++) {
		bestMatch =	UPCEANReaderDecodeDigit(row,counters,4,rowOffset,(int **)ppL_PATTERNS_,L_PATTERNS_LEN);
		if(bestMatch < 0){
			ret = RT_ERR_DECODEEAN13;
		    goto ERROR_EXIT;
		}
		//resultString.append(1, (char) ('0' + bestMatch));
		*q++ = (char)('0' + bestMatch);
		qLen++;
		for (i = 0, endx = 4; i < endx; i++) {
			rowOffset += counters[i];
		}
	}
	ret =  rowOffset;
    *pResultLen = qLen;
ERROR_EXIT:
	FREE(ppL_PATTERNS_);
	FREE(ppL_AND_G_PATTERNS_);
	return ret;
}

