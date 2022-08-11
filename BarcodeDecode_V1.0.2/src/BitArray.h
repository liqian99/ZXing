#ifndef __BIT_AYYAY_H__
#define __BIT_AYYAY_H__

#include "BaseTypeDef.h"

typedef struct _BitArray
{
    int size;
	int *pbits;
	int bitsSize;
}BitArray;


int BitArrayMakeArraySize(int size);
int BitArrayInit(BitArray *pBitArray,int size_);
void BitArrayUninit(BitArray *pBitArray);
int BitArrayGetSize(BitArray *pBitArray);
void BitArraySetBulk(BitArray *pBitArray,int i,int newBits);
void BitArrayClear(BitArray *pBitArray);
BOOL BitArrayIsRange(BitArray *pBitArray,int start,int end,BOOL value);
int *BitArrayGetBitArray(BitArray *pBitArray);
void BitArrayReverse(BitArray *pBitArray);
int numberOfTrailingZeros(int i);
void BitArraySet(BitArray *pBitArray,int i);
BOOL BitArrayGet(BitArray *pBitArray,int i);
int BitArrayGetNextSet(BitArray *pBitArray,int from);
int BitArrayGetNextUnset(BitArray *pBitArray,int from);

#endif