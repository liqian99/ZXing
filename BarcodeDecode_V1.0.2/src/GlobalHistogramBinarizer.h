#ifndef __GLOBAL_HISTOGRAM_BINARIZER_H__
#define __GLOBAL_HISTOGRAM_BINARIZER_H__

#include "BitArray.h"
#include "BitMatrix.h"

#define  LUMINANCE_BITS 5
#define  LUMINANCE_SHIFT 3     //8 - LUMINANCE_BITS;
#define  LUMINANCE_BUCKETS 32  //1 << LUMINANCE_BITS;

int GlobalHistogramBinarizerGetBlackRow(/*GHBinarizer *pGHB,*/unsigned char *pGrayImg,int w,int h,int y,BitArray *pRow);
int GlobalHistogramBinarizerGetBlackMatrix(unsigned char *pImgGray,int w,int h,Bitmatrix *pMatrix);


#endif /*__GLOBAL_HISTOGRAM_BINARIZER_H__*/