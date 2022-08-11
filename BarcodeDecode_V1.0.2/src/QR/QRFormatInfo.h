#ifndef __QR_FORMATINFO_H__
#define __QR_FORMATINFO_H__
#include "QRErrorCorrectLevel.h"

#define FORMAT_INFO_MASK_QR 0x5412
#define N_FORMAT_INFO_DECODE_LOOKUPS 32

typedef struct _QRFormatInfo{
    QRErrorCorrectLevel *pErrorCorrectionLevel_;
	char dataMask_;
}QRFormatInfo;

void QRFormatInfoInit(QRFormatInfo *pFormatInfo,int formatInfo);
QRErrorCorrectLevel *QRFormatInfoGetErrorCorrectionLevel(QRFormatInfo *pFormatInfo);
char QRFormatInfoGetDataMask(QRFormatInfo *pFormatInfo);
int QRFormatInfoNumBitsDiffering(int a,int b);
int QRFormatInfoDoDecodeFormatInfo(QRFormatInfo *pFormatInfo,int maskedFormatInfo1,int maskedFormatInfo2);
int QRFormatInfoDecodeFormatInfo(QRFormatInfo *pFormatInfo,int maskedFormatInfo1,int maskedFormatInfo2);

#endif