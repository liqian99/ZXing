#include "CodaBarReader.h"
#include "../BaseTypeDef.h"
#include "../DiyMalloc.h"
#include "OneDReader.h"
#include "../BarcodeFormat.h"

static char const ALPHABET_STRING[] = "0123456789-$:/.+ABCD";
static char const* const ALPHABET = ALPHABET_STRING;
static const int CHARACTER_ENCODINGS[] = {
    0x003, 0x006, 0x009, 0x060, 0x012, 0x042, 0x021, 0x024, 0x030, 0x048, // 0-9
    0x00c, 0x018, 0x045, 0x051, 0x054, 0x015, 0x01A, 0x029, 0x00B, 0x00E, // -$:/.+ABCD
  };
static const int MIN_CHARACTER_LENGTH = 3;
static const char STARTEND_ENCODING[] = {'A', 'B', 'C', 'D', 0};

#define MAX_ACCEPTABLE (PATTERN_MATCH_RESULT_SCALE_FACTOR * 2.0f)
#define PADDING (PATTERN_MATCH_RESULT_SCALE_FACTOR * 1.5f)

static int CodaBarReaderCounterAppend(int e,int counterLength,int *counters,int countersSize)
{
  int counterLengthTmp = counterLength;
  if (counterLengthTmp < (int)countersSize) {
    counters[counterLengthTmp] = e;
  } else {
    //counters.push_back(e);
	return -1;  //超出不再增加
  }
  counterLengthTmp++;

  return counterLengthTmp;
}

static BOOL CodaBarReaderArrayContains(char const array[], char key)
{
	return strchr(array, key) != 0;
}


static int CodaBarReaderToNarrowWidePattern(int position,int counterLength,int *counters,int countersSize)
{
	int maxBar,minBar,i,j,currentCounter,thresholdBar,maxSpace,minSpace,thresholdSpace,bitmask,pattern,threshold;
	int *theCounters = counters;
	int end = position + 7;
	if (end >= counterLength) {
		return -1;
	}
	
	
	maxBar = 0;
	minBar = INT_MAX;
	for (j = position; j < end; j += 2) {
		currentCounter = theCounters[j];
		if (currentCounter < minBar) {
			minBar = currentCounter;
		}
		if (currentCounter > maxBar) {
			maxBar = currentCounter;
		}
	}
	thresholdBar = (minBar + maxBar) / 2;
	
	maxSpace = 0;
	minSpace = INT_MAX;
	for (j = position + 1; j < end; j += 2) {
		currentCounter = theCounters[j];
		if (currentCounter < minSpace) {
			minSpace = currentCounter;
		}
		if (currentCounter > maxSpace) {
			maxSpace = currentCounter;
		}
	}
	thresholdSpace = (minSpace + maxSpace) / 2;
	
	bitmask = 1 << 7;
	pattern = 0;
	for (i = 0; i < 7; i++) {
		threshold = (i & 1) == 0 ? thresholdBar : thresholdSpace;
		bitmask >>= 1;
		if (theCounters[position + i] > threshold) {
			pattern |= bitmask;
		}
	}
	
	for (i = 0; i < ZXING_ARRAY_LEN(CHARACTER_ENCODINGS); i++) {
		if (CHARACTER_ENCODINGS[i] == pattern) {
			return i;
		}
	}
	return -1;
}


static int CodaBarReaderFindStartPattern(int counterLength,int *counters,int countersSize) 
{
  int i,charOffset,j,patternSize;

  for (i = 1; i < counterLength; i += 2) {
    charOffset = CodaBarReaderToNarrowWidePattern(i,counterLength,counters,countersSize);
    if (charOffset != -1 && CodaBarReaderArrayContains(STARTEND_ENCODING, ALPHABET[charOffset])) {
      // Look for whitespace before start pattern, >= 50% of width of start pattern
      // We make an exception if the whitespace is the first element.
      patternSize = 0;
      for (j = i; j < i + 7; j++) {
        patternSize += counters[j];
      }
      if (i == 1 || counters[i-1] >= patternSize / 2) {
        return i;
      }
    }
  }
  //throw NotFoundException();
  return -1;
}

/**
 * Records the size of all runs of white and black pixels, starting with white.
 * This is just like recordPattern, except it records all the counters, and
 * uses our builtin "counters" member for storage.
 * @param row row to count from
 */
static int CodaBarReaderSetCounters(BitArray *row,int *counters,int countersSize) 
{
  int i,end,count;
  BOOL isWhite;
  int counterLength = 0;
  // Start from the first white bit.
  i = BitArrayGetNextUnset(row,0);  //row->getNextUnset(0);
  end = BitArrayGetSize(row);//row->getSize();
  if (i >= end) {
    return -1;  ///throw NotFoundException();
  }
  isWhite = TRUE;
  count = 0;
  for (; i < end; i++) {
    if (BitArrayGet(row,i) ^ isWhite) { // that is, exactly one is true
      count++;
    } else {
      counterLength = CodaBarReaderCounterAppend(count,counterLength,counters,countersSize);
	  if(counterLength < 0)
		  return -2;
      count = 1;
      isWhite = !isWhite;
    }
  }
  counterLength = CodaBarReaderCounterAppend(count,counterLength,counters,countersSize);
  if(counterLength < 0)
	  return -2;

  return counterLength;
}

static int CodaBarReaderValidatePattern(int start,char *decodeRowResult,int decodeRowResultSize,int *counters,int countersSize)
{
	// First, sum up the total size of our four categories of stripe sizes;
	//   vector<int> sizes (4, 0);
	//   vector<int> counts (4, 0);
	int sizes[4] = {0};
	int counts[4] = {0};
	int maxes[4] = {0};
	int mins[4] = {0};
	int end = decodeRowResultSize - 1; //decodeRowResult.length() - 1;
	// We break out of this loop in the middle, in order to handle
	// inter-character spaces properly.
	int pos = start,i,j,pattern,category,size;
	for (i = 0;TRUE; i++) {
		pattern = CHARACTER_ENCODINGS[(int)decodeRowResult[i]];
		for (j = 6; j >= 0; j--) {
			// Even j = bars, while odd j = spaces. Categories 2 and 3 are for
			// long stripes, while 0 and 1 are for short stripes.
			category = (j & 1) + (pattern & 1) * 2;
			sizes[category] += counters[pos + j];
			counts[category]++;
			pattern >>= 1;
		}
		if (i >= end) {
			break;
		}
		// We ignore the inter-character space - it could be of any size.
		pos += 8;
	}
	
	// Calculate our allowable size thresholds using fixed-point math.
	//vector<int> maxes (4, 0);
	//vector<int> mins (4, 0);
	// Define the threshold of acceptability to be the midpoint between the
	// average small stripe and the average large stripe. No stripe lengths
	// should be on the "wrong" side of that line.
	for (i = 0; i < 2; i++) {
		mins[i] = 0;  // Accept arbitrarily small "short" stripes.
		mins[i + 2] = ((sizes[i] << INTEGER_MATH_SHIFT) / counts[i] +
			(sizes[i + 2] << INTEGER_MATH_SHIFT) / counts[i + 2]) >> 1;
		maxes[i] = mins[i + 2];
		maxes[i + 2] = (int)(sizes[i + 2] * MAX_ACCEPTABLE + PADDING) / counts[i + 2];
	}
	
	// Now verify that all of the stripes are within the thresholds.
	pos = start;
	for (i = 0; TRUE; i++) {
		pattern = CHARACTER_ENCODINGS[(int)decodeRowResult[i]];
		for (j = 6; j >= 0; j--) {
			// Even j = bars, while odd j = spaces. Categories 2 and 3 are for
			// long stripes, while 0 and 1 are for short stripes.
			category = (j & 1) + (pattern & 1) * 2;
			size = counters[pos + j] << INTEGER_MATH_SHIFT;
			if (size < mins[category] || size > maxes[category]) {
				//throw NotFoundException();
				return -1;
			}
			pattern >>= 1;
		}
		if (i >= end) {
			break;
		}
		pos += 8;
	}
	
	return 0;
}

#define MAX_COUNTERS_SIZE  512
int CodaBarDecodeRow(int rowNumber,BitArray *row,char *pResult,int *pResultLen,int *pType)
{ // Arrays.fill(counters, 0);
    int size = MAX_COUNTERS_SIZE,ret = 0,startOffset,nextStart;
	int *counters = NULL;
    int counterLength = 0,charOffset,trailingWhitespace,lastPatternSize,i;
    #define  decodeRowResultSize 128
	char decodeRowResult[decodeRowResultSize] = {0};
    char *q = (char *)decodeRowResult;
	int qLen = 0;
	char startchar,endchar;

	MALLOC(counters,int,size);

    counterLength = CodaBarReaderSetCounters(row,counters,size);
    if(counterLength < 0)
	{
        FREE(counters);
		return RT_ERR_DECODACODE;
	}

    startOffset = CodaBarReaderFindStartPattern(counterLength,counters,size);
	if(startOffset < 0)
	{
		FREE(counters);
		return RT_ERR_DECODACODE;
	}
    nextStart = startOffset;

  //decodeReaderRowResult.clear();
   do {
    charOffset = CodaBarReaderToNarrowWidePattern(nextStart,counterLength,counters,size);
    if (charOffset == -1) {
      //throw NotFoundException();
		ret = RT_ERR_DECODACODE;
		goto ERROR_EXIT;
    }
    // Hack: We store the position in the alphabet table into a
    // StringBuilder, so that we can access the decoded patterns in
    // validatePattern. We'll translate to the actual characters later.
    *q++ = (char)charOffset;//decodeRowResult.append(1, (char)charOffset);
	qLen++;
    nextStart += 8;
    // Stop as soon as we see the end character.
    if (qLen > 1 &&
        CodaBarReaderArrayContains(STARTEND_ENCODING, ALPHABET[charOffset])) {
      break;
    }
  } while (nextStart < counterLength); // no fixed end pattern so keep on reading while data is available

  // Look for whitespace after pattern:
  trailingWhitespace = counters[nextStart - 1];
  lastPatternSize = 0;
  for (i = -8; i < -1; i++) {
    lastPatternSize += counters[nextStart + i];
  }

  // We need to see whitespace equal to 50% of the last pattern size,
  // otherwise this is probably a false positive. The exception is if we are
  // at the end of the row. (I.e. the barcode barely fits.)
  if (nextStart < counterLength && trailingWhitespace < lastPatternSize / 2) {
    //throw NotFoundException();
	ret = RT_ERR_DECODACODE;
	goto ERROR_EXIT;
  }

  ret = CodaBarReaderValidatePattern(startOffset,decodeRowResult,qLen,counters,size);
  if(ret < 0)
  {
	  ret = RT_ERR_DECODACODE;
	  goto ERROR_EXIT;
  }

  // Translate character table offsets to actual characters.
  for (i = 0; i < (int)qLen; i++) {
    decodeRowResult[i] = ALPHABET[(int)decodeRowResult[i]];
  }
  // Ensure a valid start and end character
  startchar = decodeRowResult[0];
  if (!CodaBarReaderArrayContains(STARTEND_ENCODING, startchar)) {
    //throw NotFoundException();
    ret = RT_ERR_DECODACODE;
    goto ERROR_EXIT;
  }
  endchar = decodeRowResult[qLen - 1];
  if (!CodaBarReaderArrayContains(STARTEND_ENCODING, endchar)) {
    //throw NotFoundException();
	ret = RT_ERR_DECODACODE;
	goto ERROR_EXIT;
  }

  // remove stop/start characters character and check if a long enough string is contained
  if ((int)qLen <= MIN_CHARACTER_LENGTH) {
    // Almost surely a false positive ( start + stop + at least 1 character)
    //throw NotFoundException();
    ret = RT_ERR_DECODACODE;
    goto ERROR_EXIT;
  }

  qLen-=2;
  memcpy(pResult,decodeRowResult+1,qLen);
  *pResultLen = qLen;
  ret = 0;

  *pType = CODABAR;

ERROR_EXIT:
    FREE(counters);
	return ret;
}
