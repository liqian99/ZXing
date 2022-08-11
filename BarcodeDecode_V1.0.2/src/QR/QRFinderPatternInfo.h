#ifndef __QR_FINDER_PATTERN_INFO_H__
#define __QR_FINDER_PATTERN_INFO_H__
#include "QRFinderPattern.h"

typedef struct _QRFinderPatternInfo{
    QRFinderPattern bottomLeft_;  
	QRFinderPattern topLeft_;
	QRFinderPattern topRight_;
}QRFinderPatternInfo;

void QRFinderPatternInfoInit(QRFinderPatternInfo *pFPI,QRFinderPattern *patternCenters);
QRFinderPattern QRFinderPatternInfoGetBottomLeft(QRFinderPatternInfo *pFPI);
QRFinderPattern QRFinderPatternInfoGetTopLeft(QRFinderPatternInfo *pFPI);
QRFinderPattern QRFinderPatternInfoGetTopRight(QRFinderPatternInfo *pFPI);

#endif