#ifndef __QR_VERSION_H__
#define __QR_VERSION_H__

#include "../BitMatrix.h"
#include "QRFormatInfo.h"
#include "QRErrorCorrectLevel.h"

typedef struct _QRECB
{
    int count_;
	int dataCodewords_;
}QRECB;

typedef struct _QRECBlocks
{
    int ecCodewords_;
	int ecBSize_;
	QRECB ecB1_;
	QRECB ecB2_;
}QRECBlocks;

typedef struct _QRVersion
{
    int versionNumber_;
	int alignmentPatternCentersSize_;
	int *alignmentPatternCenters_;
	int ecBlocksSize_;
	QRECBlocks *ecBlocks_;
	int totalCodewords_;
}QRVersion;

#define N_VERSION_DECODE_INFOS  34
#define N_VERSIONS 40

extern QRVersion *g_pVersionGroup;

void QRECBInit(QRECB *pECB,int count,int dataCodewords);
int QRECBGetCount(QRECB *pECB);
int QRECBGetDataCodewords(QRECB *pECB);
void QRECBlocksInit(QRECBlocks *pECBlocks,int ecCodewords,QRECB *ecBlocks);
void QRECBlocksUninit(QRECBlocks *pECBlocks);
void QRECBlocksInitA(QRECBlocks *pECBlocks,int ecCodewords,QRECB *ecBlocks1,QRECB *ecBlocks2);
int QRECBlockGetECCodewords(QRECBlocks *pECBlocks);
//QRECB *QRECBlocksGetECBlocks(QRECBlocks *pECBlocks);
void QRECBlocksGetECBlocks(QRECB *pECB,int *ECBSize,QRECBlocks *pECBlocks);
int QRVersionGetVersionNumber(QRVersion *pVersion);
int *QRVersionGetAlignmentPatternCenters(QRVersion *pVersion);
int QRVersionGetTotalCodewords(QRVersion *pVersion);
int QRVersionGetDimensionForVersion(QRVersion *pVersion);
QRECBlocks QRVersionGetECBlocksForLevel(QRVersion *pVersion,QRErrorCorrectLevel *pecLevel);
QRVersion *QRVersionGetProvisonalVersionForDimension(QRVersion *pVersionGroup,int dimension);
QRVersion *QRVersionGetVersionForNumber(QRVersion *pVersionGroup,int versionNumber);
int QRVersionInit(QRVersion *pVersion,int versionNumber,int *alignmentPatternCenters,int alignmentPatternCentersSize,
				  QRECBlocks *ecBlocks1,QRECBlocks *ecBlocks2,
				  QRECBlocks *ecBlocks3,QRECBlocks *ecBlocks4);
void QRVersionUninit(QRVersion *pVersion);
int QRVersionBuildVersions(QRVersion *pVersionGroup);
void QRVersionDestroyVersions(QRVersion *pVersionGroup);
QRVersion *QRVersionDecodeVersionInformation(QRVersion *pVersionGroup,unsigned int versionBits);
int QRVersionBuildFunctionPattern(Bitmatrix **ppBitmatrix,QRVersion *pVersion);
void QRVersionDestroyFunctionPattern(Bitmatrix *pBitmatrix);
int QRVersionGetAlignmentPatternCentersSize(QRVersion *pVersion);
#endif