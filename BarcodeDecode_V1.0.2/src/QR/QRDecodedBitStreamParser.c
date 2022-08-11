#include "QRDecodedBitStreamParser.h"
#include "../DiyMalloc.h"
#include "../StringUtils.h"
#include "../iconv/utfswap.h"
#include "../iconv/wcmbswap.h"

const char ALPHANUMERIC_CHARS[] =
	{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B',
	'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
	'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z', ' ', '$', '%', '*', '+', '-', '.', '/', ':'
	};

int GB2312_SUBSET = 1;

int UTF8ToGB2312(unsigned char *pUtf8,int utf8Len,unsigned char *pGB2312,int *pGB2312Len)
{
	int ret = 0;
    unsigned char *pUtf16 = NULL;
	int nUtf16Len = utf8Len * 2 + 2;

	MALLOC(pUtf16,unsigned char,nUtf16Len);

	ret = UTF8ToUTF16(pUtf8,utf8Len,pUtf16,&nUtf16Len);
	if(ret > 0 && nUtf16Len > 2)
	{
	    ret = UTF16ToGB2312(pUtf16,nUtf16Len,pGB2312,pGB2312Len);
		if(ret <= 0)
           ret = RT_ERR_ILLEGALCHARACTER;
	}else
		ret = RT_ERR_ILLEGALCHARACTER;

ERROR_EXIT:
	FREE(pUtf16);
    return ret;
}

int QRDecodedBitStreamParserDecodeHanziSegment(BitSource *bits_,char *result,int *resultLen,int count)
{
    BitSource *bits = bits_;
	unsigned int nBytes;
    char *buffer = NULL;
	int offset,twoBytes,assembledTwoBytes,ret=0;

	if(count * 13 > BitSourceAvailable(bits))
		return RT_ERR_QR_FORMAT;

	// Each character will require 2 bytes. Read the characters as 2-byte pairs
	// and decode as GB2312 afterwards
	nBytes = 2 * count;
	MALLOC(buffer,char,nBytes);
	offset = 0;
	while (count > 0) {
		// Each 13 bits encodes a 2-byte character
		twoBytes = BitSourceReadBits(bits,13);
		assembledTwoBytes = ((twoBytes / 0x060) << 8) | (twoBytes % 0x060);
		if (assembledTwoBytes < 0x003BF) {
			// In the 0xA1A1 to 0xAAFE range
			assembledTwoBytes += 0x0A1A1;
		} else {
			// In the 0xB0A1 to 0xFAFE range
			assembledTwoBytes += 0x0A6A1;
		}
		buffer[offset] = (char) ((assembledTwoBytes >> 8) & 0xFF);
		buffer[offset + 1] = (char) (assembledTwoBytes & 0xFF);
		offset += 2;
		count--;
  }

  diy_memcpy(result,buffer,nBytes);
  *resultLen = nBytes;

ERROR_EXIT:
  FREE(buffer);
  return ret;
}

int QRDecodedBitStreamParserDecodeKanjiSegment(BitSource *bits_,char *result,int *resultLen,int count)
{
	// Each character will require 2 bytes. Read the characters as 2-byte pairs
	// and decode as Shift_JIS afterwards
	unsigned int nBytes = 2 * count;
	char* buffer = NULL;
	int twoBytes,assembledTwoBytes;
	int offset = 0,ret = 0;

    MALLOC(buffer,char,nBytes);
	while (count > 0) {
		// Each 13 bits encodes a 2-byte character
		
		twoBytes = BitSourceReadBits(bits_,13);
		assembledTwoBytes = ((twoBytes / 0x0C0) << 8) | (twoBytes % 0x0C0);
		if (assembledTwoBytes < 0x01F00) {
			// In the 0x8140 to 0x9FFC range
			assembledTwoBytes += 0x08140;
		} else {
			// In the 0xE040 to 0xEBBF range
			assembledTwoBytes += 0x0C140;
		}
		buffer[offset] = (char)(assembledTwoBytes >> 8);
		buffer[offset + 1] = (char)assembledTwoBytes;
		offset += 2;
		count--;
  }
  
  diy_memcpy(result,buffer,nBytes);
  *resultLen = nBytes;

ERROR_EXIT:
  FREE(buffer);
  return ret;
}

int QRDecodedBitStreamParserDecodeByteSegment(BitSource *bits_,char *result,int *resultLen,int count,
											  CharacterSetECI *currentCharacterSetECI,
											  char *byteSegments,
											  DecodeHints *hints
                                              )
{
    //int nBytes = count;
	BitSource *bits = bits_;
    char *bytes_ = NULL,*readBytes = NULL;
	int i,ret = 0,nLen = 1024;
	char *encoding = NULL;


	if(count << 3 > BitSourceAvailable(bits))
		return RT_ERR_QR_FORMAT;

    MALLOC(bytes_,char,count);
	readBytes = bytes_;
    for(i=0;i<count;i++)
    {
        readBytes[i] = (char)BitSourceReadBits(bits,8);
    }

	if (currentCharacterSetECI == 0) {
		// The spec isn't clear on this mode; see
		// section 6.4.5: t does not say which encoding to assuming
		// upon decoding. I have seen ISO-8859-1 used as well as
		// Shift_JIS -- without anything like an ECI designator to
		// give a hint.
		encoding = (char *)StringUtilsGuessEncoding(readBytes,count,hints);
	} else {
		encoding = (char *)CharacterSetECIGetName(currentCharacterSetECI);//currentCharacterSetECI->name();
	}

 	diy_memcpy(result,bytes_,count);
 	*resultLen = count;
//	GB2UtfInit();
//	ret = CodeConv("UTF8","GB2312",(unsigned char *)result,&nLen,);
//    *resultLen = nLen;
// #ifdef _DEBUG
// 	printf("result=%s\n",(char *)result);
// #endif
	ret = UTF8ToGB2312((unsigned char *)bytes_,(int)count,(unsigned char *)result,&nLen);
    *resultLen = nLen;
ERROR_EXIT:
	FREE(bytes_);
	return ret;
}

int QRDecodedBitStreamParserDecodeNumericSegment(BitSource *bits,char *result,int *resultLen,int count)
{
    int nBytes = count,i=0,threeDigitsBits,twoDigitsBits,ret = 0,digitBits;
	char *bytes = NULL;

	MALLOC(bytes,char,nBytes);
	// Read three digits at a time
	while (count >= 3) {
		// Each 10 bits encodes three digits
		if (BitSourceAvailable(bits) < 10)//(bits->available() < 10)
		{
			ret = RT_ERR_QR_FORMAT;
			goto ERROR_EXIT;
			//throw ReaderException("format exception");
		}
		threeDigitsBits = BitSourceReadBits(bits,10);
		if (threeDigitsBits >= 1000) {
			ret = RT_ERR_QR_FORMAT;
			goto ERROR_EXIT;
		}
		bytes[i++] = ALPHANUMERIC_CHARS[threeDigitsBits / 100];
		bytes[i++] = ALPHANUMERIC_CHARS[(threeDigitsBits / 10) % 10];
		bytes[i++] = ALPHANUMERIC_CHARS[threeDigitsBits % 10];
		count -= 3;
	}
	if (count == 2) {
		if (BitSourceAvailable(bits) < 7)
		{
			ret = RT_ERR_QR_FORMAT;
			goto ERROR_EXIT;
		}
		// Two digits left over to read, encoded in 7 bits
		twoDigitsBits = BitSourceReadBits(bits,7); 
		if (twoDigitsBits >= 100) {
			ret = RT_ERR_QR_FORMAT;
			goto ERROR_EXIT;
		}
		bytes[i++] = ALPHANUMERIC_CHARS[twoDigitsBits / 10];
		bytes[i++] = ALPHANUMERIC_CHARS[twoDigitsBits % 10];
	} else if (count == 1) {
		if (BitSourceAvailable(bits) < 4)
		{
			ret = RT_ERR_QR_FORMAT;
			goto ERROR_EXIT;
		}
		// One digit left over to read
		digitBits = BitSourceReadBits(bits,4);
		if (digitBits >= 10) {
			ret = RT_ERR_QR_FORMAT;
			goto ERROR_EXIT;

		}
		bytes[i++] = ALPHANUMERIC_CHARS[digitBits];
	}
// 	append(result, bytes, nBytes, StringUtils::ASCII);
//   delete[] bytes;
    diy_memcpy(result,bytes,nBytes);
	*resultLen = nBytes;

ERROR_EXIT:
	FREE(bytes);
	return ret;
}

char DecodedBitStreamParserToAlphaNumericChar(unsigned int value) {
	if (value >= sizeof(ALPHANUMERIC_CHARS)) {
		return 0;
	}
	return ALPHANUMERIC_CHARS[value];
}

int DecodedBitStreamParserDecodeAlphanumericSegment(BitSource *bits_,
                                                     char *result,//  string& result,
                                                     int *resultLen,
                                                     int count,
                                                     BOOL fc1InEffect)
{
	BitSource *bits =bits_;
    int ret = 0,nextTwoCharsBits,i;
    char *bytes = NULL,*r=NULL,*rt=NULL,*s = NULL,*bytest = NULL;
	int bytesSize = 0;

	MALLOC(bytest,char,count<<1);
	MALLOC(r,char,count<<1);
	rt = r;
	bytes = bytest;
	//ostringstream bytes;
	// Read two characters at a time
	while (count > 1) {
		if(BitSourceAvailable(bits) < 11) 
		{
			ret = RT_ERR_QR_FORMAT;
			goto ERROR_EXIT;
		}
		nextTwoCharsBits = BitSourceReadBits(bits,11);
		*bytes++ =  DecodedBitStreamParserToAlphaNumericChar(nextTwoCharsBits / 45);
		*bytes++ = DecodedBitStreamParserToAlphaNumericChar(nextTwoCharsBits % 45);
		count -= 2;
		bytesSize+=2;
	}
	if (count == 1) {
		// special case: one character left
		if (BitSourceAvailable(bits) < 6) {
			ret = RT_ERR_QR_FORMAT;
			goto ERROR_EXIT;
		}
		*bytes++ = DecodedBitStreamParserToAlphaNumericChar(BitSourceReadBits(bits,6));
		bytesSize+=1;
	}
	// See section 6.4.8.1, 6.4.8.2
	s = bytest;//string s = bytes.str();
	rt = r;
	if (fc1InEffect) {
		// We need to massage the result a bit if in an FNC1 mode:
		//ostringstream r;
		for (i = 0; i < bytesSize; i++) {
			if (s[i] != '%') {
				*r++ = s[i];
			} else {
				if (i < bytesSize - 1 && s[i + 1] == '%') {
					// %% is rendered as %
					*r++ = s[i++];
				} else {
					// In alpha mode, % should be converted to FNC1 separator 0x1D
					*r++ = (char)0x1D;
				}
			}
		}
		r = rt;
		diy_memcpy(s,r,bytesSize);
	}
	//append(result, s, StringUtils::ASCII);
	diy_memcpy(result,s,bytesSize);
	*resultLen = bytesSize;

ERROR_EXIT:
	FREE(bytest);
	FREE(r);
	return ret;
}


int DecodedBitStreamParserParseECIValue(BitSource *bits) 
{
    int firstByte,secondByte,secondThirdBytes;

	firstByte = BitSourceReadBits(bits,8);    
	if ((firstByte & 0x80) == 0) {
		// just one byte
		return firstByte & 0x7F;
	}
	if ((firstByte & 0xC0) == 0x80) {
		// two bytes
		secondByte = BitSourceReadBits(bits,8); 
		return ((firstByte & 0x3F) << 8) | secondByte;
	}
	if ((firstByte & 0xE0) == 0xC0) {
		// three bytes
		secondThirdBytes = BitSourceReadBits(bits,16);
		return ((firstByte & 0x1F) << 16) | secondThirdBytes;
	}
	return RT_ERR_QR_FORMAT;
}



int DecodedBitStreamParserDecode(char *bytes,
								 int bytesSize,
                                 QRVersion* version,
                                 QRErrorCorrectLevel *ecLevel,
                                 DecodeHints *hints,unsigned char *pResult,int *pResultLen)
{
    BitSource bits;
    char *byteSegments = NULL;
    CharacterSetECI* currentCharacterSetECI = NULL;
	BOOL fc1InEffect = FALSE;
	QRMode* mode = 0;
	int ret = 0;
	int value,subset,countHanzi,count;
	char *p = (char *)pResult;
	int nPerLen = 0,nLen=0;

	BitSourceInit(&bits,bytes,bytesSize);
    QRModeAllInit();

	currentCharacterSetECI = NULL;
	fc1InEffect = FALSE;
	mode = 0;
	do {
		// While still another segment to read...
		if (BitSourceAvailable(&bits) < 4) {
			// OK, assume we're done. Really, a TERMINATOR mode should have been recorded here
			mode = &QRMODE_TERMINATOR;
		} else {
				mode = QRModeForBits(BitSourceReadBits(&bits,4)); // mode is encoded by 4 bits
				if(mode == NULL)
				{
					ret = RT_ERR_QR_FORMAT;
					goto ERROR_EXIT;
				}
		}
		if (mode != &QRMODE_TERMINATOR) {
			if ((mode == &QRMODE_FNC1_FIRST_POSITION) || (mode == &QRMODE_FNC1_SECOND_POSITION)) {
				// We do little with FNC1 except alter the parsed result a bit according to the spec
				fc1InEffect = TRUE;
			} else if (mode == &QRMODE_STRUCTURED_APPEND) {
				if (BitSourceAvailable(&bits) < 16) {
					//throw FormatException();
					ret = RT_ERR_QR_FORMAT;
					goto ERROR_EXIT;
				}
				// not really supported; all we do is ignore it
				// Read next 8 bits (symbol sequence #) and 8 bits (parity data), then continue
				BitSourceReadBits(&bits,16);
			} else if (mode == &QRMODE_ECI) {
				// Count doesn't apply to ECI
				value = DecodedBitStreamParserParseECIValue(&bits);//parseECIValue(bits);
				currentCharacterSetECI =CharacterSetECIGetByValue(value);
				if (currentCharacterSetECI == 0) {
					ret = RT_ERR_QR_FORMAT;
					goto ERROR_EXIT;
				}
			} else {
				// First handle Hanzi mode which does not start with character count
				if (mode == &QRMODE_HANZI) {
					//chinese mode contains a sub set indicator right after mode indicator
					subset = BitSourceReadBits(&bits,4);
					countHanzi = BitSourceReadBits(&bits,QRModeGetCharacterCountBits(mode,version));
					if (subset == GB2312_SUBSET) {
						QRDecodedBitStreamParserDecodeHanziSegment(&bits,p+nLen,&nPerLen,countHanzi);
						nLen += nPerLen;
					}
				} else {
					// "Normal" QR code modes:
					// How many characters will follow, encoded in this mode?
					count = BitSourceReadBits(&bits,QRModeGetCharacterCountBits(mode,version));
					if (mode == &QRMODE_NUMERIC) {
						QRDecodedBitStreamParserDecodeNumericSegment(&bits,p+nLen,&nPerLen,count);
						nLen += nPerLen;
					} else if (mode == &QRMODE_ALPHANUMERIC) {
						DecodedBitStreamParserDecodeAlphanumericSegment(&bits,p+nLen,&nPerLen,count,fc1InEffect);
						nLen += nPerLen;
					} else if (mode == &QRMODE_BYTE) {
						QRDecodedBitStreamParserDecodeByteSegment(&bits,p+nLen,&nPerLen,count,currentCharacterSetECI, byteSegments, hints);
						nLen += nPerLen;
					} else if (mode == &QRMODE_KANJI) {
						QRDecodedBitStreamParserDecodeKanjiSegment(&bits,p+nLen,&nPerLen,count);
						nLen += nPerLen;
					} else {
						//throw FormatException();
						ret = RT_ERR_QR_FORMAT;
						goto ERROR_EXIT;
					}
				}
			}
		}
	} while (mode != &QRMODE_TERMINATOR);

	//turn Ref<DecoderResult>(new DecoderResult(bytes, Ref<String>(new String(result)), byteSegments, (string)ecLevel));
ERROR_EXIT:
	p[nLen] = '\0';
	*pResultLen = nLen;
	return ret;
}

