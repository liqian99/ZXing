#include "ResultPoint.h"
#include "MathUtils.h"

BOOL ResultPointEquals(ResultPoint *pMe,ResultPoint *pOther)
{
	return (pMe->posX_ == pOther->posX_ && pMe->posY_ == pOther->posY_); 
}

/**
* <p>Orders an array of three ResultPoints in an order [A,B,C] such that AB < AC and
* BC < AC and the angle between BC and BA is less than 180 degrees.
*/
void ResultPointOrderBestPatterns(ResultPoint *patterns)
{
    // Find distances between pattern centers
    float zeroOneDistance = ResultPointDistanceB(patterns[0].posX_, patterns[1].posX_,patterns[0].posY_, patterns[1].posY_);
    float oneTwoDistance = ResultPointDistanceB(patterns[1].posX_, patterns[2].posX_,patterns[1].posY_, patterns[2].posY_);
    float zeroTwoDistance = ResultPointDistanceB(patterns[0].posX_, patterns[2].posX_,patterns[0].posY_, patterns[2].posY_);
	
	ResultPoint pointA, pointB, pointC,temp;
    
	// Assume one closest to other two is B; A and C will just be guesses at first
    if (oneTwoDistance >= zeroOneDistance && oneTwoDistance >= zeroTwoDistance) {
		pointB = patterns[0];
		pointA = patterns[1];
		pointC = patterns[2];
    } else if (zeroTwoDistance >= oneTwoDistance && zeroTwoDistance >= zeroOneDistance) {
		pointB = patterns[1];
		pointA = patterns[0];
		pointC = patterns[2];
    } else {
		pointB = patterns[2];
		pointA = patterns[0];
		pointC = patterns[1];
    }
	
    // Use cross product to figure out whether A and C are correct or flipped.
    // This asks whether BC x BA has a positive z component, which is the arrangement
    // we want for A, B, C. If it's negative, then we've got it flipped around and
    // should swap A and C.
    if (ResultPointCrossProductZ(&pointA, &pointB, &pointC) < 0.0f)
	{
		temp = pointA;
		pointA = pointC;
		pointC = temp;
    }
	
    patterns[0] = pointA;
    patterns[1] = pointB;
    patterns[2] = pointC;
}

float ResultPointDistanceA(ResultPoint *pattern1,ResultPoint *pattern2)
{
	return MathUtilsDistanceA(pattern1->posX_,
		pattern1->posY_,
		pattern2->posX_,
		pattern2->posY_);
}

float ResultPointDistanceB(float x1, float x2, float y1, float y2)
{
	float xDiff = x1 - x2;
	float yDiff = y1 - y2;
	return (float)sqrt((double)(xDiff * xDiff + yDiff * yDiff));
}

float ResultPointCrossProductZ(ResultPoint *pointA,ResultPoint *pointB,ResultPoint *pointC)
{
	float bX = pointB->posX_;
	float bY = pointB->posY_;
	return ((pointC->posX_ - bX) * (pointA->posY_ - bY)) - ((pointC->posY_ - bY) * (pointA->posX_ - bX));
}
