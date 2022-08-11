#include "UPCEReader.h"
#include "UPCEANReader.h"
#include "../DiyMalloc.h"

static const int MIDDLE_END_PATTERN_[6] = {1, 1, 1, 1, 1, 1};
//   const vector<int> MIDDLE_END_PATTERN (VECTOR_INIT(MIDDLE_END_PATTERN_));
  

  /**
   * See {@link #L_AND_G_PATTERNS}; these values similarly represent patterns of
   * even-odd parity encodings of digits that imply both the number system (0 or 1)
   * used, and the check digit.
   */
static const int NUMSYS_AND_CHECK_DIGIT_PATTERNS[2][10] = {
    {0x38, 0x34, 0x32, 0x31, 0x2C, 0x26, 0x23, 0x2A, 0x29, 0x25},
    {0x07, 0x0B, 0x0D, 0x0E, 0x13, 0x19, 0x1C, 0x15, 0x16, 0x1A}
  };


int UPCEReaderDecodeEnd(BitArray *row, int endStart,OneDRange *pRange) 
{
  return UPCEANReaderFindGuardPattern(row, endStart, TRUE,(int *)MIDDLE_END_PATTERN_,6,pRange);
}

/**
 * Expands a UPC-E value back into its full, equivalent UPC-A code value.
 *
 * @param upce UPC-E code as string of digits
 * @return equivalent UPC-A code as string of digits
 */
int UPCEReaderConvertUPCEtoUPCA(char *upce,int *upceLen,char *out,int *outLen)
{
  int len = *upceLen;
  char *tmp = out;
  int tmpLen = 0;
  char lastChar;
  if(len < 8 || len > 15)
  {
	  return -1;
  }
  tmp[tmpLen++] = upce[0];
  lastChar = upce[6];
  switch (lastChar) {
  case '0':
  case '1':
  case '2':
//     result.append(upce.substr(1,2));
//     result.append(1, lastChar);
//     result.append("0000");
//     result.append(upce.substr(3,3));
	tmp[tmpLen++] = upce[1];
	tmp[tmpLen++] = upce[2];
    tmp[tmpLen++] = upce[lastChar];
    tmp[tmpLen++] = '0';
	tmp[tmpLen++] = '0';
	tmp[tmpLen++] = '0';
	tmp[tmpLen++] = '0';
    tmp[tmpLen++] = upce[3];
	tmp[tmpLen++] = upce[4];
	tmp[tmpLen++] = upce[5];
    break;
  case '3':
//     result.append(upce.substr(1,3));
//     result.append("00000");
//     result.append(upce.substr(4,2));
	  tmp[tmpLen++] = upce[1];
	  tmp[tmpLen++] = upce[2];
	  tmp[tmpLen++] = upce[3];
	  tmp[tmpLen++] = '0';
	  tmp[tmpLen++] = '0';
	  tmp[tmpLen++] = '0';
	  tmp[tmpLen++] = '0';
	  tmp[tmpLen++] = '0';
	  tmp[tmpLen++] = upce[4];
	  tmp[tmpLen++] = upce[5];
    break;
  case '4':
//     result.append(upce.substr(1,4));
//     result.append("00000");
//     result.append(1, upce[5]);
	  tmp[tmpLen++] = upce[1];
	  tmp[tmpLen++] = upce[2];
	  tmp[tmpLen++] = upce[3];
	  tmp[tmpLen++] = upce[4];
	  tmp[tmpLen++] = '0';
	  tmp[tmpLen++] = '0';
	  tmp[tmpLen++] = '0';
	  tmp[tmpLen++] = '0';
	  tmp[tmpLen++] = '0';
	  tmp[tmpLen++] = upce[5];
    break;
  default:
//     result.append(upce.substr(1,5));
//     result.append("0000");
//     result.append(1, lastChar);
	  tmp[tmpLen++] = upce[1];
	  tmp[tmpLen++] = upce[2];
	  tmp[tmpLen++] = upce[3];
	  tmp[tmpLen++] = upce[4];
	  tmp[tmpLen++] = upce[5];
	  tmp[tmpLen++] = '0';
	  tmp[tmpLen++] = '0';
	  tmp[tmpLen++] = '0';
	  tmp[tmpLen++] = '0';
	  tmp[tmpLen++] = lastChar;
    break;
  }

   tmp[tmpLen++] = upce[7];

   *outLen = tmpLen;
   return 0;
}


BOOL UPCEReaderCheckChecksum(char *s,int *sLen)
{
  char out[32] = {0};
  int outLen = 0;

  int ret = UPCEReaderConvertUPCEtoUPCA(s,sLen,out,&outLen);
  if(ret == 0){
       return UPCEANReaderCheckChecksum(out,&outLen);
  }

  return FALSE;
}


BOOL UPCEReaderDetermineNumSysAndCheckDigit(char *resultString,int *resultStringLen, int lgPatternFound)
{
  int d,numSys,len,i;

  len = *resultStringLen;
  for (numSys = 0; numSys <= 1; numSys++) {
    for (d = 0; d < 10; d++) {
      if (lgPatternFound == NUMSYS_AND_CHECK_DIGIT_PATTERNS[numSys][d]) {
//         resultString.insert(0, 1, (char) ('0' + numSys));
//         resultString.append(1, (char) ('0' + d));
        for (i=len;i>0;i--)
        {
            resultString[i] = resultString[i-1];
        }
        resultString[0] = (char)('0' + numSys);
		len++;
        resultString[len] = (char) ('0' + d);
		len++;
        *resultStringLen = len;
        return TRUE;
      }
    }
  }
  return FALSE;
}


int UPCEReaderDecodeMiddle(BitArray *row, OneDRange *startRange, char *result,int *resultLen)
{
	int counters[4] = {0};
	int countersSize = 4;
    int end = BitArrayGetSize(row);
    int rowOffset = startRange->data[1];
    int lgPatternFound = 0,x,bestMatch,i,ret=0,qLen=0,e;
    int **ppL_AND_G_PATTERNS_ = NULL;
	char *q = result;
	
	
	MALLOC(ppL_AND_G_PATTERNS_,int*,L_AND_G_PATTERNS_LEN)
		for(i=0; i<L_AND_G_PATTERNS_LEN; i++)
		{
			ppL_AND_G_PATTERNS_[i] = (int *)L_AND_G_PATTERNS_+i*4; 
		}
		
		for (x = 0; x < 6 && rowOffset < end; x++) {
			bestMatch = UPCEANReaderDecodeDigit(row, counters,4, rowOffset,(int **)ppL_AND_G_PATTERNS_,L_AND_G_PATTERNS_LEN);
			if(bestMatch < 0){
				ret = RT_ERR_DECODEUPCE;
				goto ERROR_EXIT;
			}
			//result.append(1, (char) ('0' + bestMatch % 10));
			*q++ = (char) ('0' + bestMatch % 10);
			qLen++;
			for (i = 0, e = countersSize; i < e; i++) {
				rowOffset += counters[i];
			}
			if (bestMatch >= 10) {
				lgPatternFound |= 1 << (5 - x);
			}
		}
		
		UPCEReaderDetermineNumSysAndCheckDigit(result,&qLen,lgPatternFound);
		
		ret = rowOffset;
		*resultLen = qLen;
ERROR_EXIT:
		FREE(ppL_AND_G_PATTERNS_);
		return ret;
}