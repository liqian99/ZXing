#include "BitArray.h"
#include "DiyMalloc.h"


int BitArrayMakeArraySize(int size)
{
    return ((size+bitsPerWord-1)>>logBits);
}

int BitArrayInit(BitArray *pBitArray,int size_)
{
	int ret = RT_ERR_BITARRAY_INIT;
	pBitArray->size = size_;
	pBitArray->bitsSize = BitArrayMakeArraySize(size_);
    MALLOC(pBitArray->pbits,int,pBitArray->bitsSize);
    ret = 0;
ERROR_EXIT:
	return ret;
}

void BitArrayUninit(BitArray *pBitArray)
{
    pBitArray->size = 0;
	pBitArray->bitsSize=0;
	FREE(pBitArray->pbits);
}

int BitArrayGetSize(BitArray *pBitArray)
{
	return pBitArray->size;
}

void BitArraySetBulk(BitArray *pBitArray,int i,int newBits)
{
    pBitArray->pbits[i>>logBits] = newBits;
}

void BitArrayClear(BitArray *pBitArray)
{
    int i=0;
	for (i=0;i<pBitArray->bitsSize;i++)
	{
		pBitArray->pbits[i] = 0;
	}
}

BOOL BitArrayIsRange(BitArray *pBitArray,int start,int end,BOOL value)
{
    int firstInt,lastInt,i,firstBit,lastBit,mask,j;
	if(end < start) return FALSE;
	if(end == start) return TRUE;  //empty range matches
	end--; // will be easier to treat this as the last actually set bit --inclusive
    firstInt = start>>logBits;
	lastInt = end>>logBits;
	for (i=firstInt;i<=lastInt;i++)
	{
		firstBit = i>firstInt?0:start & bitsMask;
		lastBit = i<lastInt?(bitsPerWord-1):end & bitsMask;
		if(firstBit==0 && lastBit==(bitsPerWord-1)){
			mask = -1;
		}else{
			mask = 0;
			for (j=firstBit;j<lastBit;j++)
			{
				mask |= 1<<j;
			}
		}

		if((pBitArray->pbits[i] & mask) != (value?mask:0)){
			return FALSE;
		}
	}
    return TRUE;
}

int *BitArrayGetBitArray(BitArray *pBitArray)
{
	return pBitArray->pbits;
}

void BitArrayReverse(BitArray *pBitArray)
{
    int *pNewBits = NULL;
	int size = pBitArray->size;
	int i,ret;

	MALLOC(pNewBits,int,pBitArray->bitsSize);
	
	for (i=0;i<size;i++)
	{
        if(BitArrayGet(pBitArray,size-i-1))
			pNewBits[i>>logBits] |= (i & bitsMask);
	}

	diy_memcpy(pBitArray->pbits,pNewBits,pBitArray->bitsSize*sizeof(int));

ERROR_EXIT:
	FREE(pNewBits);
}


int numberOfTrailingZeros(int i)
{
	int y;
	int n = 31;

    if (i == 0) return 32;
    y = i <<16; if (y != 0) { n = n -16; i = y; }
    y = i << 8; if (y != 0) { n = n - 8; i = y; }
    y = i << 4; if (y != 0) { n = n - 4; i = y; }
    y = i << 2; if (y != 0) { n = n - 2; i = y; }
    return n - (((unsigned int)(i << 1)) >> 31);
}

void BitArraySet(BitArray *pBitArray,int i)
{
    pBitArray->pbits[i>>logBits] |= 1<<(i&bitsMask);
}

BOOL BitArrayGet(BitArray *pBitArray,int i)
{
	return (pBitArray->pbits[i>>logBits] & (1<<(i & bitsMask))) !=0;
}

int BitArrayGetNextSet(BitArray *pBitArray,int from)
{
	int bitsOffset;
	int currentBits;
	int result;
    int size = pBitArray->size;

	if(from >= size)
		return size;

	bitsOffset = from >> logBits;
	currentBits = pBitArray->pbits[bitsOffset];
    //mask off lesser bits first
	currentBits &= ~((1 << (from & bitsMask))-1);
	while (currentBits == 0)
	{
		if(++bitsOffset == pBitArray->bitsSize)
			return size;
		currentBits = pBitArray->pbits[bitsOffset];
	}
	result = (bitsOffset << logBits) + numberOfTrailingZeros(currentBits);
	return result > size ? size : result;
}

int BitArrayGetNextUnset(BitArray *pBitArray,int from)
{
    int size = pBitArray->size;
	int bitsOffset,currentBits;
    int result;

	if(from >= size)
		return size;
	bitsOffset = from >> logBits;
	currentBits = ~pBitArray->pbits[bitsOffset];
	//mask off lesser bits first
	currentBits &= ~((1 << (from & bitsMask)) - 1);
	while (currentBits == 0)
	{
		if(++bitsOffset == pBitArray->bitsSize)
			return size;
		currentBits = ~pBitArray->pbits[bitsOffset];
	}
	result = (bitsOffset << logBits) + numberOfTrailingZeros(currentBits);

	return result>size?size:result;
}