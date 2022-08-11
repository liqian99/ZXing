#ifndef __QR_DECODE_H__
#define __QR_DECODE_H__

#include "../BitMatrix.h"
#include "QRBitMatrixParser.h"
#include "QRDataBlock.h"
#include "../ReedSolomonDecoder.h"
#include "QRDecodedBitStreamParser.h"

int QRDecode(Bitmatrix *bits,unsigned char *pResult,int *pResultLen);

#endif