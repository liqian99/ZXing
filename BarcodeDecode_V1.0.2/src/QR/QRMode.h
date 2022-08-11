#ifndef __QR_MODE_H__
#define __QR_MODE_H__
#include "../BaseTypeDef.h"
#include "QRVersion.h"

typedef struct _QRMode{
	int characterCountBitsForVersions0To9_;
	int characterCountBitsForVersions10To26_;
	int characterCountBitsForVersions27AndHigher_;
    char name_[32];
}QRMode;

extern QRMode QRMODE_TERMINATOR;
extern QRMode QRMODE_NUMERIC;
extern QRMode QRMODE_ALPHANUMERIC;
extern QRMode QRMODE_STRUCTURED_APPEND;
extern QRMode QRMODE_BYTE;
extern QRMode QRMODE_ECI;
extern QRMode QRMODE_KANJI;
extern QRMode QRMODE_FNC1_FIRST_POSITION;
extern QRMode QRMODE_FNC1_SECOND_POSITION;
extern QRMode QRMODE_HANZI;

void QRModeInit(QRMode *pMode,int cbv0_9, int cbv10_26, int cbv27, int /* bits */, char const* name);
void QRModeAllInit(void);
QRMode *QRModeForBits(int bits);
int QRModeGetCharacterCountBits(QRMode *pMode,QRVersion *version);

#endif 