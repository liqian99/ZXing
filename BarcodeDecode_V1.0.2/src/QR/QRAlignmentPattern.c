#include "QRAlignmentPattern.h"

void QRAlignmentPatternInit(QRAlignmentPattern *pAP,float posX,float posY,float estimatedModuleSize)
{
	pAP->point_.posX_ = posX;
	pAP->point_.posY_ = posY;
	pAP->estimatedModuleSize_ = estimatedModuleSize;
}

BOOL AlignmentPatternAboutEquals(QRAlignmentPattern *pAP,float moduleSize, float i, float j)
{
	float moduleSizeDiff;
	if (ABS(i - pAP->point_.posY_) <= moduleSize && ABS(j - pAP->point_.posX_) <= moduleSize)
	{
		moduleSizeDiff = ABS(moduleSize - pAP->estimatedModuleSize_);
		return moduleSizeDiff <= 1.0f || moduleSizeDiff <= pAP->estimatedModuleSize_;
	}
	return FALSE;
}

QRAlignmentPattern AlignmentPatternCombineEstimate(QRAlignmentPattern *pAP,float i, float j, float newModuleSize) 
{
	QRAlignmentPattern result;

	float combinedX = (pAP->point_.posX_ + j) / 2.0f;
	float combinedY = (pAP->point_.posY_ + i) / 2.0f;
	float combinedModuleSize = (pAP->estimatedModuleSize_ + newModuleSize) / 2.0f;
	QRAlignmentPatternInit(&result,combinedX, combinedY, combinedModuleSize);

	return result;
}