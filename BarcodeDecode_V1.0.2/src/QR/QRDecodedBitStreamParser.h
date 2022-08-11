#ifndef __QR_DECODEDBITSTREAMPARSER_H__
#define __QR_DECODEDBITSTREAMPARSER_H__
#include "../BaseTypeDef.h"
#include "../BitSource.h"
#include "../CharacterSetECI.h"
#include "../DecodeHints.h"
#include "QRMode.h"

int DecodedBitStreamParserDecode(char *bytes,
								 int bytesSize,
                                 QRVersion* version,
                                 QRErrorCorrectLevel *ecLevel,
                                 DecodeHints *hints,unsigned char *pResult,int *pResultLen);

#endif