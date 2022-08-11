#ifndef __QR_CODEREADER_H__
#define __QR_CODEREADER_H__
#include "QRDetector.h"
#include "QRDecode.h"
#include "../HybridBinarizer.h"

int QRCodeReaderBuildAllVariable(int nImgW,int nImgH,Bitmatrix *pBitmatrix);
void QRCodeReaderDestroyAllVariable(Bitmatrix *pBitmatrix);
int QRCodeReaderDecodeBW(Bitmatrix *pBitmatrix,unsigned char *pResult,int *pResultLen);
int QRCodeReaderDecodeGray(unsigned char *pImgGray,int w,int h,unsigned char *pResult,int *pResultLen);

#endif