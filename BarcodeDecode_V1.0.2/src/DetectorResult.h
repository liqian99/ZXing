#ifndef __DETECRESULT_H__
#define __DETECRESULT_H__

#include "BitMatrix.h"
#include "ResultPoint.h"

typedef struct _DetectorResult{
    Bitmatrix *bits_;
	int pointsSize_;
    ResultPoint *points_;
}DetectorResult;

void DetectorResultInit(DetectorResult *pDR,Bitmatrix *bits,ResultPoint *points,int pointsSize);
Bitmatrix *DetectorResultGetBits(DetectorResult *pDR);
ResultPoint *DetectorResultGetPoints(DetectorResult *pDR);
int DetectorResultGetPointsSize(DetectorResult *pDR);

#endif