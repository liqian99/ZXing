#include "UPCAReader.h"
int UPCAReaderMaybeReturnResult(char* result,int *resultLen)
{
	int len = *resultLen,i=0;
	if (result[0] == '0') 
	{
		for (i=0;i<len-1;i++)
		{
			result[i] = result[i+1];
		}
		result[len-1] = 0;
		len--;
		*resultLen = len;
        
		return len;
	}
	return RT_ERR_DECODEUPCA;
}

// int UPCAReaderDecodeRow(int rowNumber, BitArray *row,char *pResult,int *pResultLen) 
// {
// 	int ret = EAN13ReaderDecodeRow(rowNumber, row,pResult,pResultLen);
// 	if(ret >= 0)
// 	  ret = UPCAReaderMaybeReturnResult(pResult,pResultLen);
// 
// 	return ret;
// }

// Ref<Result> UPCAReader::decodeRow(int rowNumber,
//                                   Ref<BitArray> row,
//                                   Range const& startGuardRange) {
// 	return maybeReturnResult(ean13Reader.decodeRow(rowNumber, row, startGuardRange));
// }

// Ref<Result> UPCAReader::decode(Ref<BinaryBitmap> image, DecodeHints hints) {
// 	return maybeReturnResult(ean13Reader.decode(image, hints));
// }

// int UPCAReaderDecodeMiddle(BitArray *row,
//                            OneDRange *startRange,
//                            char *result,int *resultLen)
// {
// 	return EAN13ReaderDecodeMiddle(row, startRange, result,resultLen);
// }

