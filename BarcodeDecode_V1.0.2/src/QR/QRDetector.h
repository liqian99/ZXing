#ifndef __QR_DETECTOR_H__
#define __QR_DETECTOR_H__
#include "../BitMatrix.h"
#include "QRFinderPatternFinder.h"
#include "QRVersion.h"
#include "../DetectorResult.h"
#include "../PerspectiveTransform.h"
#include "../ResultPoint.h"
#include "QRAlignmentPattern.h"
#include "QRAlignmentPatternFinder.h"
#include "../GridSampler.h"
#include "../MathUtils.h"

typedef struct _QRDetector{
    Bitmatrix *image_;
}QRDetector;

int QRDetectorDetect(QRDetector *pD,DecodeHints *hints,DetectorResult **ppDR);
void DetectorResultUninit(DetectorResult *pDR);

int QRDetectorProcessFinderPatternInfo(QRDetector *pD,QRFinderPatternInfo *info,DetectorResult **pDR);
PerspectiveTransform DetectorCreateTransform(QRFinderPattern *topLeft, QRFinderPattern *topRight, QRFinderPattern *bottomLeft, QRAlignmentPattern *alignmentPattern, int dimension);
int DetectorSmpleGrid(Bitmatrix *image,Bitmatrix **ppBits,int dimension, PerspectiveTransform *transform);
int QRDetectorComputeDimension(QRFinderPattern *topLeft, QRFinderPattern *topRight, QRFinderPattern *bottomLeft,float moduleSize);
float QRDetectorCalculateModuleSize(QRDetector *pD,QRFinderPattern *topLeft, QRFinderPattern *topRight, QRFinderPattern *bottomLeft); 
float QRDetectorCalculateModuleSizeOneWay(QRDetector *pD,ResultPoint *pattern, ResultPoint *otherPattern); 
float QRDetectorSizeOfBlackWhiteBlackRunBothWays(QRDetector *pD,int fromX, int fromY, int toX, int toY);
float QRDetectorSizeOfBlackWhiteBlackRun(QRDetector *pD,int fromX, int fromY, int toX, int toY); 
int DetectorFindAlignmentInRegion(QRDetector *pD,float overallEstModuleSize, int estAlignmentX, int estAlignmentY,
                                  float allowanceFactor,QRAlignmentPattern *pAP);

#endif