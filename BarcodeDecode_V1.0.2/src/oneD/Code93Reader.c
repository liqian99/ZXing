/**
*  Ò»Î¬Âë Code93
**/
#include "../BaseTypeDef.h"
#include "Code93Reader.h"
#include "../DiyMalloc.h"
#include "../GlobalHistogramBinarizer.h"
#include "OneDReader.h"
#include "../BarcodeFormat.h"

static char const ALPHABET[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ-. $/+%abcd*";
static int const CHARACTER_ENCODINGS[] = {
        0x114, 0x148, 0x144, 0x142, 0x128, 0x124, 0x122, 0x150, 0x112, 0x10A, // 0-9
		0x1A8, 0x1A4, 0x1A2, 0x194, 0x192, 0x18A, 0x168, 0x164, 0x162, 0x134, // A-J
		0x11A, 0x158, 0x14C, 0x146, 0x12C, 0x116, 0x1B4, 0x1B2, 0x1AC, 0x1A6, // K-T
		0x196, 0x19A, 0x16C, 0x166, 0x136, 0x13A, // U-Z
		0x12E, 0x1D4, 0x1D2, 0x1CA, 0x16E, 0x176, 0x1AE, // - - %
		0x126, 0x1DA, 0x1D6, 0x132, 0x15E, // Control chars? $-*
};
static int const CHARACTER_ENCODINGS_LENGTH = (int)sizeof(CHARACTER_ENCODINGS)/sizeof(CHARACTER_ENCODINGS[0]);
static const int ASTERISK_ENCODING = 0x15E;//CHARACTER_ENCODINGS[47];

static int Code93ToPattern(int *pCounters,int nCountersSize)
{
    int nmax,nsum,i,e,j,npattern,scaledShifted,scaledUnshifted;
    
	nmax = nCountersSize;
	nsum = 0;

	for (i=0,e=nCountersSize;i<e;++i)
	{
		nsum += pCounters[i];
	}
	npattern = 0;

	for (i=0;i<nmax;i++)
	{
		scaledShifted = (pCounters[i] << INTEGER_MATH_SHIFT)*9/nsum;
		scaledUnshifted = scaledShifted >> INTEGER_MATH_SHIFT;
		if((scaledShifted & 0xFF) > 0x7F)
			scaledUnshifted++;
		if(scaledUnshifted < 1 || scaledUnshifted > 4)
			return -1;
		if((i&0x01) == 0)
		{
			for (j=0;j<scaledUnshifted;j++)
				npattern = (npattern<<1) | 0x01;
		}else{
			npattern <<= scaledUnshifted;
		}
	}
    return npattern;
}

static char Code93PatternToChar(int nPattern)
{
	int i;

	for (i=0;i<CHARACTER_ENCODINGS_LENGTH;i++)
	{
		if(CHARACTER_ENCODINGS[i]==nPattern)
			return ALPHABET[i];
	}

	return 0;
}

static int Code93DecodeExtended(const char *pEncoded,int nEncodedLen,char *pResult,int *pResultLen)
{
   int length = nEncodedLen;
   int i;
   char c,next,decodedChar = '\0';
   char *q = pResult;
   int ret = -1,len=0;;

   for (i=0;i<length;i++)
   {
	   c = pEncoded[i];
	   if(c >= 'a' && c<= 'd')
	   {
		   if(i >= length-1)
			   goto ERROR_EXIT;
		   next = pEncoded[i+1];
		   decodedChar = '\0';
		   switch(c)
		   {
			   case 'd':
				   if(next>='A' && next<='Z')
					   decodedChar = (char)(next+32);
				   else
					   goto ERROR_EXIT;
				   break;
			   case 'a':
				   // $A to $Z map to control codes SH to SB
				   if (next >= 'A' && next <= 'Z') {
					   decodedChar = (char) (next - 64);
				   } else {
					   goto ERROR_EXIT;
				   }
				   break;
			   case 'b':
				   // %A to %E map to control codes ESC to US
				   if (next >= 'A' && next <= 'E') {
					   decodedChar = (char) (next - 38);
				   } else if (next >= 'F' && next <= 'W') {
					   decodedChar = (char) (next - 11);
				   } else {
					   goto ERROR_EXIT;
				   }
				   break;
			   case 'c':
				   // /A to /O map to ! to , and /Z maps to :
				   if (next >= 'A' && next <= 'O') {
					   decodedChar = (char) (next - 32);
				   } else if (next == 'Z') {
					   decodedChar = ':';
				   } else {
					   goto ERROR_EXIT;
				   }
				   break;
		   }
		   *q++ = decodedChar;
		   len++;
		   i++;
	   }else{
	       *q++ = c;
		   len++;
	   }
   }
   ret = 0;

ERROR_EXIT:
   *pResultLen = len;
   return ret;
}

static int Code93FindFirstOf(char *pSrc,int pSrcLen,char cPattern)
{
	int i=0;
	
    for (i=0;i<pSrcLen;i++)
    {
		if(pSrc[i] == cPattern)
			break;
    }
	
	return i;
}

static int Code93CheckOneChecksum(char *pResult,int checkPosition,int weightMax)
{
    int weight=1,total=0,i=0;
	int ret = 0;
	
	for (i=checkPosition-1;i>=0;i--)
	{
		total += weight * Code93FindFirstOf((char *)ALPHABET,sizeof(ALPHABET),pResult[i]);
		if(++weight > weightMax)
			weight = 1;
	}
	if(pResult[checkPosition] != ALPHABET[total%47])
		ret = -1;
	
	return ret;
}

static int Code93CheckChecksums(char *pResult,int nResultLen)
{
	int ret1=0,ret2=0;

	ret1 = Code93CheckOneChecksum(pResult, nResultLen-2, 20);
    ret2 = Code93CheckOneChecksum(pResult, nResultLen-1, 15);

	if(ret1==0 && ret2==0)
		return 0;
	else
		return -1;
}

static int Code93FindAsteriskPattern(BitArray *pRow,int *pCounters,int nCounterSize,OneDRange *pRange)
{
    int width,rowOffset;
	int patternStart,patternLength,counterPosition,i,y;
	BOOL isWhite;
	BOOL bGet;
	int ret = -1;

	width = BitArrayGetSize(pRow);
	rowOffset = BitArrayGetNextSet(pRow,0);

	counterPosition=0;
	patternStart = rowOffset;
	isWhite = FALSE;
	patternLength = nCounterSize;

	counterPosition = 0;

	for (i=rowOffset;i<width;i++)
	{
		bGet = BitArrayGet(pRow,i);
		if(bGet^isWhite)
		{
            pCounters[counterPosition]++;
		}else{
			if(counterPosition == patternLength-1){
				if(Code93ToPattern(pCounters,nCounterSize) == ASTERISK_ENCODING){
				    pRange->data[0] = patternStart;
					pRange->data[1] = i;
					ret = 0;
					return ret;
				}
				patternStart += pCounters[0] + pCounters[1];
				for (y=2;y<patternLength;y++)
				{
                    pCounters[y-2] = pCounters[y];
				}
				pCounters[patternLength-2] = 0;
				pCounters[patternLength-1] = 0;
				counterPosition--;
			}else{
				counterPosition++;
			}
			pCounters[counterPosition] = 1;
			isWhite = !isWhite;
		}
	}

	return ret;
}

int Code93DecodeRow(int rowNumber,BitArray *pRow,char *pResult,int *pResultLen,int *pType)
{
   OneDRange start;
   int nextStart,end,pattern;
   char decodedChar;
   int lastStart;
   int i;
   int qLen=0;
   int ret = RT_ERR_UNKNOW;
   #define nCountersSize 6
   #define nMax93Len     128
   int pCounters[nCountersSize] = {0};
   char cTmp[nMax93Len] = {0};
   char *q = (char *)cTmp;


   ret = Code93FindAsteriskPattern(pRow,pCounters,nCountersSize,&start);
   if(ret==0)
   {
       nextStart = BitArrayGetNextSet(pRow,start.data[1]);
	   end = BitArrayGetSize(pRow);

	   do{
           ret = OneDRecordPattern(pRow,nextStart,pCounters,nCountersSize);
		   if(ret==0)
		   {
               pattern = Code93ToPattern(pCounters,nCountersSize);
			   if(pattern < 0)
			   {
				   ret = RT_ERR_FOUNDPATTERN;
				   goto ERROR_EXIT;
			   }
			   decodedChar = Code93PatternToChar(pattern);
			   if(decodedChar > 0)
			   {
                   *q++ = decodedChar;
				   qLen++;
				   if(qLen >= nMax93Len-1) {ret=RT_ERR_DECODE93;goto ERROR_EXIT;}
				   lastStart = nextStart;
				   for (i=0,end=nCountersSize;i<end;i++)
				   {
					   nextStart += pCounters[i];
				   }
				   nextStart = BitArrayGetNextSet(pRow,nextStart);
			   }else{
				   ret = RT_ERR_FOUNDPATTERN;
				   goto ERROR_EXIT;
			   }
		   }else
		   {
			   ret = RT_ERR_FOUNDPATTERN;
			   goto ERROR_EXIT;
		   }
	   }while(decodedChar != '*');
       //remove asterisk
	   if(qLen<=3)
	   {
		   ret = RT_ERR_DECODE93;
		   goto ERROR_EXIT;
	   }
	   q[--qLen] = 0;
       //Should be at least one more black module
	   if(nextStart == end || !BitArrayGet(pRow,nextStart))
	   {
		   ret = RT_ERR_DECODE93;
		   goto ERROR_EXIT;
	   }
       //false postive -- need at least 2 checksum digits
//        if(qLen < 2)
// 	   {
// 		   ret = -1;
// 		   goto ERROR_EXIT;
// 	   }

	   ret = Code93CheckChecksums(cTmp,qLen);

	   //Remove checksum digits
	   cTmp[--qLen] = 0;
	   cTmp[--qLen] = 0;

	   ret = Code93DecodeExtended(cTmp,qLen,pResult,pResultLen);

	   *pType = CODE_93;
   }

ERROR_EXIT:
   return ret;
}

