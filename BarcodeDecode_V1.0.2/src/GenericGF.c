#include "GenericGF.h"
#include "DiyMalloc.h"

const int INITIALIZATION_THRESHOLD = 0;

GenericGF *GenericGF_AZTEC_DATA_12 = NULL;
GenericGF *GenericGF_AZTEC_DATA_10 = NULL;
GenericGF *GenericGF_AZTEC_DATA_6 = NULL;
GenericGF *GenericGF_AZTEC_PARAM = NULL;
GenericGF *GenericGF_QR_CODE_FIELD_256 = NULL;
GenericGF *GenericGF_DATA_MATRIX_FIELD_256 = NULL;
GenericGF *GenericGF_AZTEC_DATA_8  = NULL;
GenericGF *GenericGF_MAXICODE_FIELD_64  = NULL;

void GenericGFInit(GenericGF *pGGF,int primitive,int size,int b)
{
	pGGF->size_ = size;
	pGGF->primitive_ = primitive;
	pGGF->generatorBase_ = b;
	pGGF->initialized_ = FALSE;

 	if(size <= INITIALIZATION_THRESHOLD)
	{
       GenericGFInitialize(pGGF);
	}
}

void GenericGFUninit(GenericGF *pGGF)
{
   if(pGGF)
   {
	   FREE(pGGF->expTable_);
	   pGGF->expTableSize_ = 0;
	   FREE(pGGF->logTable_);
	   pGGF->logTableSize_ = 0;
	   GenericGFPolyUninit(pGGF->zero_);
	   FREE(pGGF->zero_);
       GenericGFPolyUninit(pGGF->one_);
	   FREE(pGGF->one_);
   }
}

int GenericGFBuildAll()
{
	int ret = 0;

	MALLOC(GenericGF_AZTEC_DATA_12,GenericGF,1);
	MALLOC(GenericGF_AZTEC_DATA_10,GenericGF,1);
	MALLOC(GenericGF_AZTEC_DATA_6,GenericGF,1);
	MALLOC(GenericGF_AZTEC_PARAM,GenericGF,1);
	MALLOC(GenericGF_QR_CODE_FIELD_256,GenericGF,1);
	MALLOC(GenericGF_DATA_MATRIX_FIELD_256,GenericGF,1);


	GenericGFInit(GenericGF_AZTEC_DATA_12,0x1069, 4096, 1);
	GenericGFInit(GenericGF_AZTEC_DATA_10,0x409, 1024, 1);
	GenericGFInit(GenericGF_AZTEC_DATA_6,0x43, 64, 1);
	GenericGFInit(GenericGF_AZTEC_PARAM,0x13, 16, 1);
	GenericGFInit(GenericGF_QR_CODE_FIELD_256,0x011D, 256, 0);
	GenericGFInit(GenericGF_DATA_MATRIX_FIELD_256,0x012D, 256, 1);
	GenericGF_AZTEC_DATA_8 = GenericGF_DATA_MATRIX_FIELD_256;
	GenericGF_MAXICODE_FIELD_64 = GenericGF_AZTEC_DATA_6;

ERROR_EXIT:
	return ret;
}

void GenericGFDestroyAll()
{
    if(GenericGF_AZTEC_DATA_12)
	{
        GenericGFUninit(GenericGF_AZTEC_DATA_12);
		FREE(GenericGF_AZTEC_DATA_12);
	}
    if(GenericGF_AZTEC_DATA_10)
    {
		   GenericGFUninit(GenericGF_AZTEC_DATA_10);
		   FREE(GenericGF_AZTEC_DATA_10);
	}
    if(GenericGF_AZTEC_DATA_6)
    {
		GenericGFUninit(GenericGF_AZTEC_DATA_6);
		FREE(GenericGF_AZTEC_DATA_6);
	}
    if(GenericGF_AZTEC_PARAM)
    {
		GenericGFUninit(GenericGF_AZTEC_PARAM);
		FREE(GenericGF_AZTEC_PARAM);
	}
    if(GenericGF_QR_CODE_FIELD_256)
    {
		GenericGFUninit(GenericGF_QR_CODE_FIELD_256);
		FREE(GenericGF_QR_CODE_FIELD_256);
	}
    if(GenericGF_DATA_MATRIX_FIELD_256)
    {
		GenericGFUninit(GenericGF_DATA_MATRIX_FIELD_256);
		FREE(GenericGF_DATA_MATRIX_FIELD_256);
	}
	GenericGF_AZTEC_DATA_8 = GenericGF_DATA_MATRIX_FIELD_256;
	GenericGF_MAXICODE_FIELD_64 = GenericGF_AZTEC_DATA_6;
}

int GenericGFInitialize(GenericGF *pGGF) 
{
	int ret = 0,x,i;
	int nArray[1] = {0};

	pGGF->expTableSize_ = pGGF->logTableSize_ = pGGF->size_;
	REALLOC(pGGF->expTable_,int,pGGF->expTableSize_);
	REALLOC(pGGF->logTable_,int,pGGF->logTableSize_);
    
	x = 1;
    
	for (i = 0; i < pGGF->size_; i++) {
		pGGF->expTable_[i] = x;
		x <<= 1; // x = x * 2; we're assuming the generator alpha is 2
		if (x >= pGGF->size_) {
			x ^= pGGF->primitive_;
			x &= pGGF->size_-1;
		}
	}
	for (i = 0; i < pGGF->size_-1; i++) {
		pGGF->logTable_[pGGF->expTable_[i]] = i;
	}
	//logTable[0] == 0 but this should never be used
    
    if(pGGF->zero_ == NULL)
		MALLOC(pGGF->zero_,_GenericGFPoly,1);

    GenericGFPolyInit(pGGF->zero_,pGGF,nArray,1);
	pGGF->zero_->coefficients_[0] = 0;

	if(pGGF->one_ == NULL)
		MALLOC(pGGF->one_,_GenericGFPoly,1);
	GenericGFPolyInit(pGGF->one_,pGGF,nArray,1);
	pGGF->one_->coefficients_[0] = 1;


	pGGF->initialized_ = TRUE;

ERROR_EXIT:
	return ret;
}

void GenericGFCheckInit(GenericGF *pGGF)
{
	if (!pGGF->initialized_) {
		GenericGFInitialize(pGGF);
	}
}

_GenericGFPoly* GenericGFGetZero(GenericGF *pGGF)
{
	GenericGFCheckInit(pGGF);
	return pGGF->zero_;
}

_GenericGFPoly* GenericGFGetOne(GenericGF *pGGF) {
	GenericGFCheckInit(pGGF);
	return pGGF->one_;
}
_GenericGFPoly *GenericGFBuildMonomial(GenericGF *pGGF,int degree, int coefficient,_GenericGFPoly *pGGFP,BOOL *bUninit)
{
	int *coefficients = NULL;
    int ret = 0;

//  	GenericGFCheckInit(pGGF);
    
	if (degree < 0) {
		//throw IllegalArgumentException("Degree must be non-negative");
		*bUninit = FALSE;
		return NULL;
	}
	if (coefficient == 0) {
		*bUninit = FALSE;
        return pGGF->zero_;
	}
	MALLOC(coefficients,int,degree+1);
	coefficients[0] = coefficient;
    
    GenericGFPolyUninit(pGGFP);

    GenericGFPolyInit(pGGFP,pGGF,coefficients,degree+1);

ERROR_EXIT:
	FREE(coefficients);
	if(ret == 0)
	{
		*bUninit = TRUE;
		return pGGFP;
	}else{
        *bUninit = FALSE;
		return NULL;
	}
}

int GenericGFAddOrSubtract(int a, int b)
{
	return a ^ b;
}

int GenericGFExp(GenericGF *pGGF,int a)
{
	GenericGFCheckInit(pGGF);
	return pGGF->expTable_[a];
}

int GenericGFLog(GenericGF *pGGF,int a) 
{
	GenericGFCheckInit(pGGF);
	if (a == 0) {
		//throw IllegalArgumentException("cannot give log(0)");
		return 0;
	}
	return pGGF->logTable_[a];
}

int GenericGFInverse(GenericGF *pGGF,int a)
{
	GenericGFCheckInit(pGGF);
	if (a == 0) {
		//throw IllegalArgumentException("Cannot calculate the inverse of 0");
		return 0;
	}
	return pGGF->expTable_[pGGF->size_ - pGGF->logTable_[a] - 1];
}

int GenericGFMultiply(GenericGF *pGGF,int a, int b)
{
	GenericGFCheckInit(pGGF);
    
	if (a == 0 || b == 0) {
		return 0;
	}
    
	return pGGF->expTable_[(pGGF->logTable_[a] + pGGF->logTable_[b]) % (pGGF->size_ - 1)];
}

int GenericGFGetSize(GenericGF *pGGF)
{
	return pGGF->size_;
}

int GenericGFGetGeneratorBase(GenericGF *pGGF)
{
	return pGGF->generatorBase_;
}
