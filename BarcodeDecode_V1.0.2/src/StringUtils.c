#include "StringUtils.h"


char const* const PLATFORM_DEFAULT_ENCODING = "UTF-8";
char const* const ASCII = "ASCII";
char const* const SHIFT_JIS = "SHIFT_JIS";
char const* const GB2312 = "GBK";
char const* const EUC_JP = "EUC-JP";
char const* const UTF8 = "UTF-8";
char const* const ISO88591 = "ISO8859-1";
const BOOL ASSUME_SHIFT_JIS = FALSE;

const char *StringUtilsGuessEncoding(char *bytes,int length,DecodeHints *phints)
{
  typedef BOOL boolean;
  // For now, merely tries to distinguish ISO-8859-1, UTF-8 and Shift_JIS,
  // which should be by far the most common encodings.
  boolean canBeISO88591 = TRUE;
  boolean canBeShiftJIS = TRUE;
  boolean canBeUTF8 = TRUE;
  int utf8BytesLeft = 0;
  //int utf8LowChars = 0;
  int utf2BytesChars = 0;
  int utf3BytesChars = 0;
  int utf4BytesChars = 0;
  int sjisBytesLeft = 0;
  //int sjisLowChars = 0;
  int sjisKatakanaChars = 0;
  //int sjisDoubleBytesChars = 0;
  int sjisCurKatakanaWordLength = 0;
  int sjisCurDoubleBytesWordLength = 0;
  int sjisMaxKatakanaWordLength = 0;
  int sjisMaxDoubleBytesWordLength = 0;
  //int isoLowChars = 0;
  //int isoHighChars = 0;
  int isoHighOther = 0;
  int i,value;


  typedef char byte;
  boolean utf8bom = length > 3 &&
    bytes[0] == (byte) 0xEF &&
    bytes[1] == (byte) 0xBB &&
    bytes[2] == (byte) 0xBF;

  for (i = 0;
       i < length && (canBeISO88591 || canBeShiftJIS || canBeUTF8);
       i++) {

    value = bytes[i] & 0xFF;

    // UTF-8 stuff
    if (canBeUTF8) {
      if (utf8BytesLeft > 0) {
        if ((value & 0x80) == 0) {
          canBeUTF8 = FALSE;
        } else {
          utf8BytesLeft--;
        }
      } else if ((value & 0x80) != 0) {
        if ((value & 0x40) == 0) {
          canBeUTF8 = FALSE;
        } else {
          utf8BytesLeft++;
          if ((value & 0x20) == 0) {
            utf2BytesChars++;
          } else {
            utf8BytesLeft++;
            if ((value & 0x10) == 0) {
              utf3BytesChars++;
            } else {
              utf8BytesLeft++;
              if ((value & 0x08) == 0) {
                utf4BytesChars++;
              } else {
                canBeUTF8 = FALSE;
              }
            }
          }
        }
      } //else {
      //utf8LowChars++;
      //}
    }

    // ISO-8859-1 stuff
    if (canBeISO88591) {
      if (value > 0x7F && value < 0xA0) {
        canBeISO88591 = FALSE;
      } else if (value > 0x9F) {
        if (value < 0xC0 || value == 0xD7 || value == 0xF7) {
          isoHighOther++;
        } //else {
        //isoHighChars++;
        //}
      } //else {
      //isoLowChars++;
      //}
    }

    // Shift_JIS stuff
    if (canBeShiftJIS) {
      if (sjisBytesLeft > 0) {
        if (value < 0x40 || value == 0x7F || value > 0xFC) {
          canBeShiftJIS = FALSE;
        } else {
          sjisBytesLeft--;
        }
      } else if (value == 0x80 || value == 0xA0 || value > 0xEF) {
        canBeShiftJIS = FALSE;
      } else if (value > 0xA0 && value < 0xE0) {
        sjisKatakanaChars++;
        sjisCurDoubleBytesWordLength = 0;
        sjisCurKatakanaWordLength++;
        if (sjisCurKatakanaWordLength > sjisMaxKatakanaWordLength) {
          sjisMaxKatakanaWordLength = sjisCurKatakanaWordLength;
        }
      } else if (value > 0x7F) {
        sjisBytesLeft++;
        //sjisDoubleBytesChars++;
        sjisCurKatakanaWordLength = 0;
        sjisCurDoubleBytesWordLength++;
        if (sjisCurDoubleBytesWordLength > sjisMaxDoubleBytesWordLength) {
          sjisMaxDoubleBytesWordLength = sjisCurDoubleBytesWordLength;
        }
      } else {
        //sjisLowChars++;
        sjisCurKatakanaWordLength = 0;
        sjisCurDoubleBytesWordLength = 0;
      }
    }
  }

  if (canBeUTF8 && utf8BytesLeft > 0) {
    canBeUTF8 = FALSE;
  }
  if (canBeShiftJIS && sjisBytesLeft > 0) {
    canBeShiftJIS = FALSE;
  }

  // Easy -- if there is BOM or at least 1 valid not-single byte character (and no evidence it can't be UTF-8), done
  if (canBeUTF8 && (utf8bom || utf2BytesChars + utf3BytesChars + utf4BytesChars > 0)) {
    return UTF8;
  }
  // Easy -- if assuming Shift_JIS or at least 3 valid consecutive not-ascii characters (and no evidence it can't be), done
  if (canBeShiftJIS && (ASSUME_SHIFT_JIS || sjisMaxKatakanaWordLength >= 3 || sjisMaxDoubleBytesWordLength >= 3)) {
    return SHIFT_JIS;
  }
  // Distinguishing Shift_JIS and ISO-8859-1 can be a little tough for short words. The crude heuristic is:
  // - If we saw
  //   - only two consecutive katakana chars in the whole text, or
  //   - at least 10% of bytes that could be "upper" not-alphanumeric Latin1,
  // - then we conclude Shift_JIS, else ISO-8859-1
  if (canBeISO88591 && canBeShiftJIS) {
    return (sjisMaxKatakanaWordLength == 2 && sjisKatakanaChars == 2) || isoHighOther * 10 >= length
      ? SHIFT_JIS : ISO88591;
  }

  // Otherwise, try in order ISO-8859-1, Shift JIS, UTF-8 and fall back to default platform encoding
  if (canBeISO88591) {
    return ISO88591;
  }
  if (canBeShiftJIS) {
    return SHIFT_JIS;
  }
  if (canBeUTF8) {
    return UTF8;
  }
  // Otherwise, we take a wild guess with platform encoding
  return PLATFORM_DEFAULT_ENCODING;
}