#ifndef __QR_FINDER_PATTERN_H__
#define __QR_FINDER_PATTERN_H__

#include "../BaseTypeDef.h"
#include "../ResultPoint.h"

typedef struct __QRFinderPattern
{
    ResultPoint point_;
	float estimatedModuleSize_;
	int count_;
}QRFinderPattern;

void QRFinderPatternInit(QRFinderPattern *pFP,float posX,float posY,float estimagedModuleSize,int count);
int QRFinderPatternGetCount(QRFinderPattern *pFP);
float QRFinderPatternGetEstimateModuleSize(QRFinderPattern *pFP);
void QRFinderPatternIncrementCount(QRFinderPattern *pFP);
BOOL QRFinderPatternAboutEquals(QRFinderPattern *pFP,float moduleSize, float i, float j);
void QRFinderPatternCombineEstimate(QRFinderPattern *pFP,QRFinderPattern *pFPd,float i, float j, float newModuleSize);
void QRFinderPatternSortByFurthestFromAverage(QRFinderPattern *pFP,int array_size,float average);
void QRFinderPatternEraseIndex(QRFinderPattern *pFP,int *array_size,int index);
void QRFinderPatternSortByCenter(QRFinderPattern *pFP,int array_size,float average);
void QRFinderPatternEraseFromTo(QRFinderPattern *pFP,int *array_size,int from,int to);

#endif /*__QR_FINDER_PATTERN_H__*/