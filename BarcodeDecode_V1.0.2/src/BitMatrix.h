#ifndef __BIT_MATRIX_H__
#define __BIT_MATRIX_H__

#include "BaseTypeDef.h"
#include "BitArray.h"

#define BLOCK_SIZE_POWER 3
#define BLOCK_SIZE (1<<BLOCK_SIZE_POWER)
#define BLOCK_SIZE_MASK (BLOCK_SIZE-1)
#define MINIMUM_DIMENSION (BLOCK_SIZE*5)





typedef struct _BitMatrix{
    int width;
	int height;
	int rowSize;
	int *bits;
	int bitsSize;
}Bitmatrix;

int BitMatrixInit(Bitmatrix *pMatrix,int width,int height);
void BitMatrixUninit(Bitmatrix *pMatrix);
void BitMatrixFlip(Bitmatrix *pMatrix,int x,int y);
int BitMatrixSetRegion(Bitmatrix *pMatrix,int left,int top,int width,int height);
int BitMatrixGetRow(Bitmatrix *pMatrix,int y,BitArray *pRow);
int BitMatrixGetWidth(Bitmatrix *pMatrix);
int BitMatrixGetHeight(Bitmatrix *pMatrix);
int BitMatrixGetTopLeftOnBit(Bitmatrix *pMatrix,int *pTop,int *pLeft);
int BitMatrixGetBottomRightOnBit(Bitmatrix *pMatrix,int *pBottom,int *pRight);
void BitMatrixSet(Bitmatrix *pMatrix,int x,int y);
BOOL BitMatrixGet(Bitmatrix *pMatrix,int x,int y);

#endif /*__BIT_MATRIX_H__*/