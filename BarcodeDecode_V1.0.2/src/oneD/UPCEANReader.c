#include "UPCEANReader.h"
#include "../DiyMalloc.h"
#include "../BarcodeFormat.h"
#include "UPCAReader.h"
#include "UPCEReader.h"

int UPCEANReaderFindStartGuardPattern(BitArray *row,OneDRange *pRange)
{
  BOOL foundStart = FALSE;
  int nextStart = 0,i,ret = 0,start,quietStart;
  int counters[START_END_PATTERN_LEN] = {0};

  while (!foundStart) {
    for(i=0; i < (int)START_END_PATTERN_LEN; ++i) {
      counters[i] = 0;
    }
    ret = UPCEANReaderFindGuardPatternB(row, nextStart, FALSE, (int *)START_END_PATTERN_, START_END_PATTERN_LEN,counters,START_END_PATTERN_LEN,pRange);
	if(ret < 0)
		return ret;
    start = pRange->data[0];
    nextStart = pRange->data[1];
    // Make sure there is a quiet zone at least as big as the start pattern before the barcode.
    // If this check would run off the left edge of the image, do not accept this barcode,
    // as it is very likely to be a false positive.
    quietStart = start - (nextStart - start);
    if (quietStart >= 0) {
      foundStart = BitArrayIsRange(row,quietStart, start, FALSE);//row->isRange(quietStart, start, false);
    }
  }
  return 0;
}

int UPCEANReaderFindGuardPatternB(BitArray *row,
								  int rowOffset,
								  BOOL whiteFirst,
								  int *pattern,int patternSize,
								  int *counters,int countersSize,
								  OneDRange *pRange)
{
	int patternLength = patternSize,counterPosition,patternStart,x,y;
	int width = BitArrayGetSize(row);//row->getSize();
	BOOL isWhite = whiteFirst;
	rowOffset = whiteFirst ? BitArrayGetNextUnset(row,rowOffset):BitArrayGetNextSet(row,rowOffset);
	counterPosition = 0;
	patternStart = rowOffset;
	for (x = rowOffset; x < width; x++) {
		if (BitArrayGet(row,x) ^ isWhite) {
			counters[counterPosition]++;
		} else {
			if (counterPosition == patternLength - 1) {
				if (OneDPatternMatchVariance(counters,countersSize, pattern, MAX_INDIVIDUAL_VARIANCE) < MAX_AVG_VARIANCE) {
					//return Range(patternStart, x);
					pRange->data[0] = patternStart;
					pRange->data[1] = x;
					return 0;
				}
				patternStart += counters[0] + counters[1];
				for (y = 2; y < patternLength; y++) {
					counters[y - 2] = counters[y];
				}
				counters[patternLength - 2] = 0;
				counters[patternLength - 1] = 0;
				counterPosition--;
			} else {
				counterPosition++;
			}
			counters[counterPosition] = 1;
			isWhite = !isWhite;
		}
	}
	return -1;//throw NotFoundException();
}

int UPCEANReaderFindGuardPattern(BitArray *row,
                                int rowOffset,
                                BOOL whiteFirst,
                                int *pattern,int patternSize,OneDRange *pRange)
{
  //vector<int> counters (pattern.size(), 0);
  int *counters = NULL;
  int ret = 0;

  MALLOC(counters,int,patternSize);
  ret = UPCEANReaderFindGuardPatternB(row, rowOffset, whiteFirst, pattern,patternSize,counters,patternSize,pRange);

ERROR_EXIT:
  FREE(counters);
  return ret;
}



int UPCEANReaderDecodeEnd(BitArray *row, int endStart,OneDRange *pRange)
{
    return UPCEANReaderFindGuardPattern(row, endStart, FALSE, (int *)START_END_PATTERN_,START_END_PATTERN_LEN,pRange);
}

int UPCEANReaderDecodeDigit(BitArray *row,
                            int *counters,int countersSize,
                            int rowOffset,
                            int **patterns,int patternsSize)
{
  int ret = 0,bestVariance,bestMatch,nmax,i,variance;
  int *pattern = NULL;

  ret = OneDRecordPattern(row, rowOffset, counters,countersSize);
  if(ret != 0) return ret;

  bestVariance = MAX_AVG_VARIANCE; // worst variance we'll accept
  bestMatch = -1;
  nmax = patternsSize;//patterns.size();
  for (i = 0; i < nmax; i++) {
    pattern =  &patterns[i][0];
    variance = OneDPatternMatchVariance(counters, countersSize,pattern, MAX_INDIVIDUAL_VARIANCE);
    if (variance < bestVariance) {
      bestVariance = variance;
      bestMatch = i;
    }
  }
  if (bestMatch >= 0) {
    return bestMatch;
  } else {
    //throw NotFoundException();
	return -1;
  }
}

/**
 * Computes the UPC/EAN checksum on a string of digits, and reports
 * whether the checksum is correct or not.
 *
 * @param s string of digits to check
 * @return true iff string of digits passes the UPC/EAN checksum algorithm
 */
BOOL UPCEANReaderCheckStandardUPCEANChecksum(char *s_,int *sSize_)
{
  //std::string const& s (s_->getText());
  char *s = s_;
  int sum,i,digit;
  int length = *sSize_;
  if (length == 0) {
    return FALSE;
  }

  sum = 0;
  for (i = length - 2; i >= 0; i -= 2) {
    digit = (int)s[i] - (int)'0';
    if (digit < 0 || digit > 9) {
      return FALSE;
    }
    sum += digit;
  }
  sum *= 3;
  for (i = length - 1; i >= 0; i -= 2) {
    int digit = (int) s[i] - (int) '0';
    if (digit < 0 || digit > 9) {
      return FALSE;
    }
    sum += digit;
  }
  return sum % 10 == 0;
}

/**
 * @return {@link #checkStandardUPCEANChecksum(String)}
 */
BOOL UPCEANReaderCheckChecksum(char *s_,int *sSize_)
{
  return UPCEANReaderCheckStandardUPCEANChecksum(s_,sSize_);
}


int UPCEANReaderDecodeRowB(int rowNumber,
                           BitArray *row,
                           OneDRange *startGuardRange,
						   char *pResult,int *pResultLen,int *pType)
{
	
	int ret = 0,end,quietEnd,endStart,i,nType = 0;
	OneDRange endRange;
	typedef int (*pDecodeMiddle)(BitArray *row,OneDRange *startRange,char *pResult,int *pResultLen);
	pDecodeMiddle DecodeMiddle = NULL;
	//BOOL ean13MayBeUPCA = FALSE;
    diy_memset(&endRange,0,sizeof(OneDRange));

	for(i=EAN_8;i<=UPC_E;i++)
	{
	   switch(i)
	   {
	   case EAN_8:
           DecodeMiddle = EAN8ReaderDecodeMiddle;
		   nType = EAN_8;
		   break;
	   case EAN_13:
		   DecodeMiddle = EAN13ReaderDecodeMiddle;
		   nType = EAN_13;
		   break;
	   case UPC_E:
		   DecodeMiddle = UPCEReaderDecodeMiddle;
		   nType = UPC_E;
		   break;
	   default:
		   DecodeMiddle = NULL;
		   break;

	   }
	   if(DecodeMiddle){
		   endStart = DecodeMiddle(row, startGuardRange,pResult,pResultLen);
		   if(endStart >= 0){ 
			   if(nType==EAN_13 && pResult[0]=='0'){
                     //ean13MayBeUPCA
				     nType = UPC_A;
				     UPCAReaderMaybeReturnResult(pResult,pResultLen);
			   }

			   diy_memset(&endRange,0,sizeof(OneDRange));
			   if(nType==UPC_E)
			   {
                   ret = UPCEReaderDecodeEnd(row,endStart,&endRange);
			   }else{
				   ret = UPCEANReaderDecodeEnd(row,endStart,&endRange);
			   }
			   if(ret < 0) continue;//return ret;
			   
			   // Make sure there is a quiet zone at least as big as the end pattern after the barcode.
			   // The spec might want more whitespace, but in practice this is the maximum we can count on.
			   end = endRange.data[1];
			   quietEnd = end + (end - endRange.data[0]);
			   if (quietEnd >= BitArrayGetSize(row) || !BitArrayIsRange(row,end, quietEnd,FALSE)) {
				   continue; //return RT_ERR_DECODEUPCEAN;
			   }
			   
			   if(nType==UPC_E)
               {
				   if (!UPCEReaderCheckChecksum(pResult,pResultLen)) {
					   continue; //return RT_ERR_CHECKSUM;
				   }
			   }else{
				   if (!UPCEANReaderCheckChecksum(pResult,pResultLen)) {
					   continue; //return RT_ERR_CHECKSUM;
				   }
			   }

			   *pType = nType;
			   break;
		   }
 	   }
	}
    

	
	return ret;
}


int UPCEANReaderDecodeRow(int rowNumber, BitArray *row,char *pResult,int *pResultLen,int *pType)
{
	OneDRange startGuardRange;
	int ret = 0;
    diy_memset(&startGuardRange,0,sizeof(OneDRange));
	
	ret = UPCEANReaderFindStartGuardPattern(row,&startGuardRange);
	if(ret == 0)
	{
		ret = UPCEANReaderDecodeRowB(rowNumber,row,&startGuardRange,pResult,pResultLen,pType);
	}
	
    return ret;
}

