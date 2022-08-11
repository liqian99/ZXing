#include "PerspectiveTransform.h"

void PerspectiveTransformInit(PerspectiveTransform *pPT,float inA11, float inA21, 
                         float inA31, float inA12, 
                         float inA22, float inA32, 
                         float inA13, float inA23, 
                         float inA33) 
{
   pPT->a11 = inA11;
   pPT->a12 = inA12;
   pPT->a13 = inA13;
   pPT->a21 = inA21;
   pPT->a22 = inA22;
   pPT->a23 = inA23;
   pPT->a31 = inA31;
   pPT->a32 = inA32;
   pPT->a33 = inA33;
}

PerspectiveTransform PerspectiveTransformQuadrilateralToQuadrilateral(float x0, float y0, float x1, float y1,
																	  float x2, float y2, float x3, float y3, float x0p, float y0p, float x1p, float y1p, float x2p, float y2p,
																	  float x3p, float y3p)
{
	PerspectiveTransform qToS = PerspectiveTransformQuadrilateralToSquare(x0, y0, x1, y1, x2, y2, x3, y3);
	PerspectiveTransform sToQ =	PerspectiveTransformSquareToQuadrilateral(x0p, y0p, x1p, y1p, x2p, y2p, x3p, y3p);
	return PerspectiveTransformTimes(&sToQ,&qToS);
}

PerspectiveTransform PerspectiveTransformSquareToQuadrilateral(float x0, float y0, float x1, float y1, float x2,
															   float y2, float x3, float y3) 
{
	float dx3 = x0 - x1 + x2 - x3;
	float dy3 = y0 - y1 + y2 - y3;
	PerspectiveTransform result;
	float dx1;
	float dx2;
	float dy1;
	float dy2;
	float denominator;
	float a13;
	float a23;


	if (dx3 == 0.0f && dy3 == 0.0f) {
		PerspectiveTransformInit(&result,x1 - x0, x2 - x1, x0, y1 - y0, y2 - y1, y0, 0.0f,
			0.0f, 1.0f);
		return result;
	} else {
		dx1 = x1 - x2;
		dx2 = x3 - x2;
		dy1 = y1 - y2;
		dy2 = y3 - y2;
		denominator = dx1 * dy2 - dx2 * dy1;
		a13 = (dx3 * dy2 - dx2 * dy3) / denominator;
		a23 = (dx1 * dy3 - dx3 * dy1) / denominator;
		PerspectiveTransformInit(&result,x1 - x0 + a13 * x1, x3 - x0 + a23 * x3, x0, y1 - y0
			+ a13 * y1, y3 - y0 + a23 * y3, y0, a13, a23, 1.0f);
		return result;
	}
}

PerspectiveTransform PerspectiveTransformQuadrilateralToSquare(float x0, float y0, float x1, float y1, float x2,
															   float y2, float x3, float y3)
{
	// Here, the adjoint serves as the inverse:
	PerspectiveTransform tmp;

	tmp = PerspectiveTransformSquareToQuadrilateral(x0, y0, x1, y1, x2, y2, x3, y3);

	return PerspectiveTransformBuildAdjoint(&tmp);
}

PerspectiveTransform PerspectiveTransformBuildAdjoint(PerspectiveTransform *pPT)
{
	// Adjoint is the transpose of the cofactor matrix:
	PerspectiveTransform result;
	
	PerspectiveTransformInit(&result,pPT->a22 * pPT->a33 - pPT->a23 * pPT->a32, pPT->a23 * pPT->a31 - pPT->a21 * pPT->a33, pPT->a21 * pPT->a32
		- pPT->a22 * pPT->a31, pPT->a13 * pPT->a32 - pPT->a12 * pPT->a33, pPT->a11 * pPT->a33 - pPT->a13 * pPT->a31, pPT->a12 * pPT->a31 - pPT->a11 * pPT->a32, pPT->a12 * pPT->a23 - pPT->a13 * pPT->a22,
		pPT->a13 * pPT->a21 - pPT->a11 * pPT->a23, pPT->a11 * pPT->a22 - pPT->a12 * pPT->a21);

	return result;
}

PerspectiveTransform PerspectiveTransformTimes(PerspectiveTransform *pPT,PerspectiveTransform *other) 
{
	PerspectiveTransform result;
		
	PerspectiveTransformInit(&result,pPT->a11 * other->a11 + pPT->a21 * other->a12 + pPT->a31 * other->a13,
		pPT->a11 * other->a21 + pPT->a21 * other->a22 + pPT->a31 * other->a23, pPT->a11 * other->a31 + pPT->a21 * other->a32 + pPT->a31
		* other->a33, pPT->a12 * other->a11 + pPT->a22 * other->a12 + pPT->a32 * other->a13, pPT->a12 * other->a21 + pPT->a22
		* other->a22 + pPT->a32 * other->a23, pPT->a12 * other->a31 + pPT->a22 * other->a32 + pPT->a32 * other->a33, pPT->a13
		* other->a11 + pPT->a23 * other->a12 + pPT->a33 * other->a13, pPT->a13 * other->a21 + pPT->a23 * other->a22 + pPT->a33
		* other->a23, pPT->a13 * other->a31 + pPT->a23 * other->a32 + pPT->a33 * other->a33);

	return result;
}

void PerspectiveTransformTransformPoints(PerspectiveTransform *pPT,float *points,int pointsSize)
{
	int nmax = pointsSize,i;
	float x,y,denominator;

	for (i = 0; i < nmax; i += 2) {
		x = points[i];
		y = points[i + 1];
		denominator = pPT->a13 * x + pPT->a23 * y + pPT->a33;
		points[i] = (pPT->a11 * x + pPT->a21 * y + pPT->a31) / denominator;
		points[i + 1] = (pPT->a12 * x + pPT->a22 * y + pPT->a32) / denominator;
	}
}