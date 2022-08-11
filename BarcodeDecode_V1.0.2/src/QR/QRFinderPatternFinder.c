#include "QRFinderPatternFinder.h"
#include "../DiyMalloc.h"
#include "../BaseTypeDef.h"

const int CENTER_QUORUM = 2;
const int MIN_SKIP = 3;
const int MAX_MODULES = 57;
const int MAX_PATTERNS = 60;

float QRFinderPatternFinderCenterFromEnd(int* stateCount,int stateCountSize, int end)
{
	return (float)(end - stateCount[4] - stateCount[3]) - stateCount[2] / 2.0f;
}

BOOL QRFinderPatternFinderFoundPatternCross(int* stateCount,int stateCountSize)
{
	int totalModuleSize = 0,i;
    float moduleSize,maxVariance;

	for (i = 0; i < 5; i++) {
		if (stateCount[i] == 0) {
			return FALSE;
		}
		totalModuleSize += stateCount[i];
	}
	if (totalModuleSize < 7) {
		return FALSE;
	}
	moduleSize = (float)totalModuleSize / 7.0f;
	maxVariance = moduleSize / 1.8f; //2.0f //Changed by liqian on 2016-06-27 from 2.0f to 1.8f
	// Allow less than 50% variance from 1-1-3-1-1 proportions
	return ABS(moduleSize - stateCount[0]) < maxVariance && ABS(moduleSize - stateCount[1]) < maxVariance && ABS(3.0f
		* moduleSize - stateCount[2]) < 3.0f * maxVariance && ABS(moduleSize - stateCount[3]) < maxVariance && ABS(
		moduleSize - stateCount[4]) < maxVariance;
}

float QRFinderPatternFinderCrossCheckVertical(QRFinderPatternFinder *pFPF,unsigned int startI, unsigned int centerJ, int maxCount, int originalStateCountTotal)
{
	
	int maxI = BitMatrixGetHeight(pFPF->image_);
	int stateCount[5] = {0};	
	// Start counting up from center
	int i = startI;
	int stateCountTotal;

	while (i >= 0 && BitMatrixGet(pFPF->image_,centerJ, i)) {
		stateCount[2]++;
		i--;
	}
	if (i < 0) {
		return nan();
	}
	while (i >= 0 && !BitMatrixGet(pFPF->image_,centerJ, i) && stateCount[1] <= maxCount) {
		stateCount[1]++;
		i--;
	}
	// If already too many modules in this state or ran off the edge:
	if (i < 0 || stateCount[1] > maxCount) {
		return nan();
	}
	while (i >= 0 && BitMatrixGet(pFPF->image_,centerJ, i) && stateCount[0] <= maxCount) {
		stateCount[0]++;
		i--;
	}
	if (stateCount[0] > maxCount) {
		return nan();
	}
	
	// Now also count down from center
	i = startI + 1;
	while (i < maxI && BitMatrixGet(pFPF->image_,centerJ, i)) {
		stateCount[2]++;
		i++;
	}
	if (i == maxI) {
		return nan();
	}
	while (i < maxI && !BitMatrixGet(pFPF->image_,centerJ, i) && stateCount[3] < maxCount) {
		stateCount[3]++;
		i++;
	}
	if (i == maxI || stateCount[3] >= maxCount) {
		return nan();
	}
	while (i < maxI && BitMatrixGet(pFPF->image_,centerJ, i) && stateCount[4] < maxCount) {
		stateCount[4]++;
		i++;
	}
	if (stateCount[4] >= maxCount) {
		return nan();
	}
	
	// If we found a finder-pattern-like section, but its size is more than 40% different than
	// the original, assume it's a false positive
	stateCountTotal = stateCount[0] + stateCount[1] + stateCount[2] + stateCount[3] + stateCount[4];
	if (5 * ABS(stateCountTotal - originalStateCountTotal) >= 2 * originalStateCountTotal) {
		return nan();
	}
	
	return QRFinderPatternFinderFoundPatternCross(stateCount,5) ? QRFinderPatternFinderCenterFromEnd(stateCount,5,i) : nan();
}

float QRFinderPatternFinderCrossCheckHorizontal(QRFinderPatternFinder *pFPF,unsigned int startJ,unsigned int centerI,int maxCount,
												int originalStateCountTotal)
{
	
	int maxJ = BitMatrixGetWidth(pFPF->image_);
	int stateCount[5] = {0};
	int stateCountTotal;
	int j = startJ;

	while (j >= 0 && BitMatrixGet(pFPF->image_,j,centerI)) {
		stateCount[2]++;
		j--;
	}
	if (j < 0) {
		return nan();
	}
	while (j >= 0 && !BitMatrixGet(pFPF->image_,j,centerI) && stateCount[1] <= maxCount) {
		stateCount[1]++;
		j--;
	}
	if (j < 0 || stateCount[1] > maxCount) {
		return nan();
	}
	while (j >= 0 && BitMatrixGet(pFPF->image_,j,centerI) && stateCount[0] <= maxCount) {
		stateCount[0]++;
		j--;
	}
	if (stateCount[0] > maxCount) {
		return nan();
	}
	
	j = startJ + 1;
	while (j < maxJ && BitMatrixGet(pFPF->image_,j,centerI)) {
		stateCount[2]++;
		j++;
	}
	if (j == maxJ) {
		return nan();
	}
	while (j < maxJ && !BitMatrixGet(pFPF->image_,j,centerI) && stateCount[3] < maxCount) {
		stateCount[3]++;
		j++;
	}
	if (j == maxJ || stateCount[3] >= maxCount) {
		return nan();
	}
	while (j < maxJ && BitMatrixGet(pFPF->image_,j,centerI) && stateCount[4] < maxCount) {
		stateCount[4]++;
		j++;
	}
	if (stateCount[4] >= maxCount) {
		return nan();
	}
	
	// If we found a finder-pattern-like section, but its size is significantly different than
	// the original, assume it's a false positive
	stateCountTotal = stateCount[0] + stateCount[1] + stateCount[2] + stateCount[3] + stateCount[4];
	if (5 * ABS(stateCountTotal - originalStateCountTotal) >= originalStateCountTotal) {
		return nan();
	}
	
	return QRFinderPatternFinderFoundPatternCross(stateCount,5) ? QRFinderPatternFinderCenterFromEnd(stateCount,5,j) : nan();
}

BOOL QRFinderPatternFinderHandlePossibleCenter(QRFinderPatternFinder *pFPF,int* stateCount,int stateCountSize,unsigned int i,unsigned int j)
{
	float estimatedModuleSize;
	BOOL found;
	unsigned int nmax,index;
	QRFinderPattern center;
    int stateCountTotal;
    float centerJ,centerI;
	QRFinderPattern newPattern;

	diy_memset(&center,0,sizeof(QRFinderPattern));
	diy_memset(&newPattern,0,sizeof(QRFinderPattern));

	stateCountTotal = stateCount[0] + stateCount[1] + stateCount[2] + stateCount[3] + stateCount[4];
	centerJ = QRFinderPatternFinderCenterFromEnd(stateCount, 5,j);
	centerI = QRFinderPatternFinderCrossCheckVertical(pFPF,i,(unsigned int)centerJ, stateCount[2], stateCountTotal); 
	if (!isnan(centerI)) 
	{
		// Re-cross check
		centerJ = QRFinderPatternFinderCrossCheckHorizontal(pFPF,(unsigned int)centerJ, (unsigned int)centerI, stateCount[2], stateCountTotal);
		if (!isnan(centerJ)) 
		{
#ifdef IMAGE_HELPER
             DrawPoint(centerJ,centerI,POINT_SHAPE_PLUS,0xFF); 
#endif
			estimatedModuleSize = (float)stateCountTotal / 7.0f;
			found = FALSE;
			nmax = pFPF->possibleCentersSize_;
			for (index = 0; index < nmax; index++) 
			{
				center = pFPF->possibleCenters_[index];
				// Look for about the same center and module size:
				if (QRFinderPatternAboutEquals(&center,estimatedModuleSize, centerI, centerJ))
				{
					QRFinderPatternCombineEstimate(&center,&(pFPF->possibleCenters_[index]),centerI, centerJ, estimatedModuleSize);
					found = TRUE;
					break;
				}
			}
			if (!found)
			{
				if(pFPF->possibleCentersSize_ < MAX_PATTERNS)
				{
					QRFinderPatternInit(&newPattern,centerJ, centerI, estimatedModuleSize,1);
					pFPF->possibleCenters_[pFPF->possibleCentersSize_] = newPattern;
					pFPF->possibleCentersSize_++;
				}

// 				newPattern(new FinderPattern(centerJ, centerI, estimatedModuleSize));
// 				possibleCenters_.push_back(newPattern);
// 				if (callback_ != 0) {
// 					callback_->foundPossibleResultPoint(*newPattern);
// 				}
// 				QRFinderPatternInit(&newPattern,centerJ,centerI,estimatedModuleSize,1);
			}
			return TRUE;
		}
	}
	return FALSE;
}

int QRFinderPatternFinderFindRowSkip(QRFinderPatternFinder *pFPF)
{
	QRFinderPattern firstConfirmedCenter,center;
	unsigned int nmax = pFPF->possibleCentersSize_,i;
	if (nmax <= 1) {
		return 0;
	}
	diy_memset(&firstConfirmedCenter,0,sizeof(QRFinderPattern));
	diy_memset(&center,0,sizeof(QRFinderPattern));
	//QRFinderPattern firstConfirmedCenter;
	for (i = 0; i < nmax; i++) {
		center = pFPF->possibleCenters_[i];
		if (center.count_ >= CENTER_QUORUM) {
			if (firstConfirmedCenter.count_ == 0) {
				firstConfirmedCenter = center;
			} else {
				// We have two confirmed centers
				// How far down can we skip before resuming looking for the next
				// pattern? In the worst case, only the difference between the
				// difference in the x / y coordinates of the two centers.
				// This is the case where you find top left first. Draw it out.
				pFPF->hasSkipped_ = TRUE;
				return (int)(ABS(firstConfirmedCenter.point_.posX_ - center.point_.posX_) - ABS(firstConfirmedCenter.point_.posY_
					- center.point_.posY_))/2;
			}
		}
	}
	return 0;
}

BOOL QRFinderPatternFinderHaveMultiplyConfirmedCenters(QRFinderPatternFinder *pFPF) 
{
	int confirmedCount = 0;
	float totalModuleSize = 0.0f,average,totalDeviation;
	unsigned int i;
	unsigned int nmax = pFPF->possibleCentersSize_;
    QRFinderPattern pattern;

    diy_memset(&pattern,0,sizeof(QRFinderPattern));

	for (i = 0; i < nmax; i++) {
		pattern = pFPF->possibleCenters_[i];
		if (pattern.count_ >= CENTER_QUORUM) {
			confirmedCount++;
			totalModuleSize += QRFinderPatternGetEstimateModuleSize(&pattern);
		}
	}
	if (confirmedCount < 3) {
		return FALSE;
	}
	// OK, we have at least 3 confirmed centers, but, it's possible that one is a "false positive"
	// and that we need to keep looking. We detect this by asking if the estimated module sizes
	// vary too much. We arbitrarily say that when the total deviation from average exceeds
	// 5% of the total module size estimates, it's too much.
	average = totalModuleSize / nmax;
	totalDeviation = 0.0f;
	for (i = 0; i < nmax; i++) {
		pattern = pFPF->possibleCenters_[i];
		totalDeviation += ABS(QRFinderPatternGetEstimateModuleSize(&pattern) - average);
	}
	return totalDeviation <= 0.05f * totalModuleSize;
}

/**
*  描述：返回最合适的Pattern
*  参数：pFPF[in]
*        pFP[out] : 返回的Pattern
*        pFPSize[out] : 返回的Pattern个数
*  返回：0 代表成功，其它代表失败
**/
int QRFinderPatternFinderSelectBestPatterns(QRFinderPatternFinder *pFPF,QRFinderPattern *pFP,int *pFPSize) 
{
	int i;
	float totalModuleSize,square,size,average,stdDev,limit;
	int startSize = pFPF->possibleCentersSize_;
   
	

	if (startSize < 2/*3 20181030*/) {
		// Couldn't find enough finder patterns
		return RT_ERR_QR_ENOUGHPATTERN;
	}
	
	// Filter outlier possibilities whose module size is too different
	if (startSize > 3) {
		// But we can only afford to do so if we have at least 4 possibilities to choose from
		totalModuleSize = 0.0f;
		square = 0.0f;
		for (i = 0; i < startSize; i++) {
			size = QRFinderPatternGetEstimateModuleSize(&pFPF->possibleCenters_[i]);
			totalModuleSize += size;
			square += size * size;
		}
		average = totalModuleSize / (float) startSize;
		stdDev = (float)sqrt(square / startSize - average * average);
		
		//sort(possibleCenters_.begin(), possibleCenters_.end(), FurthestFromAverageComparator(average));
		QRFinderPatternSortByFurthestFromAverage(pFPF->possibleCenters_,pFPF->possibleCentersSize_,average);

		limit = MAX(0.2f * average, stdDev);
		
		for (i = 0; i <pFPF->possibleCentersSize_ && pFPF->possibleCentersSize_ > 3; i++) {
			if (ABS(QRFinderPatternGetEstimateModuleSize(&pFPF->possibleCenters_[i]) - average) > limit) {
				//possibleCenters_.erase(possibleCenters_.begin()+i);
				QRFinderPatternEraseIndex(pFPF->possibleCenters_,&pFPF->possibleCentersSize_,i);
				i--;
			}
		}
	}
	
	if (pFPF->possibleCentersSize_ > 3) {
		// Throw away all but those first size candidate points we found.
		totalModuleSize = 0.0f;
		for (i = 0; i < pFPF->possibleCentersSize_; i++) {
			size = QRFinderPatternGetEstimateModuleSize(&pFPF->possibleCenters_[i]);
			totalModuleSize += size;
		}
		average = totalModuleSize / (float) pFPF->possibleCentersSize_;
		//sort(possibleCenters_.begin(), possibleCenters_.end(), CenterComparator(average));
		QRFinderPatternSortByCenter(pFPF->possibleCenters_,pFPF->possibleCentersSize_,average);
	}
	
	if (pFPF->possibleCentersSize_ > 3) {
		//possibleCenters_.erase(possibleCenters_.begin()+3,possibleCenters_.end());
		QRFinderPatternEraseFromTo(pFPF->possibleCenters_,&pFPF->possibleCentersSize_,3,pFPF->possibleCentersSize_-1);
	}
	
	pFP[0] = pFPF->possibleCenters_[0];
	pFP[1] = pFPF->possibleCenters_[1];
	pFP[2] = pFPF->possibleCenters_[2];
   
	*pFPSize = 3;

	return 0;
}

float FinderPatternFinderDistance(QRFinderPattern *p1, QRFinderPattern *p2) {
	float dx = p1->point_.posX_ - p2->point_.posX_;
	float dy = p1->point_.posY_ - p2->point_.posY_;
	return (float)sqrt(dx * dx + dy * dy);
}

/**
*  描述：最合适的Pattern排序
*  参数：patterns[out] : 返回的Pattern
*  返回：0 代表成功，其它代表失败
**/
int QRFinderPatternFinderOrderBestPatterns(QRFinderPattern *patterns)
{
	// Find distances between pattern centers
	float abDistance = FinderPatternFinderDistance(&patterns[0], &patterns[1]);
	float bcDistance = FinderPatternFinderDistance(&patterns[1], &patterns[2]);
	float acDistance = FinderPatternFinderDistance(&patterns[0], &patterns[2]);
	
	QRFinderPattern topLeft;
	QRFinderPattern topRight;
	QRFinderPattern bottomLeft;
	QRFinderPattern temp;
	// Assume one closest to other two is top left;
	// topRight and bottomLeft will just be guesses below at first
	if (bcDistance >= abDistance && bcDistance >= acDistance) {
		topLeft = patterns[0];
		topRight = patterns[1];
		bottomLeft = patterns[2];
	} else if (acDistance >= bcDistance && acDistance >= abDistance) {
		topLeft = patterns[1];
		topRight = patterns[0];
		bottomLeft = patterns[2];
	} else {
		topLeft = patterns[2];
		topRight = patterns[0];
		bottomLeft = patterns[1];
	}
	
	// Use cross product to figure out which of other1/2 is the bottom left
	// pattern. The vector "top-left -> bottom-left" x "top-left -> top-right"
	// should yield a vector with positive z component
	if ((bottomLeft.point_.posY_ - topLeft.point_.posY_) * (topRight.point_.posX_ - topLeft.point_.posX_) < (bottomLeft.point_.posX_
		- topLeft.point_.posX_) * (topRight.point_.posY_ - topLeft.point_.posY_)) {
		temp = topRight;
		topRight = bottomLeft;
		bottomLeft = temp;
	}
	
	patterns[0] = bottomLeft;
	patterns[1] = topLeft;
	patterns[2] = topRight;
	return 0;
}

int QRFinderPatternFinderInit(QRFinderPatternFinder *pFPF,Bitmatrix *image/*,Ref<ResultPointCallback>const& callback*/) 
{
	int ret = 0;

    pFPF->image_ = image;
	pFPF->possibleCentersSize_ = 0;
	pFPF->hasSkipped_ = FALSE;

	MALLOC(pFPF->possibleCenters_,QRFinderPattern,MAX_PATTERNS);

ERROR_EXIT:
	return ret;
}

void QRFinderPatternFinderUninit(QRFinderPatternFinder *pFPF)
{
	if(pFPF)
	{
		FREE(pFPF->possibleCenters_);
	}
	pFPF->possibleCentersSize_ = 0;
}

int QRFinderPatternFinderFind(QRFinderPatternFinder *pFPF,DecodeHints *hints,QRFinderPatternInfo *pFPI)
{
  BOOL tryHarder = DecodeHintsGetTryHarder(hints);
  int maxI = BitMatrixGetHeight(pFPF->image_);
  int maxJ = BitMatrixGetWidth(pFPF->image_);   

  // We are looking for black/white/black/white/black modules in
  // 1:1:3:1:1 ratio; this tracks the number of such modules seen so far

  // As this is used often, we use an integer array instead of vector
  int stateCount[5] = {0};
  BOOL done = FALSE,confirmed;


  // Let's assume that the maximum version QR Code we support takes up 1/4
  // the height of the image, and then account for the center being 3
  // modules in size. This gives the smallest number of pixels the center
  // could be, so skip this often. When trying harder, look for all
  // QR versions regardless of how dense they are.
  int iSkip = (3 * maxI) / (4 * MAX_MODULES);
  int i,j,currentState,rowSkip;
  Bitmatrix *matrix = NULL;
  QRFinderPattern patternInfo[3];
  int patternInfoSize = 0,ret = -1;
  //QRFinderPatternInfo result;
#ifdef _DEBUG
  FILE *fp = NULL;
  int m = 0;
#endif

  if (iSkip < MIN_SKIP || tryHarder) {
      iSkip = MIN_SKIP;
  }

  // This is slightly faster than using the Ref. Efficiency is important here
  matrix = pFPF->image_;
#ifdef _DEBUG
  fp = fopen("G:\\2.txt","w");
  for (m=0;m<matrix->bitsSize;m++)
  {
	  fprintf(fp,"m=%d  %d\r\n",m,matrix->bits[m]);
  }
  fclose(fp);
#endif

  for (i = iSkip - 1; i < maxI && !done; i += iSkip)
  {
    // Get a row of black/white values
    stateCount[0] = 0;
    stateCount[1] = 0;
    stateCount[2] = 0;
    stateCount[3] = 0;
    stateCount[4] = 0;
    currentState = 0;
    for (j = 0; j < maxJ; j++)
	{
      if (BitMatrixGet(matrix,j,i))
	  {
        // Black pixel
        if ((currentState & 1) == 1) { // Counting white pixels
          currentState++;
        }
        stateCount[currentState]++;
      } else { // White pixel
        if ((currentState & 1) == 0)   // Counting black pixels
		{
          if (currentState == 4) { // A winner?


            if (QRFinderPatternFinderFoundPatternCross(stateCount,5))
			{ // Yes
#ifdef IMAGE_HELPER
               DrawPoint(j,i,POINT_SHAPE_PLUS,0xFF);
#endif
              confirmed = QRFinderPatternFinderHandlePossibleCenter(pFPF,stateCount,5,i,j);
              if(confirmed)
			  {
                // Start examining every other line. Checking each line turned out to be too
                // expensive and didn't improve performance.
                iSkip = 2;
                if (pFPF->hasSkipped_)
				{
                  done = QRFinderPatternFinderHaveMultiplyConfirmedCenters(pFPF);
                } else {
                  rowSkip = QRFinderPatternFinderFindRowSkip(pFPF);
                  if (rowSkip > stateCount[2]) 
				  {
                    // Skip rows between row of lower confirmed center
                    // and top of presumed third confirmed center
                    // but back up a bit to get a full chance of detecting
                    // it, entire width of center of finder pattern

                    // Skip by rowSkip, but back off by stateCount[2] (size
                    // of last center of pattern we saw) to be conservative,
                    // and also back off by iSkip which is about to be
                    // re-added
                    i += rowSkip - stateCount[2] - iSkip;
                    j = maxJ - 1;
                  }
                }
              } else {
                stateCount[0] = stateCount[2];
                stateCount[1] = stateCount[3];
                stateCount[2] = stateCount[4];
                stateCount[3] = 1;
                stateCount[4] = 0;
                currentState = 3;
                continue;
              }
              // Clear state to start looking again
              currentState = 0;
              stateCount[0] = 0;
              stateCount[1] = 0;
              stateCount[2] = 0;
              stateCount[3] = 0;
              stateCount[4] = 0;
            } else { // No, shift counts back by two
              stateCount[0] = stateCount[2];
              stateCount[1] = stateCount[3];
              stateCount[2] = stateCount[4];
              stateCount[3] = 1;
              stateCount[4] = 0;
              currentState = 3;
            }
          } else {
            stateCount[++currentState]++;
          }
        } else { // Counting white pixels
          stateCount[currentState]++;
        }
      }
    }
    if (QRFinderPatternFinderFoundPatternCross(stateCount,5))
	{
      confirmed = QRFinderPatternFinderHandlePossibleCenter(pFPF,stateCount,5,i, maxJ);
      if (confirmed) 
	  {
        iSkip = stateCount[0];
        if (pFPF->hasSkipped_)
		{
          // Found a third one
          done = QRFinderPatternFinderHaveMultiplyConfirmedCenters(pFPF);
        }
      }
    }
  }


  ret = QRFinderPatternFinderSelectBestPatterns(pFPF,&patternInfo[0],&patternInfoSize);
  if(ret == 0)
  {
      QRFinderPatternFinderOrderBestPatterns(&patternInfo[0]);

	  QRFinderPatternInfoInit(pFPI,&patternInfo[0]);
  }

 

  return ret;
}