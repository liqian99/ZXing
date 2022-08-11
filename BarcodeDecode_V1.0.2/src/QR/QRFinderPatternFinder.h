#ifndef __QR_FINDERPATTERN_FINDER_H__
#define __QR_FINDERPATTERN_FINDER_H__
#include "../BitMatrix.h"
#include "QRFinderPattern.h"
#include "QRFinderPatternInfo.h"
#include "../DecodeHints.h"

typedef struct _QRFinderPatternFinder{
    Bitmatrix *image_;
	QRFinderPattern *possibleCenters_;
	int possibleCentersSize_;
	BOOL hasSkipped_;
}QRFinderPatternFinder;

int QRFinderPatternFinderInit(QRFinderPatternFinder *pFPF,Bitmatrix *image/*,Ref<ResultPointCallback>const& callback*/);
void QRFinderPatternFinderUninit(QRFinderPatternFinder *pFPF);


#endif