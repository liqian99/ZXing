#ifndef __RESULT_POINT_H__
#define __RESULT_POINT_H__
#include "BaseTypeDef.h"

typedef struct _ResultPoint  //SQYC92416412
{
    float posX_;
	float posY_;
}ResultPoint;

BOOL ResultPointEquals(ResultPoint *pMe,ResultPoint *pOther);
void ResultPointOrderBestPatterns(ResultPoint *patterns);
float ResultPointDistanceA(ResultPoint *pattern1,ResultPoint *pattern2);
float ResultPointDistanceB(float x1, float x2, float y1, float y2);
float ResultPointCrossProductZ(ResultPoint *pointA,ResultPoint *pointB,ResultPoint *pointC);

#endif