#include "QRDataMask.h"
#include "QRDataMask.h"
// int QRDataMaskForReference(int reference)
// {
//     return reference;
// }

/**
* 000: mask bits for which (x + y) mod 2 == 0
*/
BOOL DataMask000IsMasked(unsigned int x, unsigned int y)
{
	return ((x + y) % 2) == 0;
}

BOOL DataMask001IsMasked(unsigned int x, unsigned int y)
{
	return (x % 2) == 0;
}

BOOL DataMask010IsMasked(unsigned int x, unsigned int y)
{
	return y % 3 == 0;
}

BOOL DataMask011IsMasked(unsigned int x, unsigned int y)
{
	return (x + y) % 3 == 0;
}

BOOL DataMask100IsMasked(unsigned int x, unsigned int y)
{
	return (((x >> 1) + (y / 3)) % 2) == 0;
}

BOOL DataMask101IsMasked(unsigned int x, unsigned int y)
{
	unsigned int temp = x * y;
    return (temp % 2) + (temp % 3) == 0;
}

BOOL DataMask110IsMasked(unsigned int x, unsigned int y)
{
    unsigned int temp = x * y;
    return (((temp % 2) + (temp % 3)) % 2) == 0;
}

BOOL DataMask111IsMasked(unsigned int x, unsigned int y)
{
    return ((((x + y) % 2) + ((x * y) % 3)) % 2) == 0;
}

void QRDataMaskUnmaskBitMatrix(int reference,Bitmatrix *bits,unsigned int dimension)
{
	unsigned int x,y;
	typedef BOOL (*pIsMask)(unsigned int m,unsigned int n);
    pIsMask IsMask = NULL;

	switch(reference)
	{
	case 0:
		IsMask = DataMask000IsMasked;
		break;
	case 1:
		IsMask = DataMask001IsMasked;
		break;
	case 2:
		IsMask = DataMask010IsMasked;
		break;
	case 3:
		IsMask = DataMask011IsMasked;
		break;
	case 4:
		IsMask = DataMask100IsMasked;
		break;
	case 5:
		IsMask = DataMask101IsMasked;
		break;
	case 6:
		IsMask = DataMask110IsMasked;
		break;
	case 7:
		IsMask = DataMask111IsMasked;
		break;
	default:
		break;
	}

	if(IsMask)
	{
		for (y = 0; y < dimension; y++) {
			for (x = 0; x < dimension; x++) {
				// TODO: check why the coordinates have to be swapped
				if (IsMask(y, x)) {
					BitMatrixFlip(bits,x,y);
				}
			}
		}
	}
}