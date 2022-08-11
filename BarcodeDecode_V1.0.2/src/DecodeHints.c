#include "DecodeHints.h"

const DecodeHintType AZTEC_HINT = 1 << AZTEC;
const DecodeHintType CODABAR_HINT = 1 << CODABAR;
const DecodeHintType CODE_39_HINT = 1 << CODE_39;
const DecodeHintType CODE_93_HINT = 1 << CODE_93;
const DecodeHintType CODE_128_HINT = 1 << CODE_128;
const DecodeHintType DATA_MATRIX_HINT = 1 << DATA_MATRIX;
const DecodeHintType EAN_8_HINT = 1 << EAN_8;
const DecodeHintType EAN_13_HINT = 1 << EAN_13;
const DecodeHintType ITF_HINT = 1 << ITF;
const DecodeHintType MAXICODE_HINT = 1 << MAXICODE;
const DecodeHintType PDF_417_HINT = 1 << PDF_417;
const DecodeHintType QR_CODE_HINT = 1 << QR_CODE;
const DecodeHintType RSS_14_HINT = 1 << RSS_14;
const DecodeHintType RSS_EXPANDED_HINT = 1 << RSS_EXPANDED;
const DecodeHintType UPC_A_HINT = 1 << UPC_A;
const DecodeHintType UPC_E_HINT = 1 << UPC_E;
const DecodeHintType UPC_EAN_EXTENSION_HINT = 1 << UPC_EAN_EXTENSION;

const DecodeHintType TRYHARDER_HINT = 1 << 31;
const DecodeHintType CHARACTER_SET = 1 << 30;
const DecodeHintType  ASSUME_GS1 = 1 << 27;

DecodeHintType DecodeHints_PRODUCT_HINT()
{
	return (UPC_A_HINT |
			UPC_E_HINT |
			EAN_13_HINT |
			EAN_8_HINT |
			RSS_14_HINT);
}

DecodeHintType DecodeHints_ONED_HINT()
{
	return   (CODE_39_HINT |
			 CODE_93_HINT |
			 CODE_128_HINT |
			 ITF_HINT |
			 CODABAR_HINT |
			 DecodeHints_PRODUCT_HINT());
}

DecodeHintType DecodeHints_DEFAULT_HINT()
{
	return (DecodeHints_ONED_HINT() |
		    QR_CODE_HINT |
            PDF_417_HINT);
}

void DecodeHintsInit(DecodeHints *pHints)
{
    pHints->hints_ = 0;
}

void DecodeHintsAddFormat(DecodeHints *pHints,BarcodeFormatValue toAdd)
{
	switch (toAdd)
	{
		case AZTEC: pHints->hints_ |= AZTEC_HINT; break;
		case CODABAR: pHints->hints_ |= CODABAR_HINT; break;
		case CODE_39: pHints->hints_ |= CODE_39_HINT; break;
		case CODE_93: pHints->hints_ |= CODE_93_HINT; break;
		case CODE_128: pHints->hints_ |= CODE_128_HINT; break;
		case DATA_MATRIX: pHints->hints_ |= DATA_MATRIX_HINT; break;
		case EAN_8: pHints->hints_ |= EAN_8_HINT; break;
		case EAN_13: pHints->hints_ |= EAN_13_HINT; break;
		case ITF: pHints->hints_ |= ITF_HINT; break;
		case MAXICODE: pHints->hints_ |= MAXICODE_HINT; break;
		case PDF_417: pHints->hints_ |= PDF_417_HINT; break;
		case QR_CODE: pHints->hints_ |= QR_CODE_HINT; break;
		case RSS_14: pHints->hints_ |= RSS_14_HINT; break;
		case RSS_EXPANDED: pHints->hints_ |= RSS_EXPANDED_HINT; break;
		case UPC_A: pHints->hints_ |= UPC_A_HINT; break;
		case UPC_E: pHints->hints_ |= UPC_E_HINT; break;
		case UPC_EAN_EXTENSION: pHints->hints_ |= UPC_EAN_EXTENSION_HINT; break;
		default: break;
  }
}

BOOL DecodeHintsContainsFormat(DecodeHints *pHints,BarcodeFormatValue toCheck)
{
	DecodeHintType checkAgainst = 0;
	switch (toCheck) {
		case AZTEC: checkAgainst |= AZTEC_HINT; break;
		case CODABAR: checkAgainst |= CODABAR_HINT; break;
		case CODE_39: checkAgainst |= CODE_39_HINT; break;
		case CODE_93: checkAgainst |= CODE_93_HINT; break;
		case CODE_128: checkAgainst |= CODE_128_HINT; break;
		case DATA_MATRIX: checkAgainst |= DATA_MATRIX_HINT; break;
		case EAN_8: checkAgainst |= EAN_8_HINT; break;
		case EAN_13: checkAgainst |= EAN_13_HINT; break;
		case ITF: checkAgainst |= ITF_HINT; break;
		case MAXICODE: checkAgainst |= MAXICODE_HINT; break;
		case PDF_417: checkAgainst |= PDF_417_HINT; break;
		case QR_CODE: checkAgainst |= QR_CODE_HINT; break;
		case RSS_14: checkAgainst |= RSS_14_HINT; break;
		case RSS_EXPANDED: checkAgainst |= RSS_EXPANDED_HINT; break;
		case UPC_A: checkAgainst |= UPC_A_HINT; break;
		case UPC_E: checkAgainst |= UPC_E_HINT; break;
		case UPC_EAN_EXTENSION: checkAgainst |= UPC_EAN_EXTENSION_HINT; break;
		default: 
			break;
	}
    return (pHints->hints_ & checkAgainst) != 0;
}

void DecodeHintsSetTryHarder(DecodeHints *pHints,BOOL toset)
{
	if(toset)
		pHints->hints_ |= TRYHARDER_HINT;
	else
		pHints->hints_ &= ~TRYHARDER_HINT;
}

BOOL DecodeHintsGetTryHarder(DecodeHints *pHints)
{
     return (pHints->hints_ & TRYHARDER_HINT) != 0;
}