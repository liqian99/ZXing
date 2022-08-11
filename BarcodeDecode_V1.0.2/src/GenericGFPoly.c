#include "GenericGFPoly.h"
#include "DiyMalloc.h"

int GenericGFPolyInit(_GenericGFPoly *pGGFP,GenericGF *field,
                      int *coefficients,int coefficientsSize)
{
	int coefficientsLength,firstNonZero,ret=0,i;
    _GenericGFPoly *pZero = NULL;

    pGGFP->field_ = field;
	if (coefficientsSize == 0) {
// 		throw IllegalArgumentException("need coefficients");
		return RT_ERR_QR_NEEDCOEFFICIENTS;
	}

	coefficientsLength = coefficientsSize;
	if (coefficientsLength > 1 && coefficients[0] == 0) {
		// Leading term must be non-zero for anything except the constant polynomial "0"
		firstNonZero = 1;
		while (firstNonZero < coefficientsLength && coefficients[firstNonZero] == 0) {
			firstNonZero++;
		}
		if (firstNonZero == coefficientsLength) {
			MALLOC(pGGFP->coefficients_,int,coefficientsSize);
			pZero = GenericGFGetZero(field);
			diy_memcpy(pGGFP->coefficients_,pZero->coefficients_,pZero->coefficientsSize_*sizeof(int));
			pGGFP->coefficientsSize_ = pZero->coefficientsSize_;
		} else {
			pGGFP->coefficientsSize_ = coefficientsLength-firstNonZero;
			MALLOC(pGGFP->coefficients_,int,pGGFP->coefficientsSize_);
			for (i = 0; i < (int)pGGFP->coefficientsSize_; i++) {
				pGGFP->coefficients_[i] = coefficients[i + firstNonZero];
			}
		}
	} else {
		MALLOC(pGGFP->coefficients_,int,coefficientsSize);
		diy_memcpy(pGGFP->coefficients_,coefficients,coefficientsSize*sizeof(int));
		pGGFP->coefficientsSize_ = coefficientsSize;
	}

ERROR_EXIT:
	return ret;
}

void GenericGFPolyUninit(_GenericGFPoly *pGGFP)
{
	if(pGGFP)
	{
		FREE(pGGFP->coefficients_);
		pGGFP->coefficientsSize_ = 0;
		//GenericGFUninit(pGGFP->field_);
		//pGGFP->field_ = NULL;
	}
}

int *GenericGFPolyGetCoefficients(_GenericGFPoly *pGGFP)
{
  return pGGFP->coefficients_;
}
  
int GenericGFPolyGetDegree(_GenericGFPoly *pGGFP) 
{
  return pGGFP->coefficientsSize_ - 1;
}
  
BOOL GenericGFPolyIsZero(_GenericGFPoly *pGGFP)
{
  return pGGFP->coefficients_[0] == 0;
}
  
int GenericGFPolyGetCoefficient(_GenericGFPoly *pGGFP,int degree)
{
  return pGGFP->coefficients_[pGGFP->coefficientsSize_ - 1 - degree];
}
  
int GenericGFPolyEvaluateAt(_GenericGFPoly *pGGFP,int a)
{
  int size,result,i;

  if (a == 0) {
    // Just return the x^0 coefficient
    return GenericGFPolyGetCoefficient(pGGFP,0);
  }
    
  size = pGGFP->coefficientsSize_;//coefficients_->size();
  if (a == 1) {
    // Just the sum of the coefficients
    result = 0;
    for (i = 0; i < size; i++) {
      result = GenericGFAddOrSubtract(result, pGGFP->coefficients_[i]);
    }
    return result;
  }
  result = pGGFP->coefficients_[0];
  for (i = 1; i < size; i++) {
    result = GenericGFAddOrSubtract(GenericGFMultiply(pGGFP->field_,a, result), pGGFP->coefficients_[i]);
  }
  return result;
}
  
_GenericGFPoly * GenericGFPolyAddOrSubtract(_GenericGFPoly *pGGFP,_GenericGFPoly *other,_GenericGFPoly *pOut,BOOL *bUninit) 
{
//   if (!(pGGFP->field_.o_ == other->field_.object_)) {
//     throw IllegalArgumentException("GenericGFPolys do not have same GenericGF field");
//   }
  int* smallerCoefficients = NULL,*temp = NULL;
  int smallerCoefficientsSize;
  int* largerCoefficients = NULL,*sumDiff = NULL;
  int largerCoefficientsSize,lengthDiff,i,ret = 0,ntmp;

  if(!(pGGFP->field_ == other->field_))
  {
      *bUninit = FALSE;
	  return NULL;
  }

  if(GenericGFPolyIsZero(pGGFP))
  {
	 *bUninit = FALSE;
     return other;
// 	 ret = GenericGFPolyInit(pOut,other->field_,
// 		 other->coefficients_,other->coefficientsSize_);
// 	 return pOut;

  }
  if (GenericGFPolyIsZero(other))
  {
	 *bUninit = FALSE;
     return pGGFP;
// 	 ret = GenericGFPolyInit(pOut,pGGFP->field_,
// 		 pGGFP->coefficients_,pGGFP->coefficientsSize_);
// 	 return pOut;
  }
    
  smallerCoefficients = pGGFP->coefficients_;
  smallerCoefficientsSize = pGGFP->coefficientsSize_;
  largerCoefficients = other->coefficients_;
  largerCoefficientsSize = other->coefficientsSize_;

  if (smallerCoefficientsSize > largerCoefficientsSize) {
    temp = smallerCoefficients;
    smallerCoefficients = largerCoefficients;
    largerCoefficients = temp;

	ntmp = smallerCoefficientsSize;
	smallerCoefficientsSize = largerCoefficientsSize;
	largerCoefficientsSize = ntmp;
  }
    
  MALLOC(sumDiff,int,largerCoefficientsSize);
  lengthDiff = largerCoefficientsSize - smallerCoefficientsSize;
  // Copy high-order terms only found in higher-degree polynomial's coefficients
  for (i = 0; i < lengthDiff; i++) {
    sumDiff[i] = largerCoefficients[i];
  }
    
  for (i = lengthDiff; i < (int)largerCoefficientsSize; i++) {
    sumDiff[i] = GenericGFAddOrSubtract(smallerCoefficients[i-lengthDiff],
                                        largerCoefficients[i]);
  }
    
  GenericGFPolyUninit(pOut);

  ret = GenericGFPolyInit(pOut,pGGFP->field_,
	                      sumDiff,largerCoefficientsSize);

ERROR_EXIT:
  FREE(sumDiff);
  if(ret == 0){
	  *bUninit = TRUE;
      return pOut;
  }
  else{
	  *bUninit = FALSE;
	  return NULL;
  }
}
  
_GenericGFPoly *GenericGFPolyMultiply(_GenericGFPoly *pGGFP,_GenericGFPoly *other,_GenericGFPoly *pOut,BOOL *bUninit)
{
//   if (!(field_.object_ == other->field_.object_)) {
//     throw IllegalArgumentException("GenericGFPolys do not have same GenericGF field");
//   }
  int ret = 0,aLength,bLength,aCoeff,i,j;  
  int *aCoefficients,*bCoefficients,*product;
  
  if(!(pGGFP->field_ == other->field_))
  {
      *bUninit = FALSE;
	  return NULL;
  }

  if (GenericGFPolyIsZero(pGGFP) || GenericGFPolyIsZero(other))
  {
	 *bUninit = FALSE;
     return GenericGFGetZero(pGGFP->field_);
  }
    
  aCoefficients = pGGFP->coefficients_;
  aLength = pGGFP->coefficientsSize_; 
    
  bCoefficients = other->coefficients_;
  bLength = other->coefficientsSize_;
    
  //ArrayRef<int> product(new Array<int>(aLength + bLength - 1));
  MALLOC(product,int,aLength + bLength - 1);
  for (i = 0; i < aLength; i++) {
    aCoeff = aCoefficients[i];
    for (j = 0; j < bLength; j++) {
      product[i+j] = GenericGFAddOrSubtract(product[i+j], 
                                              GenericGFMultiply(pGGFP->field_,aCoeff, bCoefficients[j]));
    }
  }
    
  GenericGFPolyUninit(pOut);

  ret = GenericGFPolyInit(pOut,pGGFP->field_,
	    product,aLength + bLength - 1);

ERROR_EXIT:
  FREE(product);
  
  if(ret==0)
  {
	  *bUninit = TRUE;
	  return pOut;
  }else{
	  *bUninit = FALSE;
	  return NULL;
  }
}
  
_GenericGFPoly *GenericGFPolyMultiplyByScale(_GenericGFPoly *pGGFP,int scalar,_GenericGFPoly *pOut,BOOL *bUninit)
{
  int ret = 0,size,i;
  int *product = NULL;


  if (scalar == 0) {
	  *bUninit = FALSE;
	  return GenericGFGetZero(pGGFP->field_);
  }
  if (scalar == 1) {
	  *bUninit = FALSE;
      return pGGFP;
  }
  size = pGGFP->coefficientsSize_; 
  MALLOC(product,int,size);
  for (i = 0; i < size; i++) {
    product[i] = GenericGFMultiply(pGGFP->field_,pGGFP->coefficients_[i], scalar);//field_->multiply(coefficients_[i], scalar);
  }
  
  GenericGFPolyUninit(pOut);

  ret = GenericGFPolyInit(pOut,pGGFP->field_,
	                      product,size);
ERROR_EXIT:
  FREE(product);
  if(ret==0)
  {
      *bUninit = TRUE;
	  return pOut;
  }else{
	  bUninit = FALSE;
	  return NULL;
  }
}
  
_GenericGFPoly *GenericGFPolyMultiplyByMonomial(_GenericGFPoly *pGGFP,int degree, int coefficient,_GenericGFPoly *pOut,BOOL *bUninit)
{
  int ret = 0,size,i;
  int *product = NULL;

  if (degree < 0) {
      //throw IllegalArgumentException("degree must not be less then 0");
	  *bUninit = FALSE;
 	  return NULL;
  }

  if (coefficient == 0) {
	 *bUninit = FALSE;
	 return GenericGFGetZero(pGGFP->field_);
  }
  size = pGGFP->coefficientsSize_; 
  MALLOC(product,int,size+degree);
  for (i = 0; i < size; i++) {
    product[i] = GenericGFMultiply(pGGFP->field_,pGGFP->coefficients_[i], coefficient);//field_->multiply(coefficients_[i], coefficient);
  }

  GenericGFPolyUninit(pOut);

  ret = GenericGFPolyInit(pOut,pGGFP->field_,
	                      product,size+degree);
ERROR_EXIT:
  FREE(product);
  if(ret==0)
  {
	  *bUninit = TRUE;
      return pOut;
  }else{
	  *bUninit = FALSE;
      return NULL;
  }
}
  
// int GenericGFPolyDivide(_GenericGFPoly *pGGFP,_GenericGFPoly *other,_GenericGFPoly **ppOut,int *pOutSize)
// {
// //   if (!(field_.object_ == other->field_.object_)) {
// //     throw IllegalArgumentException("GenericGFPolys do not have same GenericGF field");
// //   }
//   int ret = 0,denominatorLeadingTerm,inverseDenominatorLeadingTerm,degreeDifference,scale,m=0;
//   _GenericGFPoly *quotient = NULL,*remainder = NULL,*term=NULL,*iterationQuotiont=NULL,*quotientA = NULL,*remainderA = NULL,*quotientB = NULL,*remainderB = NULL;
//   
//   _GenericGFPoly mout1,mout2,mout3,mout4;
// 
//   BOOL bUninit1 = FALSE,bUninit2 = FALSE,bUninit3 = FALSE,bUninit4 = FALSE;
// 
//   if (GenericGFPolyIsZero(other)) {
//       //throw IllegalArgumentException("divide by 0");
// 	  return RT_ERR_DIVIDEBYZERO;
//   }
//     
//   memset(&mout1,0,sizeof(_GenericGFPoly));
//   memset(&mout2,0,sizeof(_GenericGFPoly));
//   memset(&mout3,0,sizeof(_GenericGFPoly));
//   memset(&mout4,0,sizeof(_GenericGFPoly));
// 
//   quotient = GenericGFGetZero(pGGFP->field_);
//   remainder = pGGFP;
//     
//   denominatorLeadingTerm = GenericGFPolyGetCoefficient(other,GenericGFPolyGetDegree(other));//other->getCoefficient(other->getDegree());
//   inverseDenominatorLeadingTerm = GenericGFInverse(pGGFP->field_,denominatorLeadingTerm);//field_->inverse(denominatorLeadingTerm);
//     
//   while (GenericGFPolyGetDegree(remainder) >= GenericGFPolyGetDegree(other) && !GenericGFPolyIsZero(remainder))
//   {
//        degreeDifference = GenericGFPolyGetDegree(remainder) - GenericGFPolyGetDegree(other);
//        scale = GenericGFMultiply(pGGFP->field_,GenericGFPolyGetCoefficient(remainder,GenericGFPolyGetDegree(remainder)),
//                                  inverseDenominatorLeadingTerm);
// 
//        
// 
//        term = GenericGFPolyMultiplyByMonomial(other,degreeDifference,scale,&mout1,&bUninit1);
// 	   if(term == NULL) {ret = RT_ERR_ALGORITHM;goto ERROR_EXIT;}
//        iterationQuotiont = GenericGFBuildMonomial(pGGFP->field_,degreeDifference,scale,&mout2,&bUninit2);
// 	   if(iterationQuotiont == NULL) {ret = RT_ERR_ALGORITHM;goto ERROR_EXIT;}
// 
// 	   quotient = GenericGFPolyAddOrSubtract(quotient,iterationQuotiont,&mout3,&bUninit3);
// 	   if(quotient == NULL) {ret = RT_ERR_ALGORITHM;goto ERROR_EXIT;}
// 	   remainder = GenericGFPolyAddOrSubtract(remainder,term,&mout4,&bUninit4);
// 	   if(remainder == NULL) {ret = RT_ERR_ALGORITHM;goto ERROR_EXIT;}
//   }
// //   std::vector<Ref<GenericGFPoly> > returnValue;
// //   returnValue[0] = quotient;
// //   returnValue[1] = remainder;
// //   return returnValue;
// ERROR_EXIT:
//     return ret;
// }

