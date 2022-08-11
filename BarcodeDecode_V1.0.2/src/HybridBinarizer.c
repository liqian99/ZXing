/**
*  二值化
**/
#include <stdlib.h>
#include <stdio.h>
#include "BaseTypeDef.h"
#include "DiyMalloc.h"
#include "HybridBinarizer.h"
#include "GlobalHistogramBinarizer.h"
/**
*  描述：二值图像每byte高低位互换
*  参数：pBW[in] : 二值图像
*        
*  返回：
**/
void BWHighLowReverse(unsigned char *pBW,int w,int h)
{
	int i,j,m;
	unsigned char tmp[8]={0};
	unsigned char val;
	unsigned char *p = NULL;
    int rowSize = (((w+bitsMask) >> logBits)<<2);

	p = pBW;
	for (j=0;j<h;j++)
	{
		for (i=0;i<rowSize;i++)
		{
			val = *p;
			for(m=0;m<8;m++)
                tmp[m] = (val>>m)&0x01;
			val = 0;
			for(m=0;m<8;m++)
				val += (tmp[m]<<(7-m));
			*p = val;
			p++;
		}
	}
}

#define CAP(val,mina,maxa) (val < mina ? mina : val > maxa ? maxa : val)

// int cap(int value,int min,int max)
// {
//     return value < min ? min : value > max ? max : value;
// }

static void thresholdBlock(char *pGray,
					int graySize,
					int xoffset,
					int yoffset,
					int threshold,
					int stride,
					Bitmatrix *pMatrix)
{
	int y,x,pixel,offset;
	
	for (y=0,offset=yoffset*stride+xoffset;
	y<BLOCK_SIZE;
	y++,offset+=stride)
	{
		for (x=0;x<BLOCK_SIZE;x++)
		{
            if(offset+x < graySize)
			{
                pixel = pGray[offset+x] & 0xFF;
				if(pixel <= threshold)
                    BitMatrixSet(pMatrix,xoffset+x,yoffset+y);
			}
		}
	}
}

static void calculateThresholdForBlock(char *pGray,
								int subWidth,
								int subHeight,
								int width,
								int height,
								int *pBlackPoints,
								Bitmatrix *pMatrix)
{
	int y,maxYOffset;
	int x,maxXOffset;
	int yoffset,xoffset;
    int left,top,suma,z,average;
    int *pblackRow;
    int graySize = width*height;

	maxYOffset = height - BLOCK_SIZE;
	maxXOffset = width - BLOCK_SIZE;

	for (y=0;y<subHeight;y++)
	{
		yoffset = (y << BLOCK_SIZE_POWER);
		if(yoffset > maxYOffset){
			yoffset = maxYOffset;
		}
		for (x=0;x<subWidth;x++)
		{
            xoffset = (x << BLOCK_SIZE_POWER);
			if(xoffset > maxXOffset)
				xoffset = maxXOffset;
			left = CAP(x,2,subWidth-3);
			top = CAP(y,2,subHeight-3);
			suma = 0;
			for (z=-2;z<=2;z++)
			{
                pblackRow = &pBlackPoints[(top+z)*subWidth];
				suma += pblackRow[left-2];
				suma += pblackRow[left-1];
				suma += pblackRow[left];
				suma += pblackRow[left+1];
				suma += pblackRow[left+2];
			}
			average = suma/25;
			thresholdBlock(pGray,graySize,xoffset,yoffset,average,width,pMatrix);
		}
	}

}

static int getBlackPointFromNeighbors(int *pBlackPoints,int subWidth,int x,int y)
{
	return ((pBlackPoints[(y-1)*subWidth+x] + 
		     (pBlackPoints[y*subWidth+x-1]<<1) + 
			 pBlackPoints[(y-1)*subWidth+x-1]) >> 2);
}

static void CalculateBlackPoints(char *pGray,
						  int subW,
						  int subH,
						  int w,
						  int h,
						  int *pBlackPoints)
{
	const int minDynamicRange = 24;
	int y,maxYOffset;
	int x,maxXOffset;
	int yoffset,xoffset;
	int suma=0,mina=0,maxa=0;
	int xx,yy,offset;
	int pixel;
	int average,bp;

	for (y=0;y<subH;y++)
	{
		yoffset = (y << BLOCK_SIZE_POWER);
	    maxYOffset = h - BLOCK_SIZE;
		if(yoffset > maxYOffset)
			yoffset = maxYOffset;

		for(x=0;x<subW;x++)
		{
			xoffset = (x << BLOCK_SIZE_POWER);
			maxXOffset = w - BLOCK_SIZE;
			if(xoffset > maxXOffset)
				xoffset = maxXOffset;

			suma = 0;
			mina = 0xFF;
			maxa = 0;
			
			for(yy=0,offset=yoffset*w+xoffset;
			    yy<BLOCK_SIZE;
				yy++,offset+=w)
			{
				for (xx=0;xx<BLOCK_SIZE;xx++)
				{
					pixel = pGray[offset+xx]&0xFF;
					suma += pixel;
					//still looking for good contrast
					if(pixel < mina)
						mina = pixel;
					if(pixel > maxa)
						maxa = pixel;
				}

                //short-circuit min/max tests once dynamic range is met
				if(maxa-mina > minDynamicRange)
				{
					for(yy++,offset+=w;yy<BLOCK_SIZE;yy++,offset+=w)
					{
						for(xx=0;xx<BLOCK_SIZE;xx+=2)
						{
							suma += pGray[offset+xx] & 0xFF;
							suma += pGray[offset+xx+1] & 0xFF;
						}
					}
				}
			}

            average = (suma >> (BLOCK_SIZE_POWER*2));
			if(maxa-mina <= minDynamicRange)
			{
				average = (mina>>1);
				if(y>0 && x>0){
					bp = getBlackPointFromNeighbors(pBlackPoints,subW,x,y);
					if(mina < bp)
						average = bp;
				}
			}
			pBlackPoints[y*subW+x] = average;
		}/*for(x=0;x<subW;x++)*/
	}/*for (y=0;y<subH;y++)*/
}


/***
*  描述：二值化
*  参数：pGray[in] : 灰度图
*        w[in] : 宽度
*        h[in] : 高度
*        pBW[out] : 二值化图像，需提前分配大小
*  返回：0 代表成功，其它代表失败
***/
int HybridBinarizer(char *pGray,int width,int height,char *pBW)
{
	int subWidth,subHeight;
	Bitmatrix bitMatrix;
    int *pBlackPoints = NULL;
	int ret = -1;

	diy_memset(&bitMatrix,0,sizeof(Bitmatrix));
	if(width >= MINIMUM_DIMENSION && height >= MINIMUM_DIMENSION)
	{
        subWidth = width >> BLOCK_SIZE_POWER;
		if((width & BLOCK_SIZE_MASK) != 0)
			subWidth++;
		subHeight = height >> BLOCK_SIZE_POWER;
		if((height & BLOCK_SIZE_MASK) != 0)
			subHeight++;
#ifdef IMAGE_HELPER
        init_image_helper();
		ClearImage();
		DrawGrayImage(pGray,width,height);
#endif   
		MALLOC(pBlackPoints,int,subWidth*subHeight);
		CalculateBlackPoints(pGray,subWidth,subHeight,width,height,pBlackPoints);

		ret = BitMatrixInit(&bitMatrix,width,height);
        if(ret==0)
		{
			calculateThresholdForBlock(pGray,subWidth,subHeight,width,height,pBlackPoints,&bitMatrix);
			BWHighLowReverse((unsigned char *)bitMatrix.bits,width,height);
			diy_memcpy(pBW,bitMatrix.bits,bitMatrix.bitsSize*sizeof(int));
#ifdef IMAGE_HELPER
		ClearImage();
		DrawBWImage((unsigned char *)pBW,width,height);
#endif
			BitMatrixUninit(&bitMatrix);
		}
	}

ERROR_EXIT:
	FREE(pBlackPoints);

	return ret;
}

int HybridBinarizerGetBlackMatrix(unsigned char *pImgGray,int w,int h,Bitmatrix *pMatrix)
{
	int width = w;
	int height = h;
    char *pLuminances = (char *)pImgGray;
	int *pBlackPoints = NULL;
	int subWidth,subHeight;
	int ret = 0;

    diy_memset(pMatrix->bits,0,pMatrix->bitsSize*sizeof(int));

	if (width >= MINIMUM_DIMENSION && height >= MINIMUM_DIMENSION) {
		subWidth = width >> BLOCK_SIZE_POWER;
		if ((width & BLOCK_SIZE_MASK) != 0) {
			subWidth++;
		}
		subHeight = height >> BLOCK_SIZE_POWER;
		if ((height & BLOCK_SIZE_MASK) != 0) {
			subHeight++;
		}
		MALLOC(pBlackPoints,int,subWidth*subHeight);
		CalculateBlackPoints(pLuminances, subWidth, subHeight, width, height,pBlackPoints);
		
		calculateThresholdForBlock(pLuminances,
				subWidth,
				subHeight,
				width,
				height,
				pBlackPoints,
			    pMatrix);
	} else {
		// If the image is too small, fall back to the global histogram approach.
		 GlobalHistogramBinarizerGetBlackMatrix(pImgGray,w,h,pMatrix);
	}

ERROR_EXIT:
	FREE(pBlackPoints);
    return ret;
}