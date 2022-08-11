#ifndef __GENERICGF_H__
#define __GENERICGF_H__

#include "BaseTypeDef.h"

typedef struct GenericGFPoly _GenericGFPoly;

typedef struct _GenericGF{
	int *expTable_;
	int expTableSize_;
	int *logTable_;
	int logTableSize_;
   
	_GenericGFPoly *zero_;
	_GenericGFPoly *one_;

    int size_;
	int primitive_;
	int generatorBase_;
	BOOL initialized_;
}GenericGF;


struct GenericGFPoly{
	int *coefficients_;
	int coefficientsSize_;
	GenericGF *field_;
};

// extern GenericGFPoly *g_zero;
// extern GenericGFPoly *g_one;

extern GenericGF *GenericGF_AZTEC_DATA_12;
extern GenericGF *GenericGF_AZTEC_DATA_10;
extern GenericGF *GenericGF_AZTEC_DATA_6;
extern GenericGF *GenericGF_AZTEC_PARAM;
extern GenericGF *GenericGF_QR_CODE_FIELD_256;
extern GenericGF *GenericGF_DATA_MATRIX_FIELD_256;
extern GenericGF *GenericGF_AZTEC_DATA_8;
extern GenericGF *GenericGF_MAXICODE_FIELD_64;

//GenericGF
void GenericGFInit(GenericGF *pGGF,int primitive,int size,int b);
void GenericGFUninit(GenericGF *pGGF);
int GenericGFBuildAll(void);
void GenericGFDestroyAll(void);
int GenericGFInitialize(GenericGF *pGGF); 
void GenericGFCheckInit(GenericGF *pGGF);
_GenericGFPoly* GenericGFGetZero(GenericGF *pGGF);
_GenericGFPoly* GenericGFGetOne(GenericGF *pGGF);
_GenericGFPoly *GenericGFBuildMonomial(GenericGF *pGGF,int degree, int coefficient,_GenericGFPoly *pGGFP,BOOL *bUninit);
int GenericGFAddOrSubtract(int a, int b);
int GenericGFExp(GenericGF *pGGF,int a);
int GenericGFLog(GenericGF *pGGF,int a); 
int GenericGFInverse(GenericGF *pGGF,int a);
int GenericGFMultiply(GenericGF *pGGF,int a, int b);
int GenericGFGetSize(GenericGF *pGGF);
int GenericGFGetGeneratorBase(GenericGF *pGGF);

//GenericGfPoly
int GenericGFPolyInit(_GenericGFPoly *pGGFP,GenericGF *field,
                      int *coefficients,int coefficientsSize);
void GenericGFPolyUninit(_GenericGFPoly *pGGFP);
int *GenericGFPolyGetCoefficients(_GenericGFPoly *pGGFP);
int GenericGFPolyGetDegree(_GenericGFPoly *pGGFP); 
BOOL GenericGFPolyIsZero(_GenericGFPoly *pGGFP);
int GenericGFPolyGetCoefficient(_GenericGFPoly *pGGFP,int degree);
int GenericGFPolyEvaluateAt(_GenericGFPoly *pGGFP,int a);
_GenericGFPoly * GenericGFPolyAddOrSubtract(_GenericGFPoly *pGGFP,_GenericGFPoly *other,_GenericGFPoly *pOut,BOOL *bUninit); 
_GenericGFPoly *GenericGFPolyMultiply(_GenericGFPoly *pGGFP,_GenericGFPoly *other,_GenericGFPoly *pOut,BOOL *bUninit);
_GenericGFPoly *GenericGFPolyMultiplyByScale(_GenericGFPoly *pGGFP,int scalar,_GenericGFPoly *pOut,BOOL *bUninit);
_GenericGFPoly *GenericGFPolyMultiplyByMonomial(_GenericGFPoly *pGGFP,int degree, int coefficient,_GenericGFPoly *pOut,BOOL *bUninit);
// int GenericGFPolyDivide(_GenericGFPoly *pGGFP,_GenericGFPoly *other,_GenericGFPoly **ppOut,int *pOutSize);

#endif