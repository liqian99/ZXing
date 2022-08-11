#include "QRBitMatrixParser.h"
#include "../DiyMalloc.h"
#include "QRDataMask.h"


int QRBitMatrixParserCopyBit(Bitmatrix *pBM,int x,int y,int versionBits)
{
	return BitMatrixGet(pBM,x,y)?(versionBits<<1)|0x01:(versionBits<<1);
}

int QRBitMatrixParserInit(QRBitMatrixParser *pBarser,Bitmatrix *pBM)
{
	unsigned int dimension;

	pBarser->bitMatrix_ = pBM;
	pBarser->parsedVersion_ = NULL;
	pBarser->parsedFormatInfo_ = NULL;

	dimension = pBM->height;
	if((dimension < 21) || (dimension & 0x03) != 1){
		//Dimension must be 1 mod 4 and >= 21"
		return RT_ERR_QR_DIMENSION;
	}

	return 0;
}

void QRBitMatrixParserUninit(QRBitMatrixParser *pBarser)
{
	if(pBarser)
	{
		FREE(pBarser->parsedFormatInfo_);
	}
}

QRFormatInfo *QRBitMatrixParserReadFormatInfo(QRBitMatrixParser *pBarser)
{
	int formatInfoBits1 = 0,i,j,dimension,formatInfoBits2,jMin,ret = 0;
    Bitmatrix *pBitmatrix = NULL;

	if(pBarser->parsedFormatInfo_ != NULL)
		return pBarser->parsedFormatInfo_;

	// Read top-left format info bits 382822800
    formatInfoBits1 = 0;
	pBitmatrix = pBarser->bitMatrix_;
	for (i=0;i<6;i++)
	{
		formatInfoBits1 = QRBitMatrixParserCopyBit(pBitmatrix,i,8,formatInfoBits1);
	}
	// .. and skip a bit in the timing pattern ...
	formatInfoBits1 = QRBitMatrixParserCopyBit(pBitmatrix,7, 8, formatInfoBits1);
	formatInfoBits1 = QRBitMatrixParserCopyBit(pBitmatrix,8, 8, formatInfoBits1);
	formatInfoBits1 = QRBitMatrixParserCopyBit(pBitmatrix,8, 7, formatInfoBits1);
	// .. and skip a bit in the timing pattern ...
	for (j = 5; j >= 0; j--) {
		formatInfoBits1 = QRBitMatrixParserCopyBit(pBitmatrix,8, j, formatInfoBits1);
	}

	// Read the top-right/bottom-left pattern
	dimension = pBitmatrix->height;
	formatInfoBits2 = 0;
	jMin = dimension - 7;
	for (j = dimension - 1; j >= jMin; j--) {
		formatInfoBits2 = QRBitMatrixParserCopyBit(pBitmatrix,8, j, formatInfoBits2);
	}
	for (i = dimension - 8; i < dimension; i++) {
		formatInfoBits2 = QRBitMatrixParserCopyBit(pBitmatrix,i, 8, formatInfoBits2);
	}
	
	MALLOC(pBarser->parsedFormatInfo_,QRFormatInfo,1);
	ret = QRFormatInfoDecodeFormatInfo(pBarser->parsedFormatInfo_,formatInfoBits1,formatInfoBits2);
	if (ret == 0) {
        return pBarser->parsedFormatInfo_;
	}

ERROR_EXIT:
	return NULL;
}

QRVersion *QRBitMatrixParserReadVersion(QRVersion *pVersionGroup,QRBitMatrixParser *pBarser)
{
    int dimesion,provisonalVersion,y,x,versionBits,xMin,yMin;
	Bitmatrix *pBitmatrix = pBarser->bitMatrix_;

	if(pBarser->parsedVersion_ != NULL)
		return pBarser->parsedVersion_;

	dimesion = pBitmatrix->height;

	provisonalVersion = (dimesion-17) >> 2;
	if(provisonalVersion <= 6)
		return QRVersionGetVersionForNumber(pVersionGroup,provisonalVersion);

	// Read top-right version info: 3 wide by 6 tall
	versionBits = 0;
	for (y=5;y>=0;y--)
	{
		xMin = dimesion - 11;
		for (x = dimesion - 9; x >= xMin; x--)
		{
			versionBits = QRBitMatrixParserCopyBit(pBitmatrix,x,y,versionBits);
		}
	}

    pBarser->parsedVersion_ = QRVersionDecodeVersionInformation(pVersionGroup,versionBits);
	if(pBarser->parsedVersion_ != NULL && QRVersionGetDimensionForVersion(pBarser->parsedVersion_)==dimesion)
		return pBarser->parsedVersion_;

	// Hmm, failed. Try bottom left: 6 wide by 3 tall
	versionBits = 0;
	for (x = 5; x >= 0; x--) {
		yMin = dimesion - 11;
		for (y = dimesion - 9; y >= yMin; y--) {
			versionBits = QRBitMatrixParserCopyBit(pBitmatrix,x, y, versionBits);
		}
	}

	pBarser->parsedVersion_ = QRVersionDecodeVersionInformation(pVersionGroup,versionBits);
	if(pBarser->parsedVersion_ != NULL && QRVersionGetDimensionForVersion(pBarser->parsedVersion_)==dimesion)
		return pBarser->parsedVersion_;

	return NULL;
}

int QRBitMatrixParserReadCodewords(QRBitMatrixParser *pBarser,char *result,int *resultLen)
{
	QRVersion *version = NULL;
	int resultOffset = 0,totalCodewords,ret=0,reference,dimension;
    QRFormatInfo *formatInfo = NULL;
    Bitmatrix *funcionPattern = NULL;
    BOOL readingUp = TRUE;
	char *resultTemp = result;
	int nResultTempLen = 0,currentByte,bitsRead,x,counter,col,y;

	formatInfo = QRBitMatrixParserReadFormatInfo(pBarser);
	if(formatInfo == NULL) {ret = RT_ERR_QR_READFORMATINFO;goto ERROR_EXIT;}
	version = QRBitMatrixParserReadVersion(g_pVersionGroup,pBarser);
	if(version == NULL) {ret = RT_ERR_QR_READVERSION;goto ERROR_EXIT;}

	// Get the data mask for the format used in this QR Code. This will exclude
    // some bits from reading as we wind through the bit matrix
	reference = QRFormatInfoGetDataMask(formatInfo);
	dimension = pBarser->bitMatrix_->height;
	QRDataMaskUnmaskBitMatrix(reference,pBarser->bitMatrix_,dimension);

  	ret = QRVersionBuildFunctionPattern(&funcionPattern,version);

	if(ret == 0)
	{
		readingUp = TRUE;
        nResultTempLen = QRVersionGetTotalCodewords(version);
		if(nResultTempLen > 0)
		{
			currentByte = 0;
			bitsRead = 0;
			for (x=dimension-1;x>0;x-=2)
			{
				if (x == 6) {
					// Skip whole column with vertical alignment pattern;
					// saves time and makes the other code proceed more cleanly
					x--;
				}
                // Read alternatingly from bottom to top then top to bottom
				for (counter = 0;counter < dimension;counter++)
				{
					y = readingUp?dimension-1-counter:counter;
					for (col=0;col<2;col++)
					{
                        if(!BitMatrixGet(funcionPattern,x-col,y)){
                            bitsRead++;
							currentByte <<= 1;
							if(BitMatrixGet(pBarser->bitMatrix_,x-col,y)){
								currentByte |= 1;
							}
							if(bitsRead == 8)
							{
								resultTemp[resultOffset++] = (char)currentByte;
								bitsRead = 0;
								currentByte = 0;
							}
						}
					}
				}
				readingUp = !readingUp;
			}
			totalCodewords = QRVersionGetTotalCodewords(version);
		}else{
			ret = RT_ERR_QR_CODEWORDS;
		}
	}/*ret==0*/

ERROR_EXIT:
   *resultLen = resultOffset;
   QRVersionDestroyFunctionPattern(funcionPattern);
   return ret;
}