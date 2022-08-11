#include "QRDecode.h"
#include "../DiyMalloc.h"

int QRDecoderCorrectErrors(char *codewordBytes,int codewordBytesSize,int numDataCodewords)
{
	int numCodewords = codewordBytesSize,ret=0,i,numECCodewords;
	int *codewordInts = NULL;
    ReedSolomonDecoder reedSolomonDecoder;

	diy_memset(&reedSolomonDecoder,0,sizeof(ReedSolomonDecoder));

    MALLOC(codewordInts,int,numCodewords);

	for (i = 0; i < numCodewords; i++)
	{
		codewordInts[i] = codewordBytes[i] & 0xff;
	}
	numECCodewords = numCodewords - numDataCodewords;
	
    ReedSolomonDecoderInit(&reedSolomonDecoder,GenericGF_QR_CODE_FIELD_256);

 	ret = ReedSolomonDecoderDecode(&reedSolomonDecoder,codewordInts,numCodewords,numECCodewords);
	
	for (i = 0; i < numDataCodewords; i++) {
		codewordBytes[i] = (char)codewordInts[i];
	}

ERROR_EXIT:
   FREE(codewordInts);
   ReedSolomonDecoderUninit();
   return ret;
}

int QRDecode(Bitmatrix *bits,unsigned char *pResult,int *pResultLen)
{
		QRBitMatrixParser parser;
		QRErrorCorrectLevel *ecLevel = NULL;
		QRVersion *version = NULL;
        int ret = 0,totalBytes,i,j;
		QRFormatInfo *pFormatInfo = NULL;
		char *codewords = NULL,*resultBytes = NULL,*codewordBytes=NULL;
		int codewordsLen = 0;
		int codewordBytesSize = 0;
		QRDataBlock *pDataBlockGroup = NULL;
		int nDataBlockGroupSize = 0,resultOffset,numDataCodewords;

		diy_memset(&parser,0,sizeof(QRBitMatrixParser));
		//diy_memset(ecLevel,0,sizeof(QRErrorCorrectLevel));

		ret = QRBitMatrixParserInit(&parser,bits);
		if(ret != 0) goto ERROR_EXIT;

		version = QRBitMatrixParserReadVersion(g_pVersionGroup,&parser);
		if(version == NULL) {ret = RT_ERR_QR_VERSION; goto ERROR_EXIT;}


		pFormatInfo = QRBitMatrixParserReadFormatInfo(&parser);
		if(pFormatInfo==NULL) {ret = RT_ERR_QR_READFORMATINFO;goto ERROR_EXIT;}

        ecLevel = QRFormatInfoGetErrorCorrectionLevel(pFormatInfo);
		
		// Read codewords
		MALLOC(codewords,char,10240);
 		ret = QRBitMatrixParserReadCodewords(&parser,codewords,&codewordsLen);
		if(ret != 0) {goto ERROR_EXIT;}
#ifdef _DEBUG
		j =  codewordsLen;
		for (i=0;i<j;i++)
		{
			printf("i=%d  %d\n",i,codewords[i]);
		}
#endif		
		// Separate into data blocks
		nDataBlockGroupSize = QRDataBlockGetDataBlocks(&pDataBlockGroup,codewords,codewordsLen,version,ecLevel);
		if(nDataBlockGroupSize <= 0) {ret=RT_ERR_QR_DATABLOCK; goto ERROR_EXIT;}

		// Count total number of data bytes
		totalBytes = 0;
		for (i = 0; i < nDataBlockGroupSize; i++) {
			totalBytes += pDataBlockGroup[i].numDataCodewords_;
		}
		if(totalBytes <= 0){
		    goto ERROR_EXIT;
		}
		MALLOC(resultBytes,char,totalBytes);
		resultOffset = 0;
 		
		// Error-correct and copy data blocks together into a stream of bytes
		for (j = 0; j < nDataBlockGroupSize; j++) {
			codewordBytes =pDataBlockGroup[j].codewords_;
			codewordBytesSize = pDataBlockGroup[j].codewordsSize_;
			numDataCodewords = pDataBlockGroup[j].numDataCodewords_;
            ret = QRDecoderCorrectErrors(codewordBytes,codewordBytesSize,numDataCodewords);
			if(ret !=0) {goto ERROR_EXIT;}
			for (i = 0; i < numDataCodewords; i++) {
				resultBytes[resultOffset++] = codewordBytes[i];
			}
		}

        ret = DecodedBitStreamParserDecode(resultBytes,totalBytes,version,ecLevel,NULL,pResult,pResultLen);

ERROR_EXIT:
	QRBitMatrixParserUninit(&parser);
	QRDataBlockDelDataBlocks(pDataBlockGroup,nDataBlockGroupSize);

	FREE(codewords);
	FREE(resultBytes);
	return ret;
}

