#include "BitMatrix.h"
#include "DiyMalloc.h"

int BitMatrixInit(Bitmatrix *pMatrix,int width,int height)
{
	int ret = 0;
	if(width<1 || height<1)
		return RT_ERR_BITMATRIX_INIT;

	pMatrix->width = width;
	pMatrix->height = height;
	pMatrix->rowSize = ((width + bitsPerWord - 1) >> logBits);
    pMatrix->bitsSize = pMatrix->rowSize * height;
	MALLOC(pMatrix->bits,int,pMatrix->bitsSize);

ERROR_EXIT:
	return ret;
}

void BitMatrixUninit(Bitmatrix *pMatrix)
{
	pMatrix->width = 0;
	pMatrix->height = 0;
	pMatrix->rowSize = 0;
    pMatrix->bitsSize = 0;
	FREE(pMatrix->bits);
	pMatrix->bits = NULL;
}

void BitMatrixFlip(Bitmatrix *pMatrix,int x,int y)
{
	int offset = y * pMatrix->rowSize + (x >> logBits);
	pMatrix->bits[offset] ^= 1 << (x & bitsMask);
}

int BitMatrixSetRegion(Bitmatrix *pMatrix,int left,int top,int width,int height)
{
	int right,bottom,y,x,offset;

    if(top<0 || left<0) return RT_ERR_TOPLEFT;
	if(height<1 || width<1) return RT_ERR_WIDTHHEIGHT;

	right = left+width;
	bottom = top + height;
	if(bottom > pMatrix->height || right>pMatrix->width)
		return RT_ERR_FITREGION;

	for (y=top;y<bottom;y++)
	{
        offset = y*pMatrix->rowSize;
		for (x=left;x<right;x++)
		{
			pMatrix->bits[offset+(x>>logBits)] |= 1 << (x & bitsMask);
		}
	}

	return 0;
}

int BitMatrixGetRow(Bitmatrix *pMatrix,int y,BitArray *pRow)
{
    int offset;
	int x;
	if(pRow==NULL)
        return RT_ERR_PARANULL; 
	if(BitArrayGetSize(pRow) < pMatrix->width)
		return RT_ERR_PARAFIT;

    offset = y * pMatrix->rowSize;
	for (x=0;x<pMatrix->rowSize;x++)
	{
		BitArraySetBulk(pRow,x<<logBits,pMatrix->bits[offset+x]);
	}

	return 0;
}

int BitMatrixGetWidth(Bitmatrix *pMatrix)
{
	return pMatrix->width;
}

int BitMatrixGetHeight(Bitmatrix *pMatrix)
{
	return pMatrix->height;
}

int BitMatrixGetTopLeftOnBit(Bitmatrix *pMatrix,int *pTop,int *pLeft)
{
    int bitsOffset=0,x,y,theBits,bit=0;

	while(bitsOffset < pMatrix->bitsSize && pMatrix->bits[bitsOffset]==0){
		bitsOffset++;
	}

	if(bitsOffset == pMatrix->bitsSize)
		return RT_ERR_UNKNOW;

	y = bitsOffset/pMatrix->rowSize;
	x = (bitsOffset%pMatrix->rowSize)<<5;
    
	theBits = pMatrix->bits[bitsOffset];
	bit = 0;
	while ((theBits << (31-bit)) == 0) {
		bit++;
	}
	x += bit;
	*pLeft=x;
	*pTop=y;

    return 0;
}

int BitMatrixGetBottomRightOnBit(Bitmatrix *pMatrix,int *pBottom,int *pRight)
{
	int x,y,bit,theBits;
	int bitsOffset = pMatrix->bitsSize - 1;
	while (bitsOffset >= 0 && pMatrix->bits[bitsOffset] == 0) {
		bitsOffset--;
	}
	if (bitsOffset < 0) {
		return RT_ERR_UNKNOW;
	}
	
	y = bitsOffset / pMatrix->rowSize;
	x = (bitsOffset % pMatrix->rowSize) << 5;
	
	theBits = pMatrix->bits[bitsOffset];
	bit = 31;
	while ((theBits >> bit) == 0) {
		bit--;
	}
	x += bit;
	
	*pRight=x;
	*pBottom=y;
    
	return 0;
}

void BitMatrixSet(Bitmatrix *pMatrix,int x,int y)
{
    int offset = y * pMatrix->rowSize + (x >> logBits);
    pMatrix->bits[offset] |= (1 << (x & bitsMask));
}

BOOL BitMatrixGet(Bitmatrix *pMatrix,int x,int y)
{
    int offset = y * pMatrix->rowSize + (x >> logBits);
    return ((((unsigned)pMatrix->bits[offset]) >> (x & bitsMask)) & 1) != 0;
}