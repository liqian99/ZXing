#include "QRFinderPatternInfo.h"

void QRFinderPatternInfoInit(QRFinderPatternInfo *pFPI,QRFinderPattern *patternCenters)
{
    pFPI->bottomLeft_ = patternCenters[0];
	pFPI->topLeft_ = patternCenters[1];
	pFPI->topRight_ = patternCenters[2];
}

QRFinderPattern QRFinderPatternInfoGetBottomLeft(QRFinderPatternInfo *pFPI)
{
	return pFPI->bottomLeft_;
}

QRFinderPattern QRFinderPatternInfoGetTopLeft(QRFinderPatternInfo *pFPI)
{
	return pFPI->topLeft_;
}

QRFinderPattern QRFinderPatternInfoGetTopRight(QRFinderPatternInfo *pFPI)
{
	return pFPI->topRight_;
}