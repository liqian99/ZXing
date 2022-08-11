#include "QRDetector.h"
#include "../DiyMalloc.h"

int QRDetectorDetect(QRDetector *pD,DecodeHints *hints,DetectorResult **ppDR)
{
    QRFinderPatternFinder finder;
    QRFinderPatternInfo info;
	int ret;
    //DetectorResult *pDR = NULL;

	diy_memset(&info,0,sizeof(QRFinderPatternInfo));
	diy_memset(&finder,0,sizeof(QRFinderPatternFinder));
	//³õÊ¼»¯
	ret = QRFinderPatternFinderInit(&finder,pD->image_);
	if(ret == 0)
	{
		ret = QRFinderPatternFinderFind(&finder,hints,&info);
		if(ret == 0)
		{
  			ret = QRDetectorProcessFinderPatternInfo(pD,&info,ppDR);
		}
	}

   
    QRFinderPatternFinderUninit(&finder);
	return ret;
}

int QRDetectorProcessFinderPatternInfo(QRDetector *pD,QRFinderPatternInfo *info,DetectorResult **pDR)
{
	int ret = 0,i,estAlignmentX,estAlignmentY;
    float moduleSize,dimension,modulesBetweenFPCenters,bottomRightX,bottomRightY,correctionToTopLeft;
	QRVersion *provisionalVersion = NULL;
	PerspectiveTransform transform;
	Bitmatrix *pbits = NULL;
    ResultPoint *points = NULL;
	int pointsSize = 0;
	QRAlignmentPattern alignmentPattern;


	QRFinderPattern topLeft = QRFinderPatternInfoGetTopLeft(info);
	QRFinderPattern topRight = QRFinderPatternInfoGetTopRight(info);
	QRFinderPattern bottomLeft = QRFinderPatternInfoGetBottomLeft(info);
	
    diy_memset(&alignmentPattern,0,sizeof(QRAlignmentPattern));
	diy_memset(&transform,0,sizeof(PerspectiveTransform));

	moduleSize = QRDetectorCalculateModuleSize(pD,&topLeft,&topRight,&bottomLeft);
	if (moduleSize < 1.0f) {
		//throw zxing::ReaderException("bad module size");
		ret = RT_ERR_QR_BADMODULESIZE;
		goto ERROR_EXIT;
	}
	dimension = (float)QRDetectorComputeDimension(&topLeft,&topRight,&bottomLeft,moduleSize);
	provisionalVersion = NULL;
	
    provisionalVersion = QRVersionGetProvisonalVersionForDimension(g_pVersionGroup,(int)dimension);
	if(provisionalVersion==NULL)
	{
         ret = RT_ERR_QR_DIMENSIONFORVERSION;
		 goto ERROR_EXIT;
	}
    modulesBetweenFPCenters = (float)QRVersionGetDimensionForVersion(provisionalVersion)-7;
	

	// Anything above version 1 has an alignment pattern
	if (QRVersionGetAlignmentPatternCentersSize(provisionalVersion) > 0)
	{
		
		
		// Guess where a "bottom right" finder pattern would have been
		bottomRightX = topRight.point_.posX_ - topLeft.point_.posX_ + bottomLeft.point_.posX_;
		bottomRightY = topRight.point_.posY_ - topLeft.point_.posY_ + bottomLeft.point_.posY_;
		
		
		// Estimate that alignment pattern is closer by 3 modules
		// from "bottom right" to known top left location
		correctionToTopLeft = 1.0f - 3.0f / (float)modulesBetweenFPCenters;
		estAlignmentX = (int)(topLeft.point_.posX_ + correctionToTopLeft * (bottomRightX - topLeft.point_.posX_));
		estAlignmentY = (int)(topLeft.point_.posY_ + correctionToTopLeft * (bottomRightY - topLeft.point_.posY_));
		
		
		// Kind of arbitrary -- expand search radius before giving up
		for (i = 4; i <= 16; i <<= 1)
		{
			ret = DetectorFindAlignmentInRegion(pD,moduleSize, estAlignmentX, estAlignmentY, (float)i,&alignmentPattern);
			if(ret == 0) break;


		}
		
	}
	
    
    transform = DetectorCreateTransform(&topLeft,&topRight,&bottomLeft,&alignmentPattern,(int)dimension);
	ret = GridSamplerSampleGridA(pD->image_,&pbits,(int)dimension,&transform);
	if(ret != 0)
		goto ERROR_EXIT;

	pointsSize = (alignmentPattern.estimatedModuleSize_ == 0 ? 3 : 4);

	MALLOC(*pDR,DetectorResult,1);
	MALLOC((*pDR)->points_,ResultPoint,pointsSize);

	(*pDR)->points_[0] = bottomLeft.point_;
	(*pDR)->points_[1] = topLeft.point_;
	(*pDR)->points_[2] = topRight.point_;
	if (alignmentPattern.estimatedModuleSize_ != 0) 
	{
		(*pDR)->points_[3] = alignmentPattern.point_;
	}
	
	points = (*pDR)->points_;
	DetectorResultInit(*pDR,pbits,points,pointsSize);

ERROR_EXIT:
	return ret;
}

void DetectorResultUninit(DetectorResult *pDR)
{
	if(pDR)
	{
		FREE(pDR->bits_->bits);
		FREE(pDR->bits_);
		FREE(pDR->points_);
		pDR->pointsSize_ = 0;
		FREE(pDR);
	}

}

PerspectiveTransform DetectorCreateTransform(QRFinderPattern *topLeft, QRFinderPattern *topRight, QRFinderPattern *bottomLeft, QRAlignmentPattern *alignmentPattern, int dimension)
{
	
	float dimMinusThree = (float)dimension - 3.5f;
	float bottomRightX;
	float bottomRightY;
	float sourceBottomRightX;
	float sourceBottomRightY;

	PerspectiveTransform transform;

    diy_memset(&transform,0,sizeof(PerspectiveTransform));

	if (alignmentPattern->estimatedModuleSize_ != 0) {
		bottomRightX = alignmentPattern->point_.posX_;
		bottomRightY = alignmentPattern->point_.posY_;
		sourceBottomRightX = dimMinusThree - 3.0f;
		sourceBottomRightY = sourceBottomRightX;
	} else {
		// Don't have an alignment pattern, just make up the bottom-right point
		bottomRightX = (topRight->point_.posX_ - topLeft->point_.posX_) + bottomLeft->point_.posX_;
		bottomRightY = (topRight->point_.posY_ - topLeft->point_.posY_) + bottomLeft->point_.posY_;
		sourceBottomRightX = dimMinusThree;
		sourceBottomRightY = dimMinusThree;
	}
	

	transform = PerspectiveTransformQuadrilateralToQuadrilateral(3.5f, 3.5f, dimMinusThree, 3.5f, sourceBottomRightX,
		sourceBottomRightY, 3.5f, dimMinusThree, topLeft->point_.posX_, topLeft->point_.posY_, topRight->point_.posX_,
		topRight->point_.posY_, bottomRightX, bottomRightY, bottomLeft->point_.posX_, bottomLeft->point_.posY_);
	
	return transform;
}

int DetectorSmpleGrid(Bitmatrix *image,Bitmatrix **ppBits, int dimension, PerspectiveTransform *transform) {
	//GridSampler &sampler = GridSampler::getInstance();
	return GridSamplerSampleGridA(image, ppBits,dimension, transform);
}

int QRDetectorComputeDimension(QRFinderPattern *topLeft, QRFinderPattern *topRight, QRFinderPattern *bottomLeft,float moduleSize)
{
	int tltrCentersDimension = MathUtilsRound(ResultPointDistanceA(&topLeft->point_, &topRight->point_) / moduleSize);
	int tlblCentersDimension = MathUtilsRound(ResultPointDistanceA(&topLeft->point_, &bottomLeft->point_) / moduleSize);
	int dimension = ((tltrCentersDimension + tlblCentersDimension) >> 1) + 7;
	switch (dimension & 0x03) { // mod 4
	case 0:
		dimension++;
		break;
		// 1? do nothing
	case 2:
		dimension--;
		break;
	case 3:
// 		ostringstream s;
// 		s << "Bad dimension: " << dimension;
// 		throw zxing::ReaderException(s.str().c_str());
		return RT_ERR_QR_BADDIMENSION;
	}
	return dimension;
}

float QRDetectorCalculateModuleSize(QRDetector *pD,QRFinderPattern *topLeft, QRFinderPattern *topRight, QRFinderPattern *bottomLeft) 
{
	// Take the average
	return (QRDetectorCalculateModuleSizeOneWay(pD,&topLeft->point_, &topRight->point_) + QRDetectorCalculateModuleSizeOneWay(pD,&topLeft->point_, &bottomLeft->point_)) / 2.0f;
}

float QRDetectorCalculateModuleSizeOneWay(QRDetector *pD,ResultPoint *pattern, ResultPoint *otherPattern) 
{
	float moduleSizeEst1 = QRDetectorSizeOfBlackWhiteBlackRunBothWays(pD,(int)pattern->posX_, (int)pattern->posY_,
		(int)otherPattern->posX_, (int)otherPattern->posY_);
	float moduleSizeEst2 = QRDetectorSizeOfBlackWhiteBlackRunBothWays(pD,(int)otherPattern->posX_, (int)otherPattern->posY_,
		(int)pattern->posX_, (int)pattern->posY_);
	if (isnan(moduleSizeEst1)) {
		return moduleSizeEst2;
	}
	if (isnan(moduleSizeEst2)) {
		return moduleSizeEst1;
	}
	// Average them, and divide by 7 since we've counted the width of 3 black modules,
	// and 1 white and 1 black module on either side. Ergo, divide sum by 14.
	return (moduleSizeEst1 + moduleSizeEst2) / 14.0f;
}

float QRDetectorSizeOfBlackWhiteBlackRunBothWays(QRDetector *pD,int fromX, int fromY, int toX, int toY)
{
	
	float result = QRDetectorSizeOfBlackWhiteBlackRun(pD,fromX, fromY, toX, toY);
	
	// Now count other way -- don't run off image though of course
	float scale = 1.0f;
	int otherToX = fromX - (toX - fromX);
    int otherToY;

	if (otherToX < 0) {
		scale = (float) fromX / (float) (fromX - otherToX);
		otherToX = 0;
	} else if (otherToX >= (int)BitMatrixGetWidth(pD->image_)) {
		scale = (float) (BitMatrixGetWidth(pD->image_) - 1 - fromX) / (float) (otherToX - fromX);
		otherToX = BitMatrixGetWidth(pD->image_) - 1;
	}
	otherToY = (int) (fromY - (toY - fromY) * scale);
	
	scale = 1.0f;
	if (otherToY < 0) {
		scale = (float) fromY / (float) (fromY - otherToY);
		otherToY = 0;
	} else if (otherToY >= (int)BitMatrixGetHeight(pD->image_)) {
		scale = (float) (BitMatrixGetHeight(pD->image_) - 1 - fromY) / (float) (otherToY - fromY);
		otherToY = BitMatrixGetHeight(pD->image_) - 1;
	}
	otherToX = (int) (fromX + (otherToX - fromX) * scale);
	
	result += QRDetectorSizeOfBlackWhiteBlackRun(pD,fromX, fromY, otherToX, otherToY);
	
	// Middle pixel is double-counted this way; subtract 1
	return result - 1.0f;
}

float QRDetectorSizeOfBlackWhiteBlackRun(QRDetector *pD,int fromX, int fromY, int toX, int toY) 
{
	// Mild variant of Bresenham's algorithm;
	// see http://en.wikipedia.org/wiki/Bresenham's_line_algorithm
	BOOL steep = ABS(toY - fromY) > ABS(toX - fromX);
    int temp,dx,dy,error,xstep,ystep,state,xLimit,realX,realY,x,y;

	if (steep) {
		temp = fromX;
		fromX = fromY;
		fromY = temp;
		temp = toX;
		toX = toY;
		toY = temp;
	}
	
	dx = ABS(toX - fromX);
	dy = ABS(toY - fromY);
	error = -dx >> 1;
	xstep = fromX < toX ? 1 : -1;
	ystep = fromY < toY ? 1 : -1;
	
	// In black pixels, looking for white, first or second time.
	state = 0;
	// Loop up until x == toX, but not beyond
	xLimit = toX + xstep;
	for (x = fromX, y = fromY; x != xLimit; x += xstep) {
		realX = steep ? y : x;
		realY = steep ? x : y;
		
		// Does current pixel mean we have moved white to black or vice versa?
		if (!((state == 1) ^ BitMatrixGet(pD->image_,realX, realY))) {
			if (state == 2) {
				return MathUtilsDistanceB(x, y, fromX, fromY);
			}
			state++;
		}
		
		error += dy;
		if (error > 0) {
			if (y == toY) {
				break;
			}
			y += ystep;
			error -= dx;
		}
	}
	// Found black-white-black; give the benefit of the doubt that the next pixel outside the image
	// is "white" so this last point at (toX+xStep,toY) is the right ending. This is really a
	// small approximation; (toX+xStep,toY+yStep) might be really correct. Ignore this.
	if (state == 2) {
		return MathUtilsDistanceB(toX + xstep, toY, fromX, fromY);
	}
	// else we didn't find even black-white-black; no estimate is really possible
	return 0;
}

int DetectorFindAlignmentInRegion(QRDetector *pD,float overallEstModuleSize, int estAlignmentX, int estAlignmentY,
                                  float allowanceFactor,QRAlignmentPattern *pAP)
{
	// Look for an alignment pattern (3 modules in size) around where it
	// should be
    int allowance,alignmentAreaLeftX,alignmentAreaRightX,alignmentAreaTopY,alignmentAreaBottomY,ret = 0;
    QRAlignmentPatternFinder alignmentFinder;

    diy_memset(&alignmentFinder,0,sizeof(QRAlignmentPatternFinder));

	allowance = (int)(allowanceFactor * overallEstModuleSize);
	alignmentAreaLeftX = MAX(0, estAlignmentX - allowance);
	alignmentAreaRightX = MIN((int)(BitMatrixGetWidth(pD->image_) - 1), estAlignmentX + allowance);
	if (alignmentAreaRightX - alignmentAreaLeftX < overallEstModuleSize * 3) {
// 		throw zxing::ReaderException("region too small to hold alignment pattern");
		ret = RT_ERR_QR_REGIONTOOSMALL;
		goto ERROR_EXIT;
	}
	alignmentAreaTopY = MAX(0, estAlignmentY - allowance);
	alignmentAreaBottomY = MIN((int)(BitMatrixGetHeight(pD->image_) - 1), estAlignmentY + allowance);
	if (alignmentAreaBottomY - alignmentAreaTopY < overallEstModuleSize * 3) {
// 		throw zxing::ReaderException("region too small to hold alignment pattern");
		ret = RT_ERR_QR_REGIONTOOSMALL;
		goto ERROR_EXIT;
	}
	
	ret = QRAlignmentPatternFinderInit(&alignmentFinder,pD->image_,alignmentAreaLeftX, alignmentAreaTopY, alignmentAreaRightX
 		                         -alignmentAreaLeftX, alignmentAreaBottomY-alignmentAreaTopY, overallEstModuleSize);
    if(ret == 0)
	   *pAP = QRAlignmentPatternFinderFind(&alignmentFinder);
ERROR_EXIT:
	QRAlignmentPatternFinderUninit(&alignmentFinder);
	return ret;
}
