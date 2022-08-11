#ifndef __QR_ALIGNMENT_PATTERN_FINDER_H__
#define __QR_ALIGNMENT_PATTERN_FINDER_H__
#include "../BitMatrix.h"
#include "QRAlignmentPattern.h"
#include "../DecodeHints.h"
#include "QRFinderPatternInfo.h"
#include "QRFinderPatternFinder.h"

typedef struct _QRAlignmentPatternFinder{
    Bitmatrix *image_;
	int possibleCentersSize_;
	QRAlignmentPattern *possibleCenters_;
	int startX_;
	int startY_;
	int width_;
	int height_;
	float moduleSize_;
}QRAlignmentPatternFinder;

int QRFinderPatternFinderFind(QRFinderPatternFinder *pFPF,DecodeHints *hints,QRFinderPatternInfo *pFPI);
int QRAlignmentPatternFinderInit(QRAlignmentPatternFinder *pAPF,Bitmatrix *image,int startX, int startY, int width,int height,float moduleSize);
void QRAlignmentPatternFinderUninit(QRAlignmentPatternFinder *pAPF);
QRAlignmentPattern QRAlignmentPatternFinderFind(QRAlignmentPatternFinder *pAPF);
							 


#endif