#include "QRFinderPattern.h"

void QRFinderPatternInit(QRFinderPattern *pFP,float posX,float posY,float estimagedModuleSize,int count)
{
    pFP->point_.posX_ = posX;
	pFP->point_.posY_ = posY;
    pFP->estimatedModuleSize_ = estimagedModuleSize;
	pFP->count_ = count;
}

int QRFinderPatternGetCount(QRFinderPattern *pFP)
{
    return pFP->count_;
}

float QRFinderPatternGetEstimateModuleSize(QRFinderPattern *pFP)
{
	return pFP->estimatedModuleSize_;
}

void QRFinderPatternIncrementCount(QRFinderPattern *pFP)
{
    pFP->count_++;
}

BOOL QRFinderPatternAboutEquals(QRFinderPattern *pFP,float moduleSize, float i, float j)
{
	float moduleSizeDiff;

	if (ABS(i - pFP->point_.posY_) <= moduleSize && ABS(j - pFP->point_.posX_) <= moduleSize) {
		moduleSizeDiff = ABS(moduleSize - pFP->estimatedModuleSize_);
		return (moduleSizeDiff <= 1.0f || moduleSizeDiff <= pFP->estimatedModuleSize_);
	}
	return FALSE;
}

void QRFinderPatternCombineEstimate(QRFinderPattern *pFPs,QRFinderPattern *pFPd,float i, float j, float newModuleSize)
{
	int combinedCount = pFPs->count_ + 1;
	float combinedX = (pFPs->count_ * pFPs->point_.posX_ + j) / combinedCount;
	float combinedY = (pFPs->count_ * pFPs->point_.posY_ + i) / combinedCount;
	float combinedModuleSize = (pFPs->count_ * QRFinderPatternGetEstimateModuleSize(pFPs) + newModuleSize) / combinedCount;
	QRFinderPatternInit(pFPd,combinedX,combinedY,combinedModuleSize,combinedCount);
}

/**
*  描述：按estimatedModuleSize_升续排列
**/
void QRFinderPatternSortByFurthestFromAverage(QRFinderPattern *pFP,int array_size,float average)
{
	unsigned int i, j, increment; 
	QRFinderPattern temp;
	
	increment = (array_size>>1);
	while (increment > 0)
	{
		for (i=0; i < (unsigned int)array_size; i++)
		{
			j = i;
			temp = pFP[i];
			while ((j >= increment) && (ABS(pFP[j-increment].estimatedModuleSize_-average) > ABS(temp.estimatedModuleSize_-average)))
			{
				pFP[j] = pFP[j - increment];
				j = j - increment;
			}
			pFP[j] = temp;
		}  
		
		if (increment == 2)
			increment = 1;
		else 
			increment = (unsigned int) (increment / 2.2f);
	}
}

void QRFinderPatternEraseIndex(QRFinderPattern *pFP,int *array_size,int index)
{
	int i;
    int nSize = *array_size;

	if(index >= nSize) return;

	for (i=index;i<nSize-1;i++)
	{
		pFP[i] = pFP[i+1];
	}
	nSize--;
	*array_size = nSize;	    
}
/**
*  描述：先按count_降序排列，再按estimatedModuleSize_升续排列
**/
void QRFinderPatternSortByCenter(QRFinderPattern *pFP,int array_size,float average)
{
	unsigned int i, j, increment,ntmp,ncount=1; 
	QRFinderPattern temp;
	
	increment = (array_size>>1);
	while (increment > 0)
	{
		for (i=0; i < (unsigned int)array_size; i++)
		{
			j = i;
			temp = pFP[i];
			while ((j >= increment) && (pFP[j-increment].count_ < temp.count_))
			{
				pFP[j] = pFP[j - increment];
				j = j - increment;
			}
			pFP[j] = temp;
		}  
		
		if (increment == 2)
			increment = 1;
		else 
			increment = (unsigned int) (increment / 2.2f);
	}
    
    ntmp = 0;
	for (i=ntmp+1;i<(unsigned int)array_size;i++)
	{
        if(pFP[i].count_ == pFP[ntmp].count_){
			ncount++;
			continue;
		}else{
            if(ncount > 1){
				QRFinderPatternSortByFurthestFromAverage(&pFP[ntmp],ncount,average);
				ncount = 1;
				ntmp = i;
			}else
				ntmp++;
		}
	}
	
	if(ncount>1)
        QRFinderPatternSortByFurthestFromAverage(&pFP[ntmp],ncount,average);

}

void QRFinderPatternEraseFromTo(QRFinderPattern *pFP,int *array_size,int from,int to)
{
	int i;
    int nSize = *array_size;
	
    if(to == nSize-1)
	{
        nSize -= (to-from);
		nSize--;
	}else{
		nSize -= (to-from);
		nSize--;
		for (i=from;i<nSize;i++)
		{
			pFP[i] = pFP[i+(to-from)+1]; 
		}
	}
	
	*array_size = nSize;	    
}