#include "BaseTypeDef.h"
#include "BitSource.h"

void BitSourceInit(BitSource *pBitSource,char *bytes,int bytesSize)
{
    pBitSource->byte_ = bytes;
	pBitSource->byteSize_ = bytesSize;
	pBitSource->byteOffset_ = 0;
	pBitSource->bitOffset_ = 0;
}

int BitSourceAvailable(BitSource *pBitSource)
{
	return 8 * (pBitSource->byteSize_ - pBitSource->byteOffset_) - pBitSource->bitOffset_;
}

int BitSourceReadBits(BitSource *pBitSource,int numBits)
{
	int result,bitsLeft,toRead,bitsToNotRead,mask;

	int nAvailable = BitSourceAvailable(pBitSource);

    if(numBits < 0 || numBits > 32 || numBits > nAvailable)
		return RT_ERR_READBITS;

    result = 0;
	
	// First, read remainder from current byte
	if (pBitSource->bitOffset_ > 0) {
		bitsLeft = 8 - pBitSource->bitOffset_;
		toRead = numBits < bitsLeft ? numBits : bitsLeft;
		bitsToNotRead = bitsLeft - toRead;
		mask = (0xFF >> (8 - toRead)) << bitsToNotRead;
		result = (pBitSource->byte_[pBitSource->byteOffset_] & mask) >> bitsToNotRead;
		numBits -= toRead;
		pBitSource->bitOffset_ += toRead;
		if (pBitSource->bitOffset_ == 8) {
			pBitSource->bitOffset_ = 0;
			pBitSource->byteOffset_++;
		}
	}

	// Next read whole bytes
	if (numBits > 0) {
		while (numBits >= 8) {
			result = (result << 8) | (pBitSource->byte_[pBitSource->byteOffset_] & 0xFF);
			pBitSource->byteOffset_++;
			numBits -= 8;
		}
		
		
		// Finally read a partial byte
		if (numBits > 0) {
			int bitsToNotRead = 8 - numBits;
			int mask = (0xFF >> bitsToNotRead) << bitsToNotRead;
			result = (result << numBits) | ((pBitSource->byte_[pBitSource->byteOffset_] & mask) >> bitsToNotRead);
			pBitSource->bitOffset_ += numBits;
		}
	}
	
  return result;
}

