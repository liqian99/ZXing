#ifndef __GRIDSAMPLER_H__
#define __GRIDSAMPLER_H__
#include "BitMatrix.h"
#include "PerspectiveTransform.h"

int GridSamplerSampleGridA(Bitmatrix *image, Bitmatrix **ppbits,int dimension, PerspectiveTransform *transform);
int GridSamplerSampleGridB(Bitmatrix *image,Bitmatrix **ppbits,int dimensionX, int dimensionY, PerspectiveTransform *transform);
int GridSamplerSampleGridC(Bitmatrix *image, Bitmatrix **ppbits,int dimension, float p1ToX, float p1ToY, float p2ToX,
						   float p2ToY, float p3ToX, float p3ToY, float p4ToX, float p4ToY, float p1FromX, float p1FromY, float p2FromX,
						   float p2FromY, float p3FromX, float p3FromY, float p4FromX, float p4FromY); 
int GridSamplerCheckAndNudgePoints(Bitmatrix *image, float *points,int pointsSize);

#endif
