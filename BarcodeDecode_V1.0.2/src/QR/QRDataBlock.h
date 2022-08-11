#ifndef __QR_DATABLOCK_H__
#define __QR_DATABLOCK_H__

#include "QRVersion.h"

typedef struct _QRDataBlock
{
    int numDataCodewords_;
	int codewordsSize_;
	char *codewords_;
}QRDataBlock;

int QRDataBlockGetDataBlocks(QRDataBlock **ppDataBlockGroup,char *rawCodewords,int rawCodewordsSize,QRVersion *version,QRErrorCorrectLevel *ecLevel);
void QRDataBlockDelDataBlocks(QRDataBlock *pDataBlockGroup,int DataBlockGroupSize);

#endif