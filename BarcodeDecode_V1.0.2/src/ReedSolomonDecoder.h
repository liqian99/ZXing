#ifndef __REED_SOLOMON_DECODER_H__
#define __REED_SOLOMON_DECODER_H__
#include "GenericGF.h"

typedef struct _ReedSolomonDecoder{
    GenericGF *field_;
}ReedSolomonDecoder;

void ReedSolomonDecoderInit(ReedSolomonDecoder *pRSD,GenericGF *field);
int ReedSolomonDecoderDecode(ReedSolomonDecoder *pRSD,int *received,int receivedSize,int twoS);
int ReedSolomonDecoderRunEuclideanAlgorithm(ReedSolomonDecoder *pRSD,_GenericGFPoly *a,
                                              _GenericGFPoly *b,int R,
											  _GenericGFPoly **sigma,_GenericGFPoly **omega); 
int ReedSolomonDecoderFindErrorLocations(ReedSolomonDecoder *pRSD,_GenericGFPoly *errorLocator,int **ppResult,int *pResultLen);
int ReedSolomonDecoderFindErrorMagnitudes(ReedSolomonDecoder *pRSD,_GenericGFPoly *errorEvaluator, int* errorLocations,int errorLocationsSize,int **ppResult,int *pResultLen);
void ReedSolomonDecoderUninit(void);


#endif