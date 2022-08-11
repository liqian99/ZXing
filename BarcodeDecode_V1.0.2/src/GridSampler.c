#include "GridSampler.h"
#include "DiyMalloc.h"

int GridSamplerSampleGridA(Bitmatrix *image, Bitmatrix **ppbits,int dimension, PerspectiveTransform *transform)
{
	float *points;
    int x,y,nmax,pointsSize,ret = 0;
	float yValue;
	
	MALLOC(*ppbits,Bitmatrix,1);
	BitMatrixInit(*ppbits,dimension,dimension);
    pointsSize = dimension << 1;
	MALLOC(points,float,dimension << 1);


	for (y = 0; y < dimension; y++) {
		nmax = pointsSize;
		yValue = (float)y + 0.5f;
		for (x = 0; x < nmax; x += 2) {
			points[x] = (float)(x >> 1) + 0.5f;
			points[x + 1] = yValue;
		}
		PerspectiveTransformTransformPoints(transform,points,pointsSize);
		ret = GridSamplerCheckAndNudgePoints(image, points,pointsSize);
		if(ret != 0) goto ERROR_EXIT;
		for (x = 0; x < nmax; x += 2) {
			if (BitMatrixGet(image,(int)points[x], (int)points[x + 1])) 
			{
				BitMatrixSet(*ppbits,x >> 1, y);
			}

		}
	}
	//return bits;
ERROR_EXIT:
	FREE(points);
	return ret;
}

int GridSamplerSampleGridB(Bitmatrix *image,Bitmatrix **ppbits,int dimensionX, int dimensionY, PerspectiveTransform *transform)
{
	float *points;
    int x,y,nmax,pointsSize,ret = 0;
	float yValue;

	BitMatrixInit(*ppbits,dimensionX, dimensionY);

	pointsSize = dimensionX << 1;
    MALLOC(points,float,dimensionX << 1);

	for (y = 0; y < dimensionY; y++) {
		nmax = pointsSize;
		yValue = (float)y + 0.5f;
		for (x = 0; x < nmax; x += 2) {
			points[x] = (float)(x >> 1) + 0.5f;
			points[x + 1] = yValue;
		}
		PerspectiveTransformTransformPoints(transform,points,pointsSize);
		ret = GridSamplerCheckAndNudgePoints(image, points,pointsSize);
		if(ret != 0) goto ERROR_EXIT;
		for (x = 0; x < nmax; x += 2) {
			if (BitMatrixGet(image,(int)points[x], (int)points[x + 1])) 
			{
				BitMatrixSet(*ppbits,x >> 1, y);
			}
		}
	}

ERROR_EXIT:
	FREE(points);
	return ret;
}

int GridSamplerSampleGridC(Bitmatrix *image, Bitmatrix **ppbits,int dimension, float p1ToX, float p1ToY, float p2ToX,
                                float p2ToY, float p3ToX, float p3ToY, float p4ToX, float p4ToY, float p1FromX, float p1FromY, float p2FromX,
                                float p2FromY, float p3FromX, float p3FromY, float p4FromX, float p4FromY) 
{
	PerspectiveTransform transform;
	transform  = PerspectiveTransformQuadrilateralToQuadrilateral(p1ToX, p1ToY, p2ToX, p2ToY,
		         p3ToX, p3ToY, p4ToX, p4ToY, p1FromX, p1FromY, p2FromX, p2FromY, p3FromX, p3FromY, p4FromX, p4FromY);
	
	return GridSamplerSampleGridA(image,ppbits,dimension, &transform);
	
}

int GridSamplerCheckAndNudgePoints(Bitmatrix *image, float *points,int pointsSize)
{
	int width = BitMatrixGetWidth(image);   //image->getWidth();
	int height = BitMatrixGetHeight(image); //image->getHeight();
	int offset,x,y;

	// The Java code assumes that if the start and end points are in bounds, the rest will also be.
	// However, in some unusual cases points in the middle may also be out of bounds.
	// Since we can't rely on an ArrayIndexOutOfBoundsException like Java, we check every point.
	
	for (offset = 0; offset < pointsSize; offset += 2) {
		x = (int)points[offset];
		y = (int)points[offset + 1];
		if (x < -1 || x > width || y < -1 || y > height) {
// 			ostringstream s;
// 			s << "Transformed point out of bounds at " << x << "," << y;
// 			throw ReaderException(s.str().c_str());
            return RT_ERR_OUTBOUND;
		}
		
		if (x == -1) {
			points[offset] = 0.0f;
		} else if (x == width) {
			points[offset] = (float)(width - 1);
		}
		if (y == -1) {
			points[offset + 1] = 0.0f;
		} else if (y == height) {
			points[offset + 1] = (float)(height - 1);
		}
	}
	
	return 0;
}