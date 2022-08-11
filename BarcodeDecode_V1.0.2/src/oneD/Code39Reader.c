#include "Code39Reader.h"
#include "../DiyMalloc.h"
#include "OneDReader.h"
#include "../BarcodeFormat.h"

static const char* ALPHABET = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ-. *$/+%";

/**
* These represent the encodings of characters, as patterns of wide and narrow
* bars.
* The 9 least-significant bits of each int correspond to the pattern of wide
* and narrow, with 1s representing "wide" and 0s representing narrow.
*/
static const int CHARACTER_ENCODINGS_LEN = 44;
static int CHARACTER_ENCODINGS[] = {
    0x034, 0x121, 0x061, 0x160, 0x031, 0x130, 0x070, 0x025, 0x124, 0x064, // 0-9
		0x109, 0x049, 0x148, 0x019, 0x118, 0x058, 0x00D, 0x10C, 0x04C, 0x01C, // A-J
		0x103, 0x043, 0x142, 0x013, 0x112, 0x052, 0x007, 0x106, 0x046, 0x016, // K-T
		0x181, 0x0C1, 0x1C0, 0x091, 0x190, 0x0D0, 0x085, 0x184, 0x0C4, 0x094, // U-*
		0x0A8, 0x0A2, 0x08A, 0x02A // $-%
};

static int ASTERISK_ENCODING = 0x094;
static const char* ALPHABET_STRING = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ-. *$/+%";

static int Code39FindFirstOf(char *pSrc,int pSrcLen,char cPattern)
{
	int i=0;
	
    for (i=0;i<pSrcLen;i++)
    {
		if(pSrc[i] == cPattern)
			break;
    }
	
	return i;
}

static int Code39ReaderToNarrowWidePattern(int *counters,int nCountersSize)
{
	int numCounters = nCountersSize;
	int maxNarrowCounter = 0;
	int wideCounters,minCounter,i,counter,totalWideCountersWidth,pattern;
	
	do {
		minCounter = INT_MAX;
		for (i = 0; i < numCounters; i++) {
			counter = counters[i];
			if (counter < minCounter && counter > maxNarrowCounter) {
				minCounter = counter;
			}
		}
		maxNarrowCounter = minCounter;
		wideCounters = 0;
		totalWideCountersWidth = 0;
		pattern = 0;
		for (i = 0; i < numCounters; i++) {
			int counter = counters[i];
			if (counters[i] > maxNarrowCounter) {
				pattern |= 1 << (numCounters - 1 - i);
				wideCounters++;
				totalWideCountersWidth += counter;
			}
		}
		if (wideCounters == 3) {
			// Found 3 wide counters, but are they close enough in width?
			// We can perform a cheap, conservative check to see if any individual
			// counter is more than 1.5 times the average:
			for (i = 0; i < numCounters && wideCounters > 0; i++) {
				counter = counters[i];
				if (counters[i] > maxNarrowCounter) {
					wideCounters--;
					// totalWideCountersWidth = 3 * average, so this checks if
					// counter >= 3/2 * average.
					if ((counter << 1) >= totalWideCountersWidth) {
						return -1;
					}
				}
			}
			return pattern;
		}
	} while (wideCounters > 3);
	return -1;
}


static int Code39ReaderFindAsteriskPattern(BitArray *row, int *pCounters,int nCounterSize,OneDRange *pRange)
{
  int width = BitArrayGetSize(row);
  int rowOffset = BitArrayGetNextSet(row,0);
  int counterPosition = 0;
  int patternStart = rowOffset;
  BOOL isWhite = FALSE;
  int patternLength = nCounterSize,i,y;

  for (i = rowOffset; i < width; i++) {
    if (BitArrayGet(row,i) ^ isWhite) {
      pCounters[counterPosition]++;
    } else {
      if (counterPosition == patternLength - 1) {
        // Look for whitespace before start pattern, >= 50% of width of
        // start pattern.
        if (Code39ReaderToNarrowWidePattern(pCounters,nCounterSize) == ASTERISK_ENCODING &&
            BitArrayIsRange(row,MAX(0, patternStart - ((i - patternStart) >> 1)), patternStart, FALSE)) {
          pRange->data[0] = patternStart;
          pRange->data[1] = i;
          return 0;
        }
        patternStart += pCounters[0] + pCounters[1];
        for (y = 2; y < patternLength; y++) {
          pCounters[y - 2] = pCounters[y];
        }
        pCounters[patternLength - 2] = 0;
        pCounters[patternLength - 1] = 0;
        counterPosition--;
      } else {
        counterPosition++;
      }
      pCounters[counterPosition] = 1;
      isWhite = !isWhite;
    }
  }

  return -1;
}

// For efficiency, returns -1 on failure. Not throwing here saved as many as
// 700 exceptions per image when using some of our blackbox images.

static char Code39ReaderPatternToChar(int pattern)
{
  int i;
  for (i = 0; i < CHARACTER_ENCODINGS_LEN; i++) {
    if (CHARACTER_ENCODINGS[i] == pattern) {
      return ALPHABET[i];
    }
  }
  return 0;
}

static int Code39ReaderDecodeExtended(const char *pEncoded,int nEncodedLen,char *pResult,int *pResultLen)
{
  int length = nEncodedLen;
  int i,ret = 0;
  char c,next,decodedChar;
  char *q = pResult;
  int qLen = 0;

  for (i = 0; i < length; i++) {
    c = pEncoded[i];
    if (c == '+' || c == '$' || c == '%' || c == '/') {
      next = pEncoded[i + 1];
      decodedChar = '\0';
      switch (c) {
      case '+':
        // +A to +Z map to a to z
        if (next >= 'A' && next <= 'Z') {
          decodedChar = (char) (next + 32);
        } else {
           //throw ReaderException("");
			ret = RT_ERR_DECODE39;
			goto ERROR_EXIT;
        }
        break;
      case '$':
        // $A to $Z map to control codes SH to SB
        if (next >= 'A' && next <= 'Z') {
          decodedChar = (char) (next - 64);
        } else {
            //throw ReaderException("");
			ret = RT_ERR_DECODE39;
			goto ERROR_EXIT;
        }
        break;
      case '%':
        // %A to %E map to control codes ESC to US
        if (next >= 'A' && next <= 'E') {
          decodedChar = (char) (next - 38);
        } else if (next >= 'F' && next <= 'W') {
          decodedChar = (char) (next - 11);
        } else {
            //throw ReaderException("");
			ret = RT_ERR_DECODE39;
			goto ERROR_EXIT;
        }
        break;
      case '/':
        // /A to /O map to ! to , and /Z maps to :
        if (next >= 'A' && next <= 'O') {
          decodedChar = (char) (next - 32);
        } else if (next == 'Z') {
          decodedChar = ':';
        } else {
          //throw ReaderException("");
		  ret = RT_ERR_DECODE39;
		  goto ERROR_EXIT;
        }
        break;
      }
	  *q++ = decodedChar;
	  qLen++;
      // bump up i again since we read two characters
      i++;
    } else {
	  *q++ = c;
	  qLen++;
    }
  }

ERROR_EXIT:
  *pResultLen = qLen;
  return ret;
}

int Code39DecodeRow(int rowNumber,BitArray *pRow,char *pResult,int *pResultLen,int *pType)
{
   int theCounters[9] = {0};
   int size = 9,ret = 0,nextStart,end,lastStart,pattern,i,lastPatternSize,whiteSpaceAfterEnd,e,endC;
   OneDRange start;
   char decodedChar;
   #define MAXCHARLEN 128
   char tmpChar[MAXCHARLEN] = {0};
   char *q = tmpChar;
   int qLen = 0,nmax,total;


   diy_memset(&start,0,sizeof(OneDRange));
   ret = Code39ReaderFindAsteriskPattern(pRow,theCounters,size,&start);
   if(ret!=0) return ret;

   // Read off white space
   nextStart = BitArrayGetNextSet(pRow,start.data[1]);
   end = BitArrayGetSize(pRow); 

  do 
  {
    ret = OneDRecordPattern(pRow, nextStart, theCounters,size);
	if(ret != 0) {goto ERROR_EXIT;}
    pattern = Code39ReaderToNarrowWidePattern(theCounters,size);
    if (pattern < 0) {
      //throw NotFoundException();;
	   ret = RT_ERR_FOUNDPATTERN;
	   goto ERROR_EXIT;
    }
    decodedChar = Code39ReaderPatternToChar(pattern);
	if(decodedChar > 0)
	{
		//result.append(1, decodedChar);
		*q++ = decodedChar;
		qLen++;
		if(qLen >= MAXCHARLEN-1)
		{
			ret = RT_ERR_FOUNDPATTERN;
			goto ERROR_EXIT;
		}
		lastStart = nextStart;
		for (i = 0, endC=size; i < endC; i++) {
		  nextStart += theCounters[i];
		}
		// Read off white space
		nextStart = BitArrayGetNextSet(pRow,nextStart);
	}else{
		ret = RT_ERR_FOUNDPATTERN;
		goto ERROR_EXIT;
	}
  } while (decodedChar != '*');
  //result.resize(decodeRowResult.length()-1);// remove asterisk
  if(qLen<=1)
  {
	  ret = RT_ERR_DECODE39;
	  goto ERROR_EXIT;
  }
  tmpChar[--qLen] = 0;
  

  // Look for whitespace after pattern:
  lastPatternSize = 0;
  for (i = 0, e = size; i < e; i++) {
    lastPatternSize += theCounters[i];
  }
  whiteSpaceAfterEnd = nextStart - lastStart - lastPatternSize;
  // If 50% of last pattern size, following last pattern, is not whitespace,
  // fail (but if it's whitespace to the very end of the image, that's OK)
  if (nextStart != end && (whiteSpaceAfterEnd >> 1) < lastPatternSize) {
     //throw NotFoundException();
     ret = RT_ERR_DECODE39;
	 goto ERROR_EXIT;
  }

  if (FALSE/*usingCheckDigit*/)
  {
     nmax = qLen - 1;
     total = 0;
     for (i = 0; i < nmax; i++) {
      total += Code39FindFirstOf((char *)ALPHABET_STRING,CHARACTER_ENCODINGS_LEN,tmpChar[i]);//alphabet_string.find_first_of(decodeRowResult[i], 0);
    }
    if (tmpChar[nmax] != ALPHABET[total % 43]) {
        //throw ChecksumException();
		ret = RT_ERR_DECODE39;
		goto ERROR_EXIT;

    }
    //result.resize(max);
	qLen = nmax;
	tmpChar[qLen-1] = 0;
  }
  
  if (qLen == 0) {
    // Almost false positive
    ret = RT_ERR_DECODE39;
    goto ERROR_EXIT;
  }
  
  if (FALSE/*extendedMode*/) {
    ret = Code39ReaderDecodeExtended(tmpChar,qLen,pResult,pResultLen);
  } else {
	diy_memcpy(pResult,tmpChar,qLen);
	*pResultLen = qLen;
  }

  *pType = CODE_39;

ERROR_EXIT:
  return ret;
}
