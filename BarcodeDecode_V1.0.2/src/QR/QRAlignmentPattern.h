#ifndef __QR_ALIGNMENT_PATTERN_H__
#define __QR_ALIGNMENT_PATTERN_H__

#include "../ResultPoint.h"

typedef struct _QRAlignmentPattern{
	ResultPoint point_;
	float estimatedModuleSize_;
}QRAlignmentPattern;

void QRAlignmentPatternInit(QRAlignmentPattern *pAP,float posX,float posY,float estimatedModuleSize);
BOOL AlignmentPatternAboutEquals(QRAlignmentPattern *pAP,float moduleSize, float i, float j);
QRAlignmentPattern AlignmentPatternCombineEstimate(QRAlignmentPattern *pAP,float i, float j, float newModuleSize);

#endif