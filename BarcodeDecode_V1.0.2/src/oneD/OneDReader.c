#include "../BarcodeFormat.h"
#include "../GlobalHistogramBinarizer.h"
#include "OneDReader.h"
#include "Code93Reader.h"
#include "Code39Reader.h"
#include "Code128Reader.h"
#include "CodaBarReader.h"
#include "UPCEANReader.h"

int OneDRecordPattern(BitArray *pRow,int start,int *pCounters,int nCountersSize)
{
    int numCounters,i,end,counterPosition;
    int ret;
    BOOL isWhite;

	numCounters = nCountersSize;
	for (i=0;i<numCounters;i++)
		pCounters[i] = 0;
    
	end = BitArrayGetSize(pRow);
	if(start >= end)
		return RT_ERR_PARAFIT;

    isWhite = !BitArrayGet(pRow,start);
	counterPosition = 0;
	i = start;
	while(i<end)
	{
		if(BitArrayGet(pRow,i)^isWhite)
		{
			pCounters[counterPosition]++;
		}else{
            counterPosition++;
			if(counterPosition == numCounters)
				break;
			else{
				pCounters[counterPosition] = 1;
				isWhite = !isWhite;
			}
		}
		i++;
	}

	if(!(counterPosition == numCounters || (counterPosition==numCounters-1 && i==end)))
		ret = RT_ERR_FOUNDPATTERN;
	else
		ret = 0;

	return ret;
}

int OneDPatternMatchVarianceB(int *counters,int countersSize,
                              int *pattern,
                              int maxIndividualVariance)
{
	int numCounters = countersSize,i,unitBarWidth,totalVariance,x,counter,scaledPattern,variance;
	unsigned int total = 0;
	unsigned int patternLength = 0;
	for (i = 0; i < numCounters; i++) {
		total += counters[i];
		patternLength += pattern[i];
	}
	if (total < patternLength) {
		// If we don't even have one pixel per unit of bar width, assume this is too small
		// to reliably match, so fail:
		return INT_MAX;
	}
	// We're going to fake floating-point math in integers. We just need to use more bits.
	// Scale up patternLength so that intermediate values below like scaledCounter will have
	// more "significant digits"
	unitBarWidth = (total << INTEGER_MATH_SHIFT) / patternLength;
	maxIndividualVariance = (maxIndividualVariance * unitBarWidth) >> INTEGER_MATH_SHIFT;
	
	totalVariance = 0;
	for (x = 0; x < numCounters; x++) {
		counter = counters[x] << INTEGER_MATH_SHIFT;
		scaledPattern = pattern[x] * unitBarWidth;
		variance = counter > scaledPattern ? counter - scaledPattern : scaledPattern - counter;
		if (variance > maxIndividualVariance) {
			return INT_MAX;
		}
		totalVariance += variance;
	}
	return totalVariance / total;
}

int OneDPatternMatchVariance(int *counters, int countersSize,
                             int *pattern,
                             int maxIndividualVariance)
{
	return OneDPatternMatchVarianceB(counters,countersSize,&pattern[0], maxIndividualVariance);
}


static int OneDDecodeRow(int rowNumber,BitArray *pRow,char *pResult,int *pResultLen,int *pType)
{
    typedef int (*pOneDDecodeRow)(int rowNumber,BitArray *pRow,char *pResult,int *pResultLen,int *pType);
    pOneDDecodeRow oneDDecodeRow = NULL;
    int ret = RT_ERR_DECODEONED;
    int i = 0;

	for (i=CODABAR;i<=EAN_13;i++)
	{
		switch(i)
		{
		    case CODABAR:
                oneDDecodeRow = CodaBarDecodeRow;
				break;
			case CODE_39:
				oneDDecodeRow = Code39DecodeRow;
				break;
			case CODE_93:
				oneDDecodeRow = Code93DecodeRow;
				break;
			case CODE_128:
				oneDDecodeRow = Code128DecodeRow;
				break;
			case EAN_13:
			case EAN_8:
				oneDDecodeRow = UPCEANReaderDecodeRow;
				break;
			default:
				break;
		}
		if(oneDDecodeRow){
			ret = oneDDecodeRow(rowNumber,pRow,pResult,pResultLen,pType);
			if(ret >= 0)
				break;
		}
		oneDDecodeRow = NULL;
	}

    return ret;
}


int OneDReaderDoDecode(unsigned char *pImgGray,int w,int h,char *pResult,int *pResultLen,int *pType) 
{
    int ret = RT_ERR_DECODEONED;
	BitArray row;
	int middle = h >> 1;
	BOOL tryHarder = FALSE,isAbove;
	int rowStep = MAX(1, h >> (tryHarder ? 8 : 5));   
	int maxLines,x,rowStepsAboveOrBelow,rowNumber,attempt;

    ret = BitArrayInit(&row,w);
    if(ret != 0)
	    return ret;

	if (tryHarder) {
		maxLines = h; // Look at the whole image, not just the center
	} else {
		maxLines = 15; // 15 rows spaced 1/32 apart is roughly the middle half of the image
	}
	
	for (x = 0; x < maxLines; x++) {
		
		// Scanning from the middle out. Determine which row we're looking at next:
		rowStepsAboveOrBelow = (x + 1) >> 1;
		isAbove = (x & 0x01) == 0; // i.e. is x even?
		rowNumber = middle + rowStep * (isAbove ? rowStepsAboveOrBelow : -rowStepsAboveOrBelow);

		if (rowNumber < 0 || rowNumber >= h) {
			// Oops, if we run off the top or bottom, stop
			break;
		}
		
		// Estimate black point for this row and load it:
		ret = GlobalHistogramBinarizerGetBlackRow(pImgGray,w,h,rowNumber,&row);
		if(ret != 0) goto ERROR_EXIT;

		// While we have the image data in a BitArray, it's fairly cheap to reverse it in place to
		// handle decoding upside down barcodes.
		for (attempt = 0; attempt < 2; attempt++)
		{
			if (attempt == 1) {
				BitArrayReverse(&row);; // reverse the row and continue
			}
			
			// Java hints stuff missing
	        ret = OneDDecodeRow(rowNumber,&row,pResult,pResultLen,pType);
			if(ret>=0)
				goto ERROR_EXIT;
		
		}
	}

ERROR_EXIT:
	BitArrayUninit(&row);
	return ret;
}