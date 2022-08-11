#ifndef __DECODEHINTS_H__
#define __DECODEHINTS_H__

#include "BaseTypeDef.h"
#include "BarcodeFormat.h"

typedef unsigned int DecodeHintType; 

extern const DecodeHintType AZTEC_HINT;
extern const DecodeHintType CODABAR_HINT;
extern const DecodeHintType CODE_39_HINT;
extern const DecodeHintType CODE_93_HINT;
extern const DecodeHintType CODE_128_HINT;
extern const DecodeHintType DATA_MATRIX_HINT;
extern const DecodeHintType EAN_8_HINT;
extern const DecodeHintType EAN_13_HINT;
extern const DecodeHintType ITF_HINT;
extern const DecodeHintType MAXICODE_HINT;
extern const DecodeHintType PDF_417_HINT;
extern const DecodeHintType QR_CODE_HINT;
extern const DecodeHintType RSS_14_HINT;
extern const DecodeHintType RSS_EXPANDED_HINT;
extern const DecodeHintType UPC_A_HINT;
extern const DecodeHintType UPC_E_HINT;
extern const DecodeHintType UPC_EAN_EXTENSION_HINT;

extern const DecodeHintType TRYHARDER_HINT;
extern const DecodeHintType CHARACTER_SET;
extern const DecodeHintType  ASSUME_GS1;

// const DecodeHints PRODUCT_HINT;
// const DecodeHints ONED_HINT;
// const DecodeHints DEFAULT_HINT;

typedef struct _DecodeHints{
    DecodeHintType hints_;
}DecodeHints;

BOOL DecodeHintsGetTryHarder(DecodeHints *pHints);


#endif