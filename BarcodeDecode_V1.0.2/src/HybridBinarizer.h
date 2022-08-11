#ifndef __HYBRID_BINARIZER_H__
#define __HYBRID_BINARIZER_H__

#include "BitMatrix.h"

int HybridBinarizer(char *pGray,int w,int h,char *pBW);
int HybridBinarizerGetBlackMatrix(unsigned char *pImgGray,int w,int h,Bitmatrix *pMatrix);
void BWHighLowReverse(unsigned char *pBW,int w,int h);

#endif