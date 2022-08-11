#ifndef __PERSPERTIVETRANSFORM_H__
#define __PERSPERTIVETRANSFORM_H__

typedef struct _PerspectiveTransform{
    float a11;
	float a12;
	float a13;
	float a21;
	float a22;
	float a23;
	float a31;
	float a32;
	float a33;
}PerspectiveTransform;

void PerspectiveTransformInit(PerspectiveTransform *pPT,float inA11, float inA21, 
							  float inA31, float inA12, 
							  float inA22, float inA32, 
							  float inA13, float inA23, 
                              float inA33);

PerspectiveTransform PerspectiveTransformQuadrilateralToQuadrilateral(float x0, float y0, float x1, float y1,
																	  float x2, float y2, float x3, float y3, float x0p, float y0p, float x1p, float y1p, float x2p, float y2p,
																	  float x3p, float y3p);
void PerspectiveTransformTransformPoints(PerspectiveTransform *pPT,float *points,int pointsSize);
PerspectiveTransform PerspectiveTransformSquareToQuadrilateral(float x0, float y0, float x1, float y1, float x2,
															   float y2, float x3, float y3);
PerspectiveTransform PerspectiveTransformQuadrilateralToSquare(float x0, float y0, float x1, float y1, float x2,
															   float y2, float x3, float y3);
PerspectiveTransform PerspectiveTransformBuildAdjoint(PerspectiveTransform *pPT);
PerspectiveTransform PerspectiveTransformTimes(PerspectiveTransform *pPT,PerspectiveTransform *other); 
void PerspectiveTransformTransformPoints(PerspectiveTransform *pPT,float *points,int pointsSize);

#endif