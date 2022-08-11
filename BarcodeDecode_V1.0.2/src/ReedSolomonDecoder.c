#include "ReedSolomonDecoder.h"
#include "DiyMalloc.h"

// _GenericGFPoly g_mout1,g_mout2,g_mout3,g_mout4,g_mout5,g_mout6,g_mout7,g_mout8,g_mout9,g_mout10;
// BOOL g_bUninit1 = FALSE,g_bUninit2 = FALSE,g_bUninit3=FALSE,g_bUninit4=FALSE,g_bUninit5=FALSE,g_bUninit6=FALSE,g_bUninit7=FALSE,g_bUninit8=FALSE,g_bUninit9=FALSE,g_bUninit10=FALSE;
// _GenericGFPoly g_mout11,g_mout12,g_mout13;
// BOOL g_bUninit11 = FALSE,g_bUninit12 = FALSE,g_bUninit13 = FALSE;

#define MAX_MOUT  1024

_GenericGFPoly *g_mout = NULL;
BOOL g_bUninit = FALSE;


void ReedSolomonDecoderInit(ReedSolomonDecoder *pRSD,GenericGF *field)
{
	int ret;

    pRSD->field_ = field;

	MALLOC(g_mout,_GenericGFPoly,MAX_MOUT);


ERROR_EXIT:
	return;
}

void ReedSolomonDecoderUninit()
{
	int i=0;

	for(i=0;i<MAX_MOUT;i++)
	{
        GenericGFPolyUninit(&g_mout[i]);
	}
	FREE(g_mout);
}

int ReedSolomonDecoderDecode(ReedSolomonDecoder *pRSD,int *received,int receivedSize,int twoS)
{
  int ret,i,eval,syndromeCoefficientsSize,position;
  int *syndromeCoefficients = NULL;
  _GenericGFPoly poly;
  BOOL noError = TRUE;
  _GenericGFPoly syndrome;
  _GenericGFPoly *pTmp = NULL,*sigma = NULL,*omega = NULL;
  int *errorLocations = NULL,*errorMagitudes = NULL;
  int errorLocationsSize = 0,errorMagitudesSize=0;
  _GenericGFPoly mout;
  BOOL bUninit = FALSE;

  diy_memset(&poly,0,sizeof(_GenericGFPoly));
  diy_memset(&syndrome,0,sizeof(_GenericGFPoly));
  diy_memset(&mout,0,sizeof(_GenericGFPoly));

  ret = GenericGFPolyInit(&poly,pRSD->field_,received,receivedSize);
  if(ret!=0) goto ERROR_EXIT;

  MALLOC(syndromeCoefficients,int,twoS);
  syndromeCoefficientsSize = twoS;

  for (i = 0; i < twoS; i++) {
	position = GenericGFExp(pRSD->field_,i + GenericGFGetGeneratorBase(pRSD->field_));
 	eval = GenericGFPolyEvaluateAt(&poly,position);
	//eval = GenericGFPolyEvaluateAt(&poly,GenericGFExp(pRSD->field_,i + GenericGFGetGeneratorBase(pRSD->field_)));
    syndromeCoefficients[syndromeCoefficientsSize - 1 - i] = eval;
    if (eval != 0) {
      noError = FALSE;
    }
  }
  if (noError)
  {
     ret = 0;
	 goto ERROR_EXIT;
  }

  ret = GenericGFPolyInit(&syndrome,pRSD->field_,syndromeCoefficients,syndromeCoefficientsSize);
  if(ret != 0) goto ERROR_EXIT;

  pTmp = GenericGFBuildMonomial(pRSD->field_,twoS,1,&mout,&bUninit);
  if(pTmp == NULL) goto ERROR_EXIT;
  

  ret = ReedSolomonDecoderRunEuclideanAlgorithm(pRSD,pTmp,&syndrome,twoS,&sigma,&omega);
  if(ret != 0) goto ERROR_EXIT;


  ret  = ReedSolomonDecoderFindErrorLocations(pRSD,sigma,&errorLocations,&errorLocationsSize);
  if(ret != 0) goto ERROR_EXIT;

  ret = ReedSolomonDecoderFindErrorMagnitudes(pRSD,omega,errorLocations,errorLocationsSize,&errorMagitudes,&errorMagitudesSize);
  if(ret != 0) goto ERROR_EXIT;

  for (i = 0; i < errorLocationsSize; i++) {
    position = receivedSize - 1 - GenericGFLog(pRSD->field_,errorLocations[i]);//field->log(errorLocations[i]);
    if (position < 0) {
      //throw ReedSolomonException("Bad error location");
	  ret = RT_ERR_BADERRLOCATION;
	  goto ERROR_EXIT;
    }
    received[position] = GenericGFAddOrSubtract(received[position], errorMagitudes[i]);
  }

ERROR_EXIT:

  GenericGFPolyUninit(&poly);
  GenericGFPolyUninit(&syndrome);

  GenericGFPolyUninit(&mout);

  FREE(syndromeCoefficients);
  FREE(errorLocations);
  FREE(errorMagitudes);
  return ret;
}

int ReedSolomonDecoderRunEuclideanAlgorithm(ReedSolomonDecoder *pRSD,_GenericGFPoly *a,
                                            _GenericGFPoly *b,int R,
											_GenericGFPoly **sigma,_GenericGFPoly **omega) 
{
  _GenericGFPoly *tmp = NULL;
  _GenericGFPoly *rLast = NULL;
  _GenericGFPoly *r = NULL;
  _GenericGFPoly *tLast = NULL;
  _GenericGFPoly *t = NULL,*rLastLast=NULL,*tLastLast=NULL,*q = NULL;
  int denominatorLeadingTerm,dltInverse,degreeDiff,scale;
  int m=0,ret = 0,sigmaTildeAtZero,inverse;

  _GenericGFPoly *qt =NULL,*rt=NULL,*tt=NULL;


  // Assume a's degree is >= b's
  if (GenericGFPolyGetDegree(a) < GenericGFPolyGetDegree(b)) 
  {
     tmp = a;
     a = b;
     b = tmp;
  }

  rLast = a;
  r = b;
  tLast = GenericGFGetZero(pRSD->field_);//(field->getZero());
  t = GenericGFGetOne(pRSD->field_); //(field->getOne());

  // Run Euclidean algorithm until r's degree is less than R/2
  while (GenericGFPolyGetDegree(r) >= R / 2) 
  {
    rLastLast = rLast;
    tLastLast = tLast;
    rLast = r;
    tLast = t;

    // Divide rLastLast by rLast, with quotient q and remainder r
    if (GenericGFPolyIsZero(rLast)) {
        // Oops, Euclidean algorithm already terminated?
        //throw ReedSolomonException("r_{i-1} was zero");
		return RT_ERR_ISZERO;
    }
    r = rLastLast;
    q = GenericGFGetZero(pRSD->field_);
    denominatorLeadingTerm =GenericGFPolyGetCoefficient(rLast,GenericGFPolyGetDegree(rLast));
    dltInverse = GenericGFInverse(pRSD->field_,denominatorLeadingTerm);
    while (GenericGFPolyGetDegree(r) >= GenericGFPolyGetDegree(rLast) && !GenericGFPolyIsZero(r)) 
	{
      degreeDiff = GenericGFPolyGetDegree(r) - GenericGFPolyGetDegree(rLast);
      scale = GenericGFMultiply(pRSD->field_,GenericGFPolyGetCoefficient(r,GenericGFPolyGetDegree(r)),dltInverse);
    
	  qt = GenericGFBuildMonomial(pRSD->field_,degreeDiff,scale,&g_mout[m++],&g_bUninit);
	  if(qt == NULL || m >= (MAX_MOUT-2)) {ret = RT_ERR_ALGORITHM;goto ERROR_EXIT;}

	  q = GenericGFPolyAddOrSubtract(q,qt,&g_mout[m++],&g_bUninit);
	  if(q == NULL || m >= (MAX_MOUT-2)) {ret = RT_ERR_ALGORITHM;goto ERROR_EXIT;}

	  
	  rt = GenericGFPolyMultiplyByMonomial(rLast,degreeDiff,scale,&g_mout[m++],&g_bUninit);
	  if(rt == NULL || m >= (MAX_MOUT-2)) {ret = RT_ERR_ALGORITHM;goto ERROR_EXIT;}
 
	  r = GenericGFPolyAddOrSubtract(r,rt,&g_mout[m++],&g_bUninit);
	  if(r == NULL || m >= (MAX_MOUT-2)) {ret = RT_ERR_ALGORITHM;goto ERROR_EXIT;}
	
	  

      m++;
    }

    tt = GenericGFPolyMultiply(q,tLast,&g_mout[m++],&g_bUninit);
    if(tt == NULL || m >= (MAX_MOUT-2)) {ret = RT_ERR_ALGORITHM;goto ERROR_EXIT;}

	t = GenericGFPolyAddOrSubtract(tt,tLastLast,&g_mout[m++],&g_bUninit);
    if(t == NULL || m >= (MAX_MOUT-2)) {ret = RT_ERR_ALGORITHM;goto ERROR_EXIT;}

    if (GenericGFPolyGetDegree(r) >= GenericGFPolyGetDegree(rLast)) {
		ret = RT_ERR_ALGORITHM;
		goto ERROR_EXIT;
    }
}

  sigmaTildeAtZero = GenericGFPolyGetCoefficient(t,0);
  if (sigmaTildeAtZero == 0) {
	  ret = RT_ERR_ISZERO;
	  goto ERROR_EXIT;
  }

  inverse = GenericGFInverse(pRSD->field_,sigmaTildeAtZero); 

  *sigma = GenericGFPolyMultiplyByScale(t,inverse,&g_mout[m++],&g_bUninit);
  *omega = GenericGFPolyMultiplyByScale(r,inverse,&g_mout[m++],&g_bUninit);


ERROR_EXIT:
  

  return ret;
}

int ReedSolomonDecoderFindErrorLocations(ReedSolomonDecoder *pRSD,_GenericGFPoly *errorLocator,int **ppResult,int *pResultLen)
{
  // This is a direct application of Chien's search
  int ret = 0,e,i;
  int numErrors = GenericGFPolyGetDegree(errorLocator);//errorLocator->getDegree();
  if (numErrors == 1) 
  { // shortcut
	MALLOC(*ppResult,int,1);
	*pResultLen = 1;
    (*ppResult)[0] = GenericGFPolyGetCoefficient(errorLocator,1);//errorLocator->getCoefficient(1);
    return ret;
  }
  MALLOC(*ppResult,int,numErrors);
  *pResultLen = numErrors;
  e = 0;
  for (i = 1; i < GenericGFGetSize(pRSD->field_) && e < numErrors; i++) 
  {
    if (GenericGFPolyEvaluateAt(errorLocator,i) == 0) {
      (*ppResult)[e] = GenericGFInverse(pRSD->field_,i);
      e++;
    }
  }

  if (e != numErrors) {
    //throw ReedSolomonException("Error locator degree does not match number of roots");
	ret = RT_ERR_DEGREENOTMATCH;
  }

ERROR_EXIT:
  return ret;
}

int ReedSolomonDecoderFindErrorMagnitudes(ReedSolomonDecoder *pRSD,_GenericGFPoly *errorEvaluator, int* errorLocations,int errorLocationsSize,int **ppResult,int *pResultLen)
{
  // This is directly applying Forney's Formula
  int s = errorLocationsSize,ret = 0,i,xiInverse,denominator,j,term,termPlus1;
  //ArrayRef<int> result(new Array<int>(s));
  MALLOC(*ppResult,int,s);

  for (i = 0; i < s; i++) {
    xiInverse = GenericGFInverse(pRSD->field_,errorLocations[i]);  //field->inverse(errorLocations[i]);
    denominator = 1;
    for (j = 0; j < s; j++) {
      if (i != j) {
        term = GenericGFMultiply(pRSD->field_,errorLocations[j],xiInverse);  //field->multiply(errorLocations[j], xiInverse);
        termPlus1 = (term & 0x1) == 0 ? term | 1 : term & ~1;
        denominator = GenericGFMultiply(pRSD->field_,denominator,termPlus1); //field->multiply(denominator, termPlus1);
      }
    }
    (*ppResult)[i] = GenericGFMultiply(pRSD->field_,GenericGFPolyEvaluateAt(errorEvaluator,xiInverse),GenericGFInverse(pRSD->field_,denominator));//field->multiply(errorEvaluator->evaluateAt(xiInverse),field->inverse(denominator));
    if (GenericGFGetGeneratorBase(pRSD->field_)/*field->getGeneratorBase()*/ != 0) 
	{
      (*ppResult)[i] = GenericGFMultiply(pRSD->field_,(*ppResult)[i], xiInverse);
    }
  }
  *pResultLen = s;

ERROR_EXIT:
  return ret;
}
