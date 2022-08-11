#include "MathUtils.h"
#include "BaseTypeDef.h"

int MathUtilsRound(float d) {
    return (int) (d + 0.5f);
}

float MathUtilsDistanceA(float aX, float aY, float bX, float bY) {
    float xDiff = aX - bX;
    float yDiff = aY - bY;
    return (float)sqrt(xDiff * xDiff + yDiff * yDiff);
}

float MathUtilsDistanceB(int aX, int aY, int bX, int bY) {
    int xDiff = aX - bX;
    int yDiff = aY - bY;
    return (float)sqrt((float)(xDiff * xDiff + yDiff * yDiff));
}