#include "DetectorResult.h"

void DetectorResultInit(DetectorResult *pDR,Bitmatrix *bits,ResultPoint *points,int pointsSize)
{
    pDR->bits_ = bits;
	pDR->points_ = points;
	pDR->pointsSize_ = pointsSize;
}

Bitmatrix *DetectorResultGetBits(DetectorResult *pDR)
{
	return pDR->bits_;
}

ResultPoint *DetectorResultGetPoints(DetectorResult *pDR)
{
	return pDR->points_;
}

int DetectorResultGetPointsSize(DetectorResult *pDR)
{
	return pDR->pointsSize_;
}