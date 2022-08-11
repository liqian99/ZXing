#include "ITFReader.h"
#include "UPCEANReader.h"
#include "../DiyMalloc.h"

#define WW 3 // Pixel width of a wide line
#define NN 1 // Pixed width of a narrow line

static const int DEFAULT_ALLOWED_LENGTHS_[] =
{ 48, 44, 24, 20, 18, 16, 14, 12, 10, 8, 6 };
// const ArrayRef<int> DEFAULT_ALLOWED_LENGTHS (new Array<int>(VECTOR_INIT(DEFAULT_ALLOWED_LENGTHS_)));

/**
 * Start/end guard pattern.
 *
 * Note: The end pattern is reversed because the row is reversed before
 * searching for the END_PATTERN
 */
static const int START_PATTERN_[] = {NN, NN, NN, NN};
// const vector<int> START_PATTERN (VECTOR_INIT(START_PATTERN_));

static const int END_PATTERN_REVERSED_[] = {NN, NN, WW};
// const vector<int> END_PATTERN_REVERSED (VECTOR_INIT(END_PATTERN_REVERSED_));

/**
 * Patterns of Wide / Narrow lines to indicate each digit
 */
static const int PATTERNS[][5] = {
  {NN, NN, WW, WW, NN}, // 0
  {WW, NN, NN, NN, WW}, // 1
  {NN, WW, NN, NN, WW}, // 2
  {WW, WW, NN, NN, NN}, // 3
  {NN, NN, WW, NN, WW}, // 4
  {WW, NN, WW, NN, NN}, // 5
  {NN, WW, WW, NN, NN}, // 6
  {NN, NN, NN, WW, WW}, // 7
  {WW, NN, NN, WW, NN}, // 8
  {NN, WW, NN, WW, NN}  // 9
};


// ITFReader::ITFReader() : narrowLineWidth(-1) {
// }

/**
 * Attempts to decode a sequence of ITF black/white lines into single
 * digit.
 *
 * @param counters the counts of runs of observed black/white/black/... values
 * @return The decoded digit
 * @throws ReaderException if digit cannot be decoded
 */
static int ITFReaderDecodeDigit(int *counters,int countersSize)
{
  int bestVariance = MAX_AVG_VARIANCE; // worst variance we'll accept
  int bestMatch = -1,i,variance;
  int max = sizeof(PATTERNS)/sizeof(PATTERNS[0]);
  int *pattern;

  for (i = 0; i < max; i++) {
    pattern = (int *)PATTERNS[i][0];
    variance = OneDPatternMatchVariance(counters,countersSize,pattern,MAX_INDIVIDUAL_VARIANCE);
    if (variance < bestVariance) {
      bestVariance = variance;
      bestMatch = i;
    }
  }
  if (bestMatch >= 0) {
     return bestMatch;
  } else {
	 return RT_ERR_FOUNDPATTERN;
  }
}

/**
 * Skip all whitespace until we get to the first black line.
 *
 * @param row row of black/white values to search
 * @return index of the first black line.
 * @throws ReaderException Throws exception if no black lines are found in the row
 */
static int ITFReaderSkipWhiteSpace(BitArray *row)
{
  int width = BitArrayGetSize(row);
  int endStart = BitArrayGetNextSet(row,0);
  if (endStart == width) {
    return RT_ERR_FOUNDPATTERN;
  }
  return endStart;
}

/**
 * @param row       row of black/white values to search
 * @param rowOffset position to start search
 * @param pattern   pattern of counts of number of black and white pixels that are
 *                  being searched for as a pattern
 * @return start/end horizontal offset of guard pattern, as an array of two
 *         ints
 * @throws ReaderException if pattern is not found
 */
static int ITFReaderFindGuardPattern(BitArray *row,
                              int rowOffset,
                              int *pattern,int patternSize,
                              OneDRange *pRange)
{
  // TODO: This is very similar to implementation in UPCEANReader. Consider if they can be
  // merged to a single method.
  int patternLength = patternSize,ret = 0;
  int *counters = NULL;
  int width = BitArrayGetSize(row);
  BOOL isWhite = FALSE;

  int counterPosition = 0,x,y;
  int patternStart = rowOffset;

  MALLOC(counters,int,patternLength);

  for (x = rowOffset; x < width; x++) {
    if (BitArrayGet(row,x) ^ isWhite) {
      counters[counterPosition]++;
    } else {
      if (counterPosition == patternLength - 1) {
        if (OneDPatternMatchVariance(counters,patternLength,&pattern[0], MAX_INDIVIDUAL_VARIANCE) < MAX_AVG_VARIANCE) {
			pRange->data[0] = patternStart;
			pRange->data[1] = x;
			ret = 0;
			goto ERROR_EXIT;
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
  ret = RT_ERR_FOUNDPATTERN;

ERROR_EXIT:
  FREE(counters);
  return ret;
}
/**
 * The start & end patterns must be pre/post fixed by a quiet zone. This
 * zone must be at least 10 times the width of a narrow line.  Scan back until
 * we either get to the start of the barcode or match the necessary number of
 * quiet zone pixels.
 *
 * Note: Its assumed the row is reversed when using this method to find
 * quiet zone after the end pattern.
 *
 * ref: http://www.barcode-1.net/i25code.html
 *
 * @param row bit array representing the scanned barcode.
 * @param startPattern index into row of the start or end pattern.
 * @throws ReaderException if the quiet zone cannot be found, a ReaderException is thrown.
 */
static int ITFReaderValidateQuietZone(BitArray *row, int startPattern,int narrowLineWidth)
{
  //return;

  int quietCount = narrowLineWidth * 10;  // expect to find this many pixels of quiet zone
  int i;

  for (i = startPattern - 1; quietCount > 0 && i >= 0; i--) {
    if (BitArrayGet(row,i)) {
      break;
    }
    quietCount--;
  }
  if (quietCount != 0) {
    // Unable to find the necessary number of quiet zone pixels.
    //throw NotFoundException();
	 return RT_ERR_FOUNDPATTERN;
  }
  return 0;
}


/**
 * Identify where the start of the middle / payload section starts.
 *
 * @param row row of black/white values to search
 * @return Array, containing index of start of 'start block' and end of
 *         'start block'
 * @throws ReaderException
 */
static int ITFReaderDecodeStart(BitArray *row,OneDRange *pRange,int *narrowLineWidth)
{
  int ret = 0;
  int endStart = ITFReaderSkipWhiteSpace(row);
  //OneDRange startPattern;

  ret = ITFReaderFindGuardPattern(row, endStart, (int *)START_PATTERN_,4,pRange);
  if(ret < 0) return ret;

  // Determine the width of a narrow line in pixels. We can do this by
  // getting the width of the start pattern and dividing by 4 because its
  // made up of 4 narrow lines.
  *narrowLineWidth = (pRange->data[1] - pRange->data[0]) >> 2;

  ret = ITFReaderValidateQuietZone(row, pRange->data[0],*narrowLineWidth);
  return ret;
}

/**
 * Identify where the end of the middle / payload section ends.
 *
 * @param row row of black/white values to search
 * @return Array, containing index of start of 'end block' and end of 'end
 *         block'
 * @throws ReaderException
 */

static int ITFReaderDecodeEnd(BitArray *row,OneDRange *pRange,int narrowLineWidth)
{
  // For convenience, reverse the row and then
  // search from 'the start' for the end block
  //BitArray::Reverse r (row);
  int endStart,ret = 0,temp;
  OneDRange endPattern;

  BitArrayReverse(row);

  endStart = ITFReaderSkipWhiteSpace(row);
  if(endStart < 0) return endStart;
  ret = ITFReaderFindGuardPattern(row, endStart, (int *)END_PATTERN_REVERSED_,3,&endPattern);
  if(ret < 0) return ret;

  // The start & end patterns must be pre/post fixed by a quiet zone. This
  // zone must be at least 10 times the width of a narrow line.
  // ref: http://www.barcode-1.net/i25code.html
  ret = ITFReaderValidateQuietZone(row, endPattern.data[0],narrowLineWidth);
  if(ret < 0) return ret;

  // Now recalculate the indices of where the 'endblock' starts & stops to
  // accommodate
  // the reversed nature of the search
  temp = endPattern.data[0];
  pRange->data[0] = BitArrayGetSize(row) - endPattern.data[1];
  pRange->data[1] = BitArrayGetSize(row) - temp;
  
  return 0;
}





/**
 * @param row          row of black/white values to search
 * @param payloadStart offset of start pattern
 * @param resultString {@link StringBuffer} to append decoded chars to
 * @throws ReaderException if decoding could not complete successfully
 */
int ITFReaderDecodeMiddle(BitArray *row,
                          int payloadStart,
                          int payloadEnd,
                          char *result,int *resultLen)
{
  // Digits are interleaved in pairs - 5 black lines for one digit, and the
  // 5
  // interleaved white lines for the second digit.
  // Therefore, need to scan 10 lines and then
  // split these into two arrays
  int counterDigitPair[10] = {0};
  int counterBlack[5] = {0};
  int counterWhite[5] = {0};
  int k,twoK,bestMatch,i,e,ret=0,qLen = 0;
  char *q = result;

  while (payloadStart < payloadEnd) {

    // Get 10 runs of black/white.
    ret = OneDRecordPattern(row, payloadStart, counterDigitPair,10);
	if(ret !=0) {return ret;}
    // Split them into each array
    for (k = 0; k < 5; k++) {
      twoK = k << 1;
      counterBlack[k] = counterDigitPair[twoK];
      counterWhite[k] = counterDigitPair[twoK + 1];
    }

    bestMatch = ITFReaderDecodeDigit(counterBlack,5);
	if(bestMatch < 0) {return bestMatch;}
	*q++ = (char) ('0' + bestMatch);
	qLen++;
    bestMatch = ITFReaderDecodeDigit(counterWhite,5);
	if(bestMatch < 0) {return bestMatch;}
	*q++ = (char) ('0' + bestMatch);
	qLen++;

    for (i = 0, e = 10; i < e; i++) {
      payloadStart += counterDigitPair[i];
    }
  }

  return 0;
}


int ITFReaderDecodeRow(int rowNumber, BitArray *row,char *result,int *resultLen)
{
	// Find out where the Middle section (payload) starts & ends
	int ret=0,narrowLineWidth=0,length,i,e;
	OneDRange startRange,endRange;
	BOOL lengthOK;
	int *allowedLengths;
	//   Range startRange = decodeStart(row);
	//   Range endRange = decodeEnd(row);
	ret = ITFReaderDecodeStart(row,&startRange,&narrowLineWidth);
	if(ret < 0) return ret;
	
	ret = ITFReaderDecodeEnd(row,&endRange,narrowLineWidth);
	if(ret < 0) return ret;
	
	//   std::string result;
	//   decodeMiddle(row, startRange[1], endRange[0], result);
	ret = ITFReaderDecodeMiddle(row,startRange.data[1], endRange.data[0], result,resultLen);
	if(ret < 0) return ret;
	
	//   Ref<String> resultString(new String(result));
	
	//   ArrayRef<int> allowedLengths;
	//   // Java hints stuff missing
	//   if (!allowedLengths) {
	//     allowedLengths = DEFAULT_ALLOWED_LENGTHS;
	//   }
	
	allowedLengths = (int *)DEFAULT_ALLOWED_LENGTHS_;
	
	// To avoid false positives with 2D barcodes (and other patterns), make
	// an assumption that the decoded string must be 6, 10 or 14 digits.
	length = *resultLen;//resultString->size();
	lengthOK = FALSE;
	for (i = 0, e = 11; i < e; i++) {
		if (length == allowedLengths[i]) {
			lengthOK = TRUE;
			break;
		}
	}
	
	if (!lengthOK) {
		//throw FormatException();
		return RT_ERR_DECODEITF;
	}
	
	return 0;
	//   ArrayRef< Ref<ResultPoint> > resultPoints(2);
	//   resultPoints[0] =
	//       Ref<OneDResultPoint>(new OneDResultPoint(float(startRange[1]), float(rowNumber)));
	//   resultPoints[1] =
	//       Ref<OneDResultPoint>(new OneDResultPoint(float(endRange[0]), float(rowNumber)));
	//   return Ref<Result>(new Result(resultString, ArrayRef<char>(), resultPoints, BarcodeFormat::ITF));
}