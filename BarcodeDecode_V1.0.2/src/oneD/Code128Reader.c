#include "Code128Reader.h"
#include "OneDReader.h"
#include "../DiyMalloc.h"
#include "../BarcodeFormat.h"

static const int MAX_AVG_VARIANCE = (int)(PATTERN_MATCH_RESULT_SCALE_FACTOR * 250/1000);
static const int MAX_INDIVIDUAL_VARIANCE = (int)(PATTERN_MATCH_RESULT_SCALE_FACTOR * 700/1000);
#define CODE_SHIFT 98
#define CODE_CODE_C 99
#define CODE_CODE_B 100
#define CODE_CODE_A 101
#define CODE_FNC_1 102
#define CODE_FNC_2 97
#define CODE_FNC_3 96
#define CODE_FNC_4_A 101
#define CODE_FNC_4_B 100
#define  CODE_START_A 103
#define  CODE_START_B 104
#define  CODE_START_C 105
#define  CODE_STOP 106
static const int CODE_PATTERNS_LENGTH = 107;
static const int CODE_PATTERNS[107/*CODE_PATTERNS_LENGTH*/][6] = {
  {2, 1, 2, 2, 2, 2}, /* 0 */
  {2, 2, 2, 1, 2, 2},
  {2, 2, 2, 2, 2, 1},
  {1, 2, 1, 2, 2, 3},
  {1, 2, 1, 3, 2, 2},
  {1, 3, 1, 2, 2, 2}, /* 5 */
  {1, 2, 2, 2, 1, 3},
  {1, 2, 2, 3, 1, 2},
  {1, 3, 2, 2, 1, 2},
  {2, 2, 1, 2, 1, 3},
  {2, 2, 1, 3, 1, 2}, /* 10 */
  {2, 3, 1, 2, 1, 2},
  {1, 1, 2, 2, 3, 2},
  {1, 2, 2, 1, 3, 2},
  {1, 2, 2, 2, 3, 1},
  {1, 1, 3, 2, 2, 2}, /* 15 */
  {1, 2, 3, 1, 2, 2},
  {1, 2, 3, 2, 2, 1},
  {2, 2, 3, 2, 1, 1},
  {2, 2, 1, 1, 3, 2},
  {2, 2, 1, 2, 3, 1}, /* 20 */
  {2, 1, 3, 2, 1, 2},
  {2, 2, 3, 1, 1, 2},
  {3, 1, 2, 1, 3, 1},
  {3, 1, 1, 2, 2, 2},
  {3, 2, 1, 1, 2, 2}, /* 25 */
  {3, 2, 1, 2, 2, 1},
  {3, 1, 2, 2, 1, 2},
  {3, 2, 2, 1, 1, 2},
  {3, 2, 2, 2, 1, 1},
  {2, 1, 2, 1, 2, 3}, /* 30 */
  {2, 1, 2, 3, 2, 1},
  {2, 3, 2, 1, 2, 1},
  {1, 1, 1, 3, 2, 3},
  {1, 3, 1, 1, 2, 3},
  {1, 3, 1, 3, 2, 1}, /* 35 */
  {1, 1, 2, 3, 1, 3},
  {1, 3, 2, 1, 1, 3},
  {1, 3, 2, 3, 1, 1},
  {2, 1, 1, 3, 1, 3},
  {2, 3, 1, 1, 1, 3}, /* 40 */
  {2, 3, 1, 3, 1, 1},
  {1, 1, 2, 1, 3, 3},
  {1, 1, 2, 3, 3, 1},
  {1, 3, 2, 1, 3, 1},
  {1, 1, 3, 1, 2, 3}, /* 45 */
  {1, 1, 3, 3, 2, 1},
  {1, 3, 3, 1, 2, 1},
  {3, 1, 3, 1, 2, 1},
  {2, 1, 1, 3, 3, 1},
  {2, 3, 1, 1, 3, 1}, /* 50 */
  {2, 1, 3, 1, 1, 3},
  {2, 1, 3, 3, 1, 1},
  {2, 1, 3, 1, 3, 1},
  {3, 1, 1, 1, 2, 3},
  {3, 1, 1, 3, 2, 1}, /* 55 */
  {3, 3, 1, 1, 2, 1},
  {3, 1, 2, 1, 1, 3},
  {3, 1, 2, 3, 1, 1},
  {3, 3, 2, 1, 1, 1},
  {3, 1, 4, 1, 1, 1}, /* 60 */
  {2, 2, 1, 4, 1, 1},
  {4, 3, 1, 1, 1, 1},
  {1, 1, 1, 2, 2, 4},
  {1, 1, 1, 4, 2, 2},
  {1, 2, 1, 1, 2, 4}, /* 65 */
  {1, 2, 1, 4, 2, 1},
  {1, 4, 1, 1, 2, 2},
  {1, 4, 1, 2, 2, 1},
  {1, 1, 2, 2, 1, 4},
  {1, 1, 2, 4, 1, 2}, /* 70 */
  {1, 2, 2, 1, 1, 4},
  {1, 2, 2, 4, 1, 1},
  {1, 4, 2, 1, 1, 2},
  {1, 4, 2, 2, 1, 1},
  {2, 4, 1, 2, 1, 1}, /* 75 */
  {2, 2, 1, 1, 1, 4},
  {4, 1, 3, 1, 1, 1},
  {2, 4, 1, 1, 1, 2},
  {1, 3, 4, 1, 1, 1},
  {1, 1, 1, 2, 4, 2}, /* 80 */
  {1, 2, 1, 1, 4, 2},
  {1, 2, 1, 2, 4, 1},
  {1, 1, 4, 2, 1, 2},
  {1, 2, 4, 1, 1, 2},
  {1, 2, 4, 2, 1, 1}, /* 85 */
  {4, 1, 1, 2, 1, 2},
  {4, 2, 1, 1, 1, 2},
  {4, 2, 1, 2, 1, 1},
  {2, 1, 2, 1, 4, 1},
  {2, 1, 4, 1, 2, 1}, /* 90 */
  {4, 1, 2, 1, 2, 1},
  {1, 1, 1, 1, 4, 3},
  {1, 1, 1, 3, 4, 1},
  {1, 3, 1, 1, 4, 1},
  {1, 1, 4, 1, 1, 3}, /* 95 */
  {1, 1, 4, 3, 1, 1},
  {4, 1, 1, 1, 1, 3},
  {4, 1, 1, 3, 1, 1},
  {1, 1, 3, 1, 4, 1},
  {1, 1, 4, 1, 3, 1}, /* 100 */
  {3, 1, 1, 1, 4, 1},
  {4, 1, 1, 1, 3, 1},
  {2, 1, 1, 4, 1, 2},
  {2, 1, 1, 2, 1, 4},
  {2, 1, 1, 2, 3, 2}, /* 105 */
  {2, 3, 3, 1, 1, 1}
};

static char *t_itoa(int value, char *string, int radix)
{
	char tmp[33] = {0};
	char *tp = tmp;
	int i;
	unsigned v;
	int sign;
	char *sp;
	
	if (radix > 36 || radix <= 1)
	{
		return 0;
	}
	
	sign = (radix == 10 && value < 0);
	if (sign)
		v = -value;
	else
		v = (unsigned)value;
	while (v || tp == tmp)
	{
		i = v % radix;
		v = v / radix;
		if (i < 10)
			*tp++ = i+'0';
		else
			*tp++ = i + 'a' - 10;
	}
	
	if (string == 0)
		return 0;
	sp = string;
	
	if (sign)
		*sp++ = '-';
	while (tp > tmp)
		*sp++ = *--tp;
	*sp = 0;
	return string;
}


static int Code128ReaderFindStartPattern(BitArray *row,OneDRange *pRange)
{
  int width = BitArrayGetSize(row);           //row->getSize();
  int rowOffset = BitArrayGetNextSet(row,0);  //row->getNextSet(0);
  int counterPosition = 0;
  int counters[6] = {0};
  int patternStart = rowOffset;
  BOOL isWhite = FALSE;
  int patternLength =  6,i,bestVariance,bestMatch,startCode,variance,y;

  for (i = rowOffset; i < width; i++) {
    if (BitArrayGet(row,i) ^ isWhite) {
      counters[counterPosition]++;
    } else {
      if (counterPosition == patternLength - 1) {
         bestVariance = MAX_AVG_VARIANCE;
         bestMatch = -1;
        for (startCode = CODE_START_A; startCode <= CODE_START_C; startCode++) {
          variance = OneDPatternMatchVariance(counters,6, (int *)(CODE_PATTERNS[startCode]), MAX_INDIVIDUAL_VARIANCE);
          if (variance < bestVariance) {
            bestVariance = variance;
            bestMatch = startCode;
          }
        }
        // Look for whitespace before start pattern, >= 50% of width of start pattern
        if (bestMatch >= 0 &&
            BitArrayIsRange(row,MAX(0, patternStart - (i - patternStart) / 2), patternStart, FALSE))
		{
           pRange->data[0] = patternStart;
           pRange->data[1] = i;
           pRange->data[2] = bestMatch;
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
  return RT_ERR_FOUNDPATTERN;
}

static int Code128ReaderDecodeCode(BitArray *row, int *counters,int countersSize,int rowOffset)
{
    int ret = 0;
	int bestVariance = MAX_AVG_VARIANCE; // worst variance we'll accept
	int bestMatch = -1,d,variance;
    int *pattern = NULL;

    ret = OneDRecordPattern(row, rowOffset, counters,countersSize);
	if(ret != 0) return ret;

    for (d = 0; d < CODE_PATTERNS_LENGTH; d++) {
    pattern = (int *)CODE_PATTERNS[d];
    variance = OneDPatternMatchVariance(counters,countersSize,pattern,MAX_INDIVIDUAL_VARIANCE);
    if (variance < bestVariance) {
      bestVariance = variance;
      bestMatch = d;
    }
  }
  // TODO We're overlooking the fact that the STOP pattern has 7 values, not 6.
  if (bestMatch >= 0) {
    return bestMatch;
  } else {
    return RT_ERR_FOUNDPATTERN;
  }
}

int Code128DecodeRow(int rowNumber,BitArray *row,char *pResult,int *pResultLen,int *pType)
{
  BOOL convertFNC1 = FALSE,done,isNextShifted,lastCharacterWasPrintable,unshift;
  OneDRange startPatternInfo;
  int ret = 0,startCode,codeSet,lastStart,nextStart,lastCode,code,checksumTotal,multiplier,i,e;
  char rawCodes[20] = {0};
  int counters[6] = {0};
  char tmpChar[128] = {0};
  char *q = tmpChar;
  int qLen = 0,resultLength/*,rawCodesSize*/;
  char tmp[16] = {0};
//   float left,right;

  diy_memset(&startPatternInfo,0,sizeof(OneDRange));
  ret = Code128ReaderFindStartPattern(row,&startPatternInfo);
  if(ret != 0) return ret;

  startCode = startPatternInfo.data[2];
  switch(startCode) 
  {
    case CODE_START_A:
      codeSet = CODE_CODE_A;
      break;
    case CODE_START_B:
      codeSet = CODE_CODE_B;
      break;
    case CODE_START_C:
      codeSet = CODE_CODE_C;
      break;
    default:
      return RT_ERR_FORMAT;
  }

  done = FALSE;
  isNextShifted = FALSE;

  //string result;
  //vector<char> rawCodes(20, 0);

  lastStart = startPatternInfo.data[0];
  nextStart = startPatternInfo.data[1];
//   vector<int> counters (6, 0);

  lastCode = 0;
  code = 0;
  checksumTotal = startCode;
  multiplier = 0;
  lastCharacterWasPrintable = TRUE;

//   std::ostringstream oss;

  while (!done) {

    unshift = isNextShifted;
    isNextShifted = FALSE;

    // Save off last code
    lastCode = code;

    code = Code128ReaderDecodeCode(row, counters,6,nextStart);

    // Remember whether the last code was printable or not (excluding CODE_STOP)
    if (code != CODE_STOP) {
      lastCharacterWasPrintable = TRUE;
    }

    // Add to checksum computation (if not CODE_STOP of course)
    if (code != CODE_STOP) {
      multiplier++;
      checksumTotal += multiplier * code;
    }

    // Advance to where the next code will to start
    lastStart = nextStart;
    for (i = 0, e = 6/*counters.size()*/; i < e; i++) {
      nextStart += counters[i];
    }

    // Take care of illegal start codes
    switch (code) {
      case CODE_START_A:
      case CODE_START_B:
      case CODE_START_C:
        //throw FormatException();
		ret = RT_ERR_FORMAT;
		goto ERROR_EXIT;
    }

    switch (codeSet) {

      case CODE_CODE_A:
        if (code < 64) {
          *q++ = (char) (' ' + code);//result.append(1, (char) (' ' + code));
		  qLen++;
        } else if (code < 96) {
          *q++ = (char) (code - 64);//result.append(1, (char) (code - 64));
		  qLen++;
        } else {
          // Don't let CODE_STOP, which always appears, affect whether whether we think the
          // last code was printable or not.
          if (code != CODE_STOP) {
            lastCharacterWasPrintable = FALSE;
          }
          switch (code) {
            case CODE_FNC_1:
              if (convertFNC1) {
                if (qLen == 0){
                  // GS1 specification 5.4.3.7. and 5.4.6.4. If the first char after the start code
                  // is FNC1 then this is GS1-128. We add the symbology identifier.
                  //result.append("]C1");
				  *q++ = ']';
				  *q++ = 'C';
				  *q++ = '1';
				  qLen+=3;
                } else {
                  // GS1 specification 5.4.7.5. Every subsequent FNC1 is returned as ASCII 29 (GS)
                  //result.append(1, (char) 29);
                  *q++ = (char)29;
				  qLen++;
                }
              }
              break;
            case CODE_FNC_2:
            case CODE_FNC_3:
            case CODE_FNC_4_A:
              // do nothing?
              break;
            case CODE_SHIFT:
              isNextShifted = TRUE;
              codeSet = CODE_CODE_B;
              break;
            case CODE_CODE_B:
              codeSet = CODE_CODE_B;
              break;
            case CODE_CODE_C:
              codeSet = CODE_CODE_C;
              break;
            case CODE_STOP:
              done = TRUE;
              break;
          }
        }
        break;
      case CODE_CODE_B:
        if (code < 96) {
          //result.append(1, (char) (' ' + code));
			*q++ = (char)(' ' + code);
            qLen++;
        } else {
          if (code != CODE_STOP) {
            lastCharacterWasPrintable = FALSE;
          }
          switch (code) {
            case CODE_FNC_1:
            case CODE_FNC_2:
            case CODE_FNC_3:
            case CODE_FNC_4_B:
              // do nothing?
              break;
            case CODE_SHIFT:
              isNextShifted = TRUE;
              codeSet = CODE_CODE_A;
              break;
            case CODE_CODE_A:
              codeSet = CODE_CODE_A;
              break;
            case CODE_CODE_C:
              codeSet = CODE_CODE_C;
              break;
            case CODE_STOP:
              done = TRUE;
              break;
          }
        }
        break;
      case CODE_CODE_C:
        if (code < 100) {
          if (code < 10) {
            //result.append(1, '0');
			*q++ = '0';
			qLen++;
          }
//           oss.clear();
//           oss.str("");
//           oss << code;
//           result.append(oss.str());
		  diy_memset(tmp,0,sizeof(tmp));
		  t_itoa(code,tmp,10);
          for(i=0;i<(int)strlen(tmp);i++)
		  {
			  *q++ = tmp[i];
			  qLen++;
		  }

        } else {
          if (code != CODE_STOP) {
            lastCharacterWasPrintable = FALSE;
          }
          switch (code) {
            case CODE_FNC_1:
              // do nothing?
              break;
            case CODE_CODE_A:
              codeSet = CODE_CODE_A;
              break;
            case CODE_CODE_B:
              codeSet = CODE_CODE_B;
              break;
            case CODE_STOP:
              done = TRUE;
              break;
          }
        }
        break;
    }

    // Unshift back to another code set if we were shifted
    if (unshift) {
      codeSet = codeSet == CODE_CODE_A ? CODE_CODE_B : CODE_CODE_A;
    }
    
  }

  // Check for ample whitespace following pattern, but, to do this we first need to remember that
  // we fudged decoding CODE_STOP since it actually has 7 bars, not 6. There is a black bar left
  // to read off. Would be slightly better to properly read. Here we just skip it:
  nextStart = BitArrayGetNextUnset(row,nextStart); //row->getNextUnset(nextStart);
  if (!BitArrayIsRange(row,nextStart,
                    MIN(BitArrayGetSize(row), nextStart + (nextStart - lastStart) / 2),
                    FALSE)) {
    //throw NotFoundException();
	  ret = RT_ERR_DECODE128;
	  goto ERROR_EXIT;
  }

  // Pull out from sum the value of the penultimate check code
  checksumTotal -= multiplier * lastCode;
  // lastCode is the checksum then:
  if (checksumTotal % 103 != lastCode) {
    //throw ChecksumException();
    ret = RT_ERR_DECODE128;
    goto ERROR_EXIT;
  }

  // Need to pull out the check digits from string
  resultLength = qLen;
  if (resultLength == 0) {
    // false positive
    //throw NotFoundException();
    ret = RT_ERR_DECODE128;
    goto ERROR_EXIT;
  }

  // Only bother if the result had at least one character, and if the checksum digit happened to
  // be a printable character. If it was just interpreted as a control code, nothing to remove.
  if (resultLength > 0 && lastCharacterWasPrintable) {
    if (codeSet == CODE_CODE_C) {
      //result.erase(resultLength - 2, resultLength);
	  tmpChar[resultLength - 2] = tmpChar[resultLength - 1] = '0';
	  resultLength-=2;
    } else {
      //result.erase(resultLength - 1, resultLength);
	  tmpChar[resultLength - 1] = '0';
      resultLength-=1;
	}
  }
  ret = 0;
  diy_memcpy(pResult,tmpChar,resultLength);
  *pResultLen = resultLength;
   
  *pType = CODE_128;

ERROR_EXIT:
  return ret;
}



