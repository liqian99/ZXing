#include "QRErrorCorrectLevel.h"

QRErrorCorrectLevel QRErrorCorrectLevelL = {0,0x01,"L"};
QRErrorCorrectLevel QRErrorCorrectLevelM = {1,0x00,"M"};
QRErrorCorrectLevel QRErrorCorrectLevelQ = {2,0x03,"Q"};
QRErrorCorrectLevel QRErrorCorrectLevelH = {3,0x02,"H"};


void QRErrorCorrectLevelInit(QRErrorCorrectLevel *pLevel,int inOrdinal,int bits,const char *name)
{
	pLevel->bits_ = bits;
	pLevel->ordinal_ = inOrdinal;
	pLevel->name_ = (char *)name;
}

int QRErrorCorrectLevelGetOrdinal(QRErrorCorrectLevel *pLevel)
{
	return pLevel->ordinal_;
}

int QRErrorCorrectLevelGetBits(QRErrorCorrectLevel *pLevel)
{
	return pLevel->bits_;
}

char *QRErrorCorrectLevelGetName(QRErrorCorrectLevel *pLevel)
{
	return pLevel->name_;
}

QRErrorCorrectLevel *QRErrorCorrectLevelForBits(int bits)
{
    if(bits < 0 || bits >= N_LEVELS) return NULL;

	switch(bits)
	{
	case 0:
		return &QRErrorCorrectLevelM;
	case 1:
		return &QRErrorCorrectLevelL;
	case 2:
		return &QRErrorCorrectLevelH;
	default:
        return &QRErrorCorrectLevelQ;
	}
}