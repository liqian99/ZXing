#include "QRDataBlock.h"
#include "../DiyMalloc.h"

int QRDataBlockInit(QRDataBlock *pDataBlock,int numDataCodewords,int codewordsSize)
{
	int ret = 0;

	pDataBlock->numDataCodewords_ = numDataCodewords;
	pDataBlock->codewordsSize_ = codewordsSize;

	MALLOC(pDataBlock->codewords_,char,pDataBlock->codewordsSize_);

ERROR_EXIT:
	return ret;
}

void QRDataBlockUninit(QRDataBlock *pDataBlock)
{
    if(pDataBlock)
	{
		FREE(pDataBlock->codewords_);
	}
	pDataBlock->codewordsSize_ = 0;
}

int QRDataBlockGetDataBlocks(QRDataBlock **ppDataBlockGroup,char *rawCodewords,int rawCodewordsSize,QRVersion *version,QRErrorCorrectLevel *ecLevel)
{
	int totalBlocks = 0,ret = 0,shorterBlocksTotalCodewords,longerBlocksStartAt,numCodewords,shorterBlocksNumDataCodewords,rawCodewordsOffset;
	int i = 0,j,nmax,iOffset;
	QRECB *ecBlockArray = NULL,*ecBlock = NULL;
	int ecBlockArraySize = 0,numResultBlocks,numDataCodewords,numBlockCodewords;
    QRDataBlock *result = NULL;

	// Figure out the number and size of data blocks used by this version and
    // error correction level	
    QRECBlocks ecBlocks;
	
	diy_memset(&ecBlocks,0,sizeof(QRECBlocks));

	ecBlocks = QRVersionGetECBlocksForLevel(version,ecLevel);

	// First count the total number of data blocks
	MALLOC(ecBlockArray,QRECB,2);
	QRECBlocksGetECBlocks(ecBlockArray,&ecBlockArraySize,&ecBlocks);
	for (i=0;i<ecBlockArraySize;i++)
	{
		totalBlocks += ecBlockArray[i].count_;
	}

	// Now establish DataBlocks of the appropriate size and number of data codewords
    MALLOC(*ppDataBlockGroup,QRDataBlock,totalBlocks);
	result = *ppDataBlockGroup;
	numResultBlocks = 0;
	for (j=0;j<ecBlockArraySize;j++)
	{
        ecBlock = &ecBlockArray[j];
		for (i=0;i<ecBlock->count_;i++)
		{
            numDataCodewords = ecBlock->dataCodewords_;
			numBlockCodewords = ecBlocks.ecCodewords_ + numDataCodewords;
            QRDataBlockInit(&result[numResultBlocks],numDataCodewords,numBlockCodewords);
			numResultBlocks++;
		}
	}

	// All blocks have the same amount of data, except that the last n
	// (where n may be 0) have 1 more byte. Figure out where these start.
	shorterBlocksTotalCodewords = result[0].codewordsSize_;
	longerBlocksStartAt = totalBlocks - 1;
	while (longerBlocksStartAt >= 0) {
		numCodewords = result[longerBlocksStartAt].codewordsSize_;
		if (numCodewords == shorterBlocksTotalCodewords) {
			break;
		}
		if (numCodewords != shorterBlocksTotalCodewords + 1) {
			//throw IllegalArgumentException("Data block sizes differ by more than 1");
            ret = RT_ERR_QR_DBMORETHANONE;
			goto ERROR_EXIT;
		}
		longerBlocksStartAt--;
	}
    longerBlocksStartAt++;

	shorterBlocksNumDataCodewords = shorterBlocksTotalCodewords - QRECBlockGetECCodewords(&ecBlocks);
	// The last elements of result may be 1 element longer;
	// first fill out as many elements as all of them have
	rawCodewordsOffset = 0;
	for (i = 0; i < shorterBlocksNumDataCodewords; i++) {
		for (j = 0; j < numResultBlocks; j++) {
			result[j].codewords_[i] = rawCodewords[rawCodewordsOffset++];
		}
	}

	// Fill out the last data block in the longer ones
	for (j = longerBlocksStartAt; j < numResultBlocks; j++) {
		result[j].codewords_[shorterBlocksNumDataCodewords] = rawCodewords[rawCodewordsOffset++];
	}
	// Now add in error correction blocks
	nmax = result[0].codewordsSize_;
	for (i = shorterBlocksNumDataCodewords; i < nmax; i++) {
		for (j = 0; j < numResultBlocks; j++) {
			iOffset = j < longerBlocksStartAt ? i : i + 1;
			result[j].codewords_[iOffset] = rawCodewords[rawCodewordsOffset++];
		}
	}
	
	if (rawCodewordsOffset != rawCodewordsSize) {
		//throw IllegalArgumentException("rawCodewordsOffset != rawCodewords.length");
		ret = RT_ERR_QR_CODESIZE;
		goto ERROR_EXIT;
	}

	ret = totalBlocks;

ERROR_EXIT:
	FREE(ecBlockArray);
	return ret;
}

void QRDataBlockDelDataBlocks(QRDataBlock *pDataBlockGroup,int DataBlockGroupSize)
{
	int i;

	for(i=0;i<DataBlockGroupSize;i++)
	{
        QRDataBlockUninit(&pDataBlockGroup[i]);
	}
          
	FREE(pDataBlockGroup);
}