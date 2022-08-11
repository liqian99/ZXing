#ifndef __BITSOURCE_H__
#define __BITSOURCE_H__

typedef struct _BitSource
{
	int byteSize_;
    char *byte_;
	int byteOffset_;
	int bitOffset_;
}BitSource;

void BitSourceInit(BitSource *pBitSource,char *bytes,int bytesSize);
int BitSourceReadBits(BitSource *pBitSource,int numBits);
int BitSourceAvailable(BitSource *pBitSource);

#endif