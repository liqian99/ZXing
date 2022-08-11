#include "QRMode.h"
#include "../DiyMalloc.h"

QRMode QRMODE_TERMINATOR;
QRMode QRMODE_NUMERIC;
QRMode QRMODE_ALPHANUMERIC;
QRMode QRMODE_STRUCTURED_APPEND;
QRMode QRMODE_BYTE;
QRMode QRMODE_ECI;
QRMode QRMODE_KANJI;
QRMode QRMODE_FNC1_FIRST_POSITION;
QRMode QRMODE_FNC1_SECOND_POSITION;
QRMode QRMODE_HANZI;

void QRModeInit(QRMode *pMode,int cbv0_9, int cbv10_26, int cbv27, int  bits , char const* name)
{
    pMode->characterCountBitsForVersions0To9_ = cbv0_9;
	pMode->characterCountBitsForVersions10To26_ = cbv10_26;
	pMode->characterCountBitsForVersions27AndHigher_ = cbv27;
	diy_memcpy(pMode->name_,(char *)name,strlen(name));
}

void QRModeAllInit(void)
{
    QRModeInit(&QRMODE_TERMINATOR,0, 0, 0, 0x00, "TERMINATOR");
    QRModeInit(&QRMODE_NUMERIC,10, 12, 14, 0x01, "NUMERIC");
	QRModeInit(&QRMODE_ALPHANUMERIC,9, 11, 13, 0x02, "ALPHANUMERIC");
	QRModeInit(&QRMODE_STRUCTURED_APPEND,0, 0, 0, 0x03, "STRUCTURED_APPEND");
	QRModeInit(&QRMODE_BYTE,8, 16, 16, 0x04, "BYTE");
	QRModeInit(&QRMODE_ECI,0, 0, 0, 0x07, "ECI");
	QRModeInit(&QRMODE_KANJI,8, 10, 12, 0x08, "KANJI");
	QRModeInit(&QRMODE_FNC1_FIRST_POSITION,0, 0, 0, 0x05, "FNC1_FIRST_POSITION");
	QRModeInit(&QRMODE_FNC1_SECOND_POSITION,0, 0, 0, 0x09, "FNC1_SECOND_POSITION");
	QRModeInit(&QRMODE_HANZI,8, 10, 12, 0x0D, "HANZI");

}
QRMode *QRModeForBits(int bits)
{
	switch (bits)
	{
		case 0x0:
			return &QRMODE_TERMINATOR;
		case 0x1:
			return &QRMODE_NUMERIC;
		case 0x2:
			return &QRMODE_ALPHANUMERIC;
		case 0x3:
			return &QRMODE_STRUCTURED_APPEND;
		case 0x4:
			return &QRMODE_BYTE;
		case 0x5:
			return &QRMODE_FNC1_FIRST_POSITION;
		case 0x7:
			return &QRMODE_ECI;
		case 0x8:
			return &QRMODE_KANJI;
		case 0x9:
			return &QRMODE_FNC1_SECOND_POSITION;
		case 0xD:
			// 0xD is defined in GBT 18284-2000, may not be supported in foreign country
			return &QRMODE_HANZI;
		default:
			return NULL;
	}
}

int QRModeGetCharacterCountBits(QRMode *pMode,QRVersion *version) {
	int number = QRVersionGetVersionNumber(version); 
	if (number <= 9) {
		return pMode->characterCountBitsForVersions0To9_;
	} else if (number <= 26) {
		return pMode->characterCountBitsForVersions10To26_;
	} else {
		return pMode->characterCountBitsForVersions27AndHigher_;
	}
}