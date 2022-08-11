#ifndef __QR_ERROR_CORRECT_LEVEL_H__
#define __QR_ERROR_CORRECT_LEVEL_H__
#include "../BaseTypeDef.h"


#define N_LEVELS  4

typedef struct _QRErrorCorrectLevel{
    int ordinal_;
	int bits_;
    char *name_;
}QRErrorCorrectLevel;

void QRErrorCorrectLevelInit(QRErrorCorrectLevel *pLevel,int inOrdinal,int bits,const char *name);
int QRErrorCorrectLevelGetOrdinal(QRErrorCorrectLevel *pLevel);
int QRErrorCorrectLevelGetBits(QRErrorCorrectLevel *pLevel);
char *QRErrorCorrectLevelGetName(QRErrorCorrectLevel *pLevel);
QRErrorCorrectLevel *QRErrorCorrectLevelForBits(int bits);

#endif