#include "GlobalHistogramBinarizer.h"
#include "DiyMalloc.h"

static int GlobalHistogramBinarizerEstimateBlackPoint(int *pBuckets,int numBuckets)
{
    int maxBucketCount=0,firstPeak=0,firstPeakSize=0;
	int x;
	int secondPeak=0,secondPeakScore=0;
	int distanceToBiggest,score,temp;
	int bestValley,bestValleyScore,fromFirst;
	
	for (x=0;x<numBuckets;x++)
	{
		if(pBuckets[x] > firstPeakSize)
		{
			firstPeak = x;
			firstPeakSize = pBuckets[x];
		}
		if(pBuckets[x] > maxBucketCount)
			maxBucketCount = pBuckets[x];
	}
	
	for (x=0;x<numBuckets;x++)
	{
		distanceToBiggest = x - firstPeak;
		score = pBuckets[x] * distanceToBiggest * distanceToBiggest;
		if(score > secondPeakScore)
		{
			secondPeak = x;
			secondPeakScore = score;
		}
	}
	
	if(firstPeak > secondPeak)
	{
		temp = firstPeak;
		firstPeak = secondPeak;
		secondPeak = temp;
	}
	
	if(secondPeak - firstPeak <= (numBuckets >> 4))
		return -1;
	
	bestValley = secondPeak - 1;
	bestValleyScore = -1;
	
	for (x=secondPeak-1;x>firstPeak;x--)
	{
        fromFirst = x - firstPeak;
		score = fromFirst*fromFirst*(secondPeak-x)*(maxBucketCount-pBuckets[x]);
		if(score > bestValleyScore)
		{
			bestValley = x;
			bestValleyScore = score;
		}
	}
	
	return (bestValley<<LUMINANCE_SHIFT);
}


/**
*  描述：得到pRow大小，pRow->pbits 需提前分配，大小为w
*  参数：pImg[in]
*        w[in]
*        h[in]
*        y[in] : 第几行，即y行
*        pRow[out] : 
*  返回：void
**/
int GlobalHistogramBinarizerGetBlackRow(unsigned char *pGrayImg,int w,int h,int y,BitArray *pRow)
{
    int width = w;
	unsigned char *pLocalLuminances = NULL;
    int ret = 0,x,pixel;
	int *pLocalBuckets = NULL;
	int blackPoint,left,center,right,luminance;

	MALLOC(pLocalLuminances,unsigned char,width);
    diy_memcpy(pLocalLuminances,pGrayImg+y*width,width);
    MALLOC(pLocalBuckets,int,LUMINANCE_BUCKETS);
    
	for (x=0;x<width;x++)
	{
		pixel = pLocalLuminances[x]&0xff;
		pLocalBuckets[pixel>>LUMINANCE_SHIFT]++;
	}

	blackPoint = GlobalHistogramBinarizerEstimateBlackPoint(pLocalBuckets,LUMINANCE_BUCKETS);
    
	left = pLocalLuminances[0]&0xff;
	center = pLocalLuminances[1]&0xff;

	for (x=1;x<width-1;x++)
	{
		right = pLocalLuminances[x+1]&0xff;
		luminance = ((center << 2) - left - right) >> 1;
		if(luminance < blackPoint)
		{
            BitArraySet(pRow,x);
		}
		left = center;
		center = right;
	}

ERROR_EXIT:
	FREE(pLocalLuminances);
    FREE(pLocalBuckets);
	return ret;
}

/**
*  描述：得到pMatrix大小，pMatrix->pbits 需提前分配，大小为w*h
*  参数：pImg[in]
*        w[in]
*        h[in]
*        pMatrix[out] : 
*  返回：void
**/
int GlobalHistogramBinarizerGetBlackMatrix(unsigned char *pImgGray,int w,int h,Bitmatrix *pMatrix)
{
	int ret = 0;
	int *pLocalBuckets = NULL;
	int x,y,row,right,pixel;
	unsigned char *pLocalLuminances = NULL;
	int blackPoint,offset;

    MALLOC(pLocalLuminances,unsigned char,w);
    MALLOC(pLocalBuckets,int,LUMINANCE_BUCKETS);

    for (y=1;y<5;y++)
    {
		row = h * y/5;
        diy_memcpy(pLocalLuminances,pImgGray+row*w,w);
        right = (w << 2)/5;
		for (x=w/5;x<right;x++)
		{
			pixel = pLocalLuminances[x] & 0xff;
			pLocalBuckets[pixel>>LUMINANCE_SHIFT]++;
		}
    }

	blackPoint = GlobalHistogramBinarizerEstimateBlackPoint(pLocalBuckets,LUMINANCE_BUCKETS);

	for (y=0;y<h;y++)
	{
		offset = y*w;
		for (x=0;x<w;x++)
		{
			pixel = pImgGray[offset + x] & 0xff;
			if(pixel < blackPoint)
				BitMatrixSet(pMatrix,x,y);
		}

	}

ERROR_EXIT:
	FREE(pLocalLuminances);
	FREE(pLocalBuckets);
	return ret;
}

