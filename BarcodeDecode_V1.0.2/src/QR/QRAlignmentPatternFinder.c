#include "QRAlignmentPatternFinder.h"
#include "../DiyMalloc.h"

#define MAX_PossibleCentersSize  36

float AlignmentPatternFinderCenterFromEnd(int *stateCount,int stateCountSize,int end)
{
	if(stateCountSize < 3)
	{
		return 0;
	}
	return (float)(end - stateCount[2]) - stateCount[1] / 2.0f;
}

BOOL AlignmentPatternFinderFoundPatternCross(QRAlignmentPatternFinder *pAPF,int *stateCount,int stateCountSize)
{
	int i;
	float maxVariance;
	if(stateCountSize < 3)
		return FALSE;
	
	maxVariance	= pAPF->moduleSize_ / 2.0f;
	for (i = 0; i < 3; i++) {
		if (ABS(pAPF->moduleSize_ - stateCount[i]) >= maxVariance) {
			return FALSE;
		}
	}
	return TRUE;
}

float AlignmentPatternFinderCrossCheckVertical(QRAlignmentPatternFinder *pAPF,int startI, int centerJ, int maxCount,
                                               int originalStateCountTotal) 
{
	int maxI = BitMatrixGetHeight(pAPF->image_);
	int stateCount[3] = {0};
	int stateCountTotal;
	// Start counting up from center
	int i = startI;
	while (i >= 0 && BitMatrixGet(pAPF->image_,centerJ, i) && stateCount[1] <= maxCount) {
		stateCount[1]++;
		i--;
	}
	// If already too many modules in this state or ran off the edge:
	if (i < 0 || stateCount[1] > maxCount) {
		return 0.0f;
	}
	while (i >= 0 && !BitMatrixGet(pAPF->image_,centerJ, i) && stateCount[0] <= maxCount) {
		stateCount[0]++;
		i--;
	}
	if (stateCount[0] > maxCount) {
		return 0.0f;
	}
	
	// Now also count down from center
	i = startI + 1;
	while (i < maxI && BitMatrixGet(pAPF->image_,centerJ, i) && stateCount[1] <= maxCount) {
		stateCount[1]++;
		i++;
	}
	if (i == maxI || stateCount[1] > maxCount) {
		return 0.0f;
	}
	while (i < maxI && !BitMatrixGet(pAPF->image_,centerJ, i) && stateCount[2] <= maxCount) {
		stateCount[2]++;
		i++;
	}
	if (stateCount[2] > maxCount) {
		return 0.0f;
	}
	
	stateCountTotal = stateCount[0] + stateCount[1] + stateCount[2];
	if (5 * ABS(stateCountTotal - originalStateCountTotal) >= 2 * originalStateCountTotal) {
		return 0.0f;
	}
	
	return AlignmentPatternFinderFoundPatternCross(pAPF,stateCount,3) ? AlignmentPatternFinderCenterFromEnd(stateCount,3,i) : 0.0f;
}

QRAlignmentPattern AlignmentPatternFinderHandlePossibleCenter(QRAlignmentPatternFinder *pAPF,int *stateCount,int stateCountSize,int i, int j) 
{
    float estimatedModuleSize,centerJ,centerI;
    int nmax,index;
	QRAlignmentPattern center,tmp,result;
    int stateCountTotal;

	diy_memset(&center,0,sizeof(QRAlignmentPattern));
	diy_memset(&tmp,0,sizeof(QRAlignmentPattern));
	diy_memset(&result,0,sizeof(QRAlignmentPattern));

	stateCountTotal = stateCount[0] + stateCount[1] + stateCount[2];
	centerJ = AlignmentPatternFinderCenterFromEnd(stateCount,stateCountSize,j);
	centerI = AlignmentPatternFinderCrossCheckVertical(pAPF,i, (int)centerJ, 2 * stateCount[1], stateCountTotal);
	if (!isnan(centerI)) {
		estimatedModuleSize = (float)(stateCount[0] + stateCount[1] + stateCount[2]) / 3.0f;
		nmax = pAPF->possibleCentersSize_;
		for (index = 0; index < nmax; index++) 
		{
			center = pAPF->possibleCenters_[index];
			// Look for about the same center and module size:
			if (AlignmentPatternAboutEquals(&center,estimatedModuleSize, centerI, centerJ))
			{
				return AlignmentPatternCombineEstimate(&center,centerI, centerJ, estimatedModuleSize);
			}
		}
		QRAlignmentPatternInit(&tmp,centerJ, centerI, estimatedModuleSize);
		// Hadn't found this before; save it
		//tmp->retain();
		//possibleCenters_->push_back(tmp);
		//if (callback_ != 0) {
		//	callback_->foundPossibleResultPoint(*tmp);
		//}
		pAPF->possibleCenters_[pAPF->possibleCentersSize_] = tmp;
		pAPF->possibleCentersSize_ += 1;
		
	}
	//Ref<AlignmentPattern> result;
	return result;
}

int QRAlignmentPatternFinderInit(QRAlignmentPatternFinder *pAPF,Bitmatrix *image,int startX, int startY, int width,
                                               int height, float moduleSize
                                               /*Ref<ResultPointCallback>const& callback*/) 
{
	int ret = 0;

	pAPF->image_ = image;
	MALLOC(pAPF->possibleCenters_,QRAlignmentPattern,MAX_PossibleCentersSize);
	pAPF->possibleCentersSize_ = 0;
	pAPF->startX_ = startX;
	pAPF->startY_ = startY;
	pAPF->width_ = width;
	pAPF->height_ = height;
	pAPF->moduleSize_ = moduleSize;

ERROR_EXIT:
	return ret;
}

void QRAlignmentPatternFinderUninit(QRAlignmentPatternFinder *pAPF)
{
	if(pAPF)
	{
		FREE(pAPF->possibleCenters_);
		pAPF->possibleCentersSize_ = 0;
	}
}

QRAlignmentPattern QRAlignmentPatternFinderFind(QRAlignmentPatternFinder *pAPF)
{
	int maxJ = pAPF->startX_ + pAPF->width_;
	int middleI = pAPF->startY_ + (pAPF->height_ >> 1);
	//      Ref<BitArray> luminanceRow(new BitArray(width_));
	// We are looking for black/white/black modules in 1:1:1 ratio;
	// this tracks the number of black/white/black modules seen so far
	//vector<int> stateCount(3, 0);
	int stateCount[3] = {0};
	int iGen,i,j,currentState;
    QRAlignmentPattern confirmed,center;

	memset(&confirmed,0,sizeof(QRAlignmentPattern));
	memset(&center,0,sizeof(QRAlignmentPattern));

	for (iGen = 0; iGen < pAPF->height_; iGen++) {
		// Search from middle outwards
		i = middleI + ((iGen & 0x01) == 0 ? ((iGen + 1) >> 1) : -((iGen + 1) >> 1));
		//        image_->getBlackRow(i, luminanceRow, startX_, width_);
		stateCount[0] = 0;
		stateCount[1] = 0;
		stateCount[2] = 0;
		j = pAPF->startX_;
		// Burn off leading white pixels before anything else; if we start in the middle of
		// a white run, it doesn't make sense to count its length, since we don't know if the
		// white run continued to the left of the start point
		while (j < maxJ && !BitMatrixGet(pAPF->image_,j,i)) {
			j++;
		}
		currentState = 0;
		while (j < maxJ) {
			if (BitMatrixGet(pAPF->image_,j,i)) {
				// Black pixel
				if (currentState == 1) { // Counting black pixels
					stateCount[currentState]++;
				} else { // Counting white pixels
					if (currentState == 2) { // A winner?
						if (AlignmentPatternFinderFoundPatternCross(pAPF,stateCount,3)) { // Yes
							confirmed = AlignmentPatternFinderHandlePossibleCenter(pAPF,stateCount,3,i,j);
							if (confirmed.estimatedModuleSize_ != 0) {
								return confirmed;
							}
						}
						stateCount[0] = stateCount[2];
						stateCount[1] = 1;
						stateCount[2] = 0;
						currentState = 1;
					} else {
						stateCount[++currentState]++;
					}
				}
			} else { // White pixel
				if (currentState == 1) { // Counting black pixels
					currentState++;
				}
				stateCount[currentState]++;
			}
			j++;
		}
		if (AlignmentPatternFinderFoundPatternCross(pAPF,stateCount,3)) {
			confirmed = (AlignmentPatternFinderHandlePossibleCenter(pAPF,stateCount,3,i, maxJ));
			if (confirmed.estimatedModuleSize_ != 0) {
				return confirmed;
			}
		}
		
	}
	
	// Hmm, nothing we saw was observed and confirmed twice. If we had
	// any guess at all, return it.
	if (pAPF->possibleCentersSize_ > 0) {
		center = pAPF->possibleCenters_[0];
		return center;
	}
// 	CLog::WriteLog(CLog::LEVEL_INFO, "%s(%d)---<%s>\n",__FILE__, __LINE__, __FUNCTION__); printf("%s(%d)---<%s>\n",__FILE__, __LINE__, __FUNCTION__);
// 	throw zxing::ReaderException("Could not find alignment pattern");
	return center;
}
