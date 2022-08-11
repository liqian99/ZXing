#ifndef __QR_BITMATRIX_PARSER_H__
#define __QR_BITMATRIX_PARSER_H__
#include "../BaseTypeDef.h"
#include "../BitMatrix.h"
#include "QRVersion.h"
#include "QRFormatInfo.h"

typedef struct __QRBitMatrixParser{
    Bitmatrix *bitMatrix_;
	QRVersion *parsedVersion_;
	QRFormatInfo *parsedFormatInfo_;
}QRBitMatrixParser;

int QRBitMatrixParserCopyBit(Bitmatrix *pBM,int x,int y,int versionBits);
int QRBitMatrixParserInit(QRBitMatrixParser *pBarser,Bitmatrix *pBM);
void QRBitMatrixParserUninit(QRBitMatrixParser *pBarser);
QRFormatInfo *QRBitMatrixParserReadFormatInfo(QRBitMatrixParser *pBarser);
QRVersion *QRBitMatrixParserReadVersion(QRVersion *pVersionGroup,QRBitMatrixParser *pBarser);
int QRBitMatrixParserReadCodewords(QRBitMatrixParser *pBarser,char *result,int *resultLen);

#endif /*__QR_BITMATRIX_PARSER_H__*/