#include "QRVersion.h"
#include "../DiyMalloc.h"

/**
*  QRECB
**/
void QRECBInit(QRECB *pECB,int count,int dataCodewords)
{
	pECB->count_ = count;
	pECB->dataCodewords_ = dataCodewords;
}

int QRECBGetCount(QRECB *pECB)
{
	return pECB->count_;
}

int QRECBGetDataCodewords(QRECB *pECB)
{
	return pECB->dataCodewords_;
}


/**
*  QRECBlocks
**/
void QRECBlocksInit(QRECBlocks *pECBlocks,int ecCodewords,QRECB *ecBlocks)
{
	pECBlocks->ecCodewords_ = ecCodewords;
	pECBlocks->ecBSize_ = 1;
    pECBlocks->ecB1_ = *ecBlocks;
}

void QRECBlocksUninit(QRECBlocks *pECBlocks)
{
    pECBlocks->ecCodewords_ = 0;
	pECBlocks->ecBSize_ = 0;
	diy_memset(&pECBlocks->ecB1_,0,sizeof(QRECB));
	diy_memset(&pECBlocks->ecB2_,0,sizeof(QRECB));
}

void QRECBlocksInitA(QRECBlocks *pECBlocks,int ecCodewords,QRECB *ecBlocks1,QRECB *ecBlocks2)
{
	pECBlocks->ecCodewords_ = ecCodewords;
	pECBlocks->ecBSize_ = 2;
	pECBlocks->ecB1_=*ecBlocks1;
	pECBlocks->ecB2_=*ecBlocks2;
}

int QRECBlockGetECCodewords(QRECBlocks *pECBlocks)
{
    return pECBlocks->ecCodewords_;
}

void QRECBlocksGetECBlocks(QRECB *pECB,int *ECBSize,QRECBlocks *pECBlocks)
{
	*ECBSize = pECBlocks->ecBSize_;
	if(*ECBSize==1)
	{
		diy_memcpy(pECB,&pECBlocks->ecB1_,sizeof(QRECB));
	}else if(*ECBSize==2)
	{
        diy_memcpy(pECB,&pECBlocks->ecB1_,sizeof(QRECB));
		diy_memcpy(pECB+1,&pECBlocks->ecB2_,sizeof(QRECB));
	}

}

/**
*  QRVersion
**/
QRVersion *g_pVersionGroup = NULL;

unsigned int VERSION_DECODE_INFO[] = { 0x07C94, 0x085BC, 0x09A99, 0x0A4D3, 0x0BBF6, 0x0C762, 0x0D847, 0x0E60D,
	0x0F928, 0x10B78, 0x1145D, 0x12A17, 0x13532, 0x149A6, 0x15683, 0x168C9, 0x177EC, 0x18EC4, 0x191E1, 0x1AFAB,
	0x1B08E, 0x1CC1A, 0x1D33F, 0x1ED75, 0x1F250, 0x209D5, 0x216F0, 0x228BA, 0x2379F, 0x24B0B, 0x2542E, 0x26A64,
	0x27541, 0x28C69
};


int QRVersionGetVersionNumber(QRVersion *pVersion)
{
    return pVersion->versionNumber_;
}

int *QRVersionGetAlignmentPatternCenters(QRVersion *pVersion)
{
    return pVersion->alignmentPatternCenters_;
}

int QRVersionGetAlignmentPatternCentersSize(QRVersion *pVersion)
{
	return pVersion->alignmentPatternCentersSize_;
}

int QRVersionGetTotalCodewords(QRVersion *pVersion)
{
    return pVersion->totalCodewords_;
}

int QRVersionGetDimensionForVersion(QRVersion *pVersion)
{
	return 17+4*pVersion->versionNumber_;
}

QRECBlocks QRVersionGetECBlocksForLevel(QRVersion *pVersion,QRErrorCorrectLevel *pecLevel)
{
    return pVersion->ecBlocks_[pecLevel->ordinal_];
}

QRVersion *QRVersionGetProvisonalVersionForDimension(QRVersion *pVersionGroup,int dimension)
{
	if(dimension%4 != 1)
		return NULL;
    return QRVersionGetVersionForNumber(pVersionGroup,(dimension-17)>>2);
}

QRVersion *QRVersionGetVersionForNumber(QRVersion *pVersionGroup,int versionNumber)
{
    if(versionNumber<1 || versionNumber>N_VERSIONS)
		return NULL;

	return &pVersionGroup[versionNumber-1];
}

int QRVersionInit(QRVersion *pVersion,int versionNumber,int *alignmentPatternCenters,int alignmentPatternCentersSize,QRECBlocks *ecBlocks1,QRECBlocks *ecBlocks2,
				   QRECBlocks *ecBlocks3,QRECBlocks *ecBlocks4)
{
    int total = 0,i;
	int ecCodewords = 0;
	QRECB *ecbArray = NULL;
	QRECB *ecBlock;
	int ecbArraySize = 0;
	int ret = 0;

    pVersion->versionNumber_ = versionNumber;
	pVersion->alignmentPatternCentersSize_ = alignmentPatternCentersSize;
	if(alignmentPatternCentersSize > 0)
	{
		MALLOC(pVersion->alignmentPatternCenters_,int,alignmentPatternCentersSize);
		diy_memcpy(pVersion->alignmentPatternCenters_,alignmentPatternCenters,sizeof(int)*alignmentPatternCentersSize);
	}
	pVersion->ecBlocksSize_ = 4;
	pVersion->totalCodewords_ = 0;
	MALLOC(pVersion->ecBlocks_,QRECBlocks,pVersion->ecBlocksSize_);

	diy_memcpy(&pVersion->ecBlocks_[0],ecBlocks1,sizeof(QRECBlocks));
	diy_memcpy(&pVersion->ecBlocks_[1],ecBlocks2,sizeof(QRECBlocks));
	diy_memcpy(&pVersion->ecBlocks_[2],ecBlocks3,sizeof(QRECBlocks));
	diy_memcpy(&pVersion->ecBlocks_[3],ecBlocks4,sizeof(QRECBlocks));

	ecCodewords = QRECBlockGetECCodewords(ecBlocks1);
	MALLOC(ecbArray,QRECB,2);
	QRECBlocksGetECBlocks(ecbArray,&ecbArraySize,ecBlocks1);

	for (i=0;i<ecbArraySize;i++)
	{
        ecBlock = &ecbArray[i];
		total += ecBlock->count_ * (ecBlock->dataCodewords_ + ecCodewords);
	}
	pVersion->totalCodewords_ = total;

ERROR_EXIT:
	FREE(ecbArray);
	return ret;
}

void QRVersionUninit(QRVersion *pVersion)
{
    if(pVersion)
	{
		if(pVersion->alignmentPatternCenters_)
			FREE(pVersion->alignmentPatternCenters_);
        
		if(pVersion->ecBlocks_)
            FREE(pVersion->ecBlocks_);
	}
}

int QRVersionBuildVersions(QRVersion *pVersionGroup)
{
    QRECB ECB1,ECB2,ECB3,ECB4,ECB5,ECB6,ECB7,ECB8;
	QRECBlocks ECBlocks1,ECBlocks2,ECBlocks3,ECBlocks4;
	int alignmentPatternCenters[8]={0};
    int ret = 0;

	//1
    QRECBInit(&ECB1,1,19);
	QRECBInit(&ECB2,1,16);
	QRECBInit(&ECB3,1,13);
    QRECBInit(&ECB4,1,9);
	QRECBlocksInit(&ECBlocks1,7,&ECB1);
	QRECBlocksInit(&ECBlocks2,10,&ECB2);
	QRECBlocksInit(&ECBlocks3,13,&ECB3);
	QRECBlocksInit(&ECBlocks4,17,&ECB4);
	ret = QRVersionInit(&pVersionGroup[0],1,&alignmentPatternCenters[0],0,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
    if(ret != 0) return ret;

	//2
	alignmentPatternCenters[0] = 6;
	alignmentPatternCenters[1] = 18;
    QRECBInit(&ECB1,1,34);
	QRECBInit(&ECB2,1,28);
	QRECBInit(&ECB3,1,22);
    QRECBInit(&ECB4,1,16);
	QRECBlocksInit(&ECBlocks1,10,&ECB1);
	QRECBlocksInit(&ECBlocks2,16,&ECB2);
	QRECBlocksInit(&ECBlocks3,22,&ECB3);
	QRECBlocksInit(&ECBlocks4,28,&ECB4);
	ret = QRVersionInit(&pVersionGroup[1],2,&alignmentPatternCenters[0],2,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
	if(ret != 0) return ret;
 
    //3
	alignmentPatternCenters[0] = 6;
	alignmentPatternCenters[1] = 22;
    QRECBInit(&ECB1,1,55);
	QRECBInit(&ECB2,1,44);
	QRECBInit(&ECB3,2,17);
    QRECBInit(&ECB4,2,13);
	QRECBlocksInit(&ECBlocks1,15,&ECB1);
	QRECBlocksInit(&ECBlocks2,26,&ECB2);
	QRECBlocksInit(&ECBlocks3,18,&ECB3);
	QRECBlocksInit(&ECBlocks4,22,&ECB4);
	ret = QRVersionInit(&pVersionGroup[2],3,&alignmentPatternCenters[0],2,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
	if(ret != 0) return ret;

    //4
	alignmentPatternCenters[0] = 6;
	alignmentPatternCenters[1] = 26;
    QRECBInit(&ECB1,1,80);
	QRECBInit(&ECB2,2,32);
	QRECBInit(&ECB3,2,24);
    QRECBInit(&ECB4,4,9);
	QRECBlocksInit(&ECBlocks1,20,&ECB1);
	QRECBlocksInit(&ECBlocks2,18,&ECB2);
	QRECBlocksInit(&ECBlocks3,26,&ECB3);
	QRECBlocksInit(&ECBlocks4,16,&ECB4);
	ret = QRVersionInit(&pVersionGroup[3],4,&alignmentPatternCenters[0],2,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
	if(ret != 0) return ret;

    //5
	alignmentPatternCenters[0] = 6;
	alignmentPatternCenters[1] = 30;
    QRECBInit(&ECB1,1,108);
	QRECBInit(&ECB2,2,43);
	QRECBInit(&ECB3,2,15);
    QRECBInit(&ECB4,2,16);
	QRECBInit(&ECB5,2,11);
    QRECBInit(&ECB6,2,12);
	QRECBlocksInit(&ECBlocks1,26,&ECB1);
	QRECBlocksInit(&ECBlocks2,24,&ECB2);
	QRECBlocksInitA(&ECBlocks3,18,&ECB3,&ECB4);
	QRECBlocksInitA(&ECBlocks4,22,&ECB5,&ECB6);
	ret = QRVersionInit(&pVersionGroup[4],5,&alignmentPatternCenters[0],2,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
	if(ret != 0) return ret;

	//6
	alignmentPatternCenters[0] = 6;
	alignmentPatternCenters[1] = 34;
    QRECBInit(&ECB1,2,68);
	QRECBInit(&ECB2,4,27);
	QRECBInit(&ECB3,4,19);
    QRECBInit(&ECB4,4,15);
	QRECBlocksInit(&ECBlocks1,18,&ECB1);
	QRECBlocksInit(&ECBlocks2,16,&ECB2);
	QRECBlocksInit(&ECBlocks3,24,&ECB3);
	QRECBlocksInit(&ECBlocks4,28,&ECB5);
	ret = QRVersionInit(&pVersionGroup[5],6,&alignmentPatternCenters[0],2,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
	if(ret != 0) return ret;

    //7
	alignmentPatternCenters[0] = 6;
	alignmentPatternCenters[1] = 22;
	alignmentPatternCenters[2] = 38;
    QRECBInit(&ECB1,2,78);
	QRECBInit(&ECB2,4,31);
	QRECBInit(&ECB3,2,14);
    QRECBInit(&ECB4,4,15);
	QRECBInit(&ECB5,4,13);
    QRECBInit(&ECB6,1,14);
	QRECBlocksInit(&ECBlocks1,20,&ECB1);
	QRECBlocksInit(&ECBlocks2,18,&ECB2);
	QRECBlocksInitA(&ECBlocks3,18,&ECB3,&ECB4);
	QRECBlocksInitA(&ECBlocks4,26,&ECB5,&ECB6);
	ret = QRVersionInit(&pVersionGroup[6],7,&alignmentPatternCenters[0],3,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
	if(ret != 0) return ret;

	//8
	alignmentPatternCenters[0] = 6;
	alignmentPatternCenters[1] = 24;
	alignmentPatternCenters[2] = 42;
    QRECBInit(&ECB1,2,97);
	QRECBInit(&ECB2,2,38);
	QRECBInit(&ECB3,2,39);
    QRECBInit(&ECB4,4,18);
	QRECBInit(&ECB5,2,19);
    QRECBInit(&ECB6,4,14);
    QRECBInit(&ECB7,2,15);
	QRECBlocksInit(&ECBlocks1,24,&ECB1);
	QRECBlocksInitA(&ECBlocks2,22,&ECB2,&ECB3);
	QRECBlocksInitA(&ECBlocks3,22,&ECB4,&ECB5);
	QRECBlocksInitA(&ECBlocks4,26,&ECB6,&ECB7);
	ret = QRVersionInit(&pVersionGroup[7],8,&alignmentPatternCenters[0],3,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
	if(ret != 0) return ret;

	//9
	alignmentPatternCenters[0] = 6;
	alignmentPatternCenters[1] = 26;
	alignmentPatternCenters[2] = 46;
    QRECBInit(&ECB1,2,116);
	QRECBInit(&ECB2,3,36);
	QRECBInit(&ECB3,2,37);
    QRECBInit(&ECB4,4,16);
	QRECBInit(&ECB5,4,17);
    QRECBInit(&ECB6,4,12);
    QRECBInit(&ECB7,4,13);
	QRECBlocksInit(&ECBlocks1,30,&ECB1);
	QRECBlocksInitA(&ECBlocks2,22,&ECB2,&ECB3);
	QRECBlocksInitA(&ECBlocks3,20,&ECB4,&ECB5);
	QRECBlocksInitA(&ECBlocks4,24,&ECB6,&ECB7);
	ret = QRVersionInit(&pVersionGroup[8],9,&alignmentPatternCenters[0],3,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
	if(ret != 0) return ret;

	//10
	alignmentPatternCenters[0] = 6;
	alignmentPatternCenters[1] = 28;
	alignmentPatternCenters[2] = 50;
    QRECBInit(&ECB1,2,68);
	QRECBInit(&ECB2,2,69);
	QRECBInit(&ECB3,4,43);
    QRECBInit(&ECB4,1,44);
	QRECBInit(&ECB5,6,19);
    QRECBInit(&ECB6,2,20);
    QRECBInit(&ECB7,6,15);
	QRECBInit(&ECB8,2,16);
	QRECBlocksInitA(&ECBlocks1,18,&ECB1,&ECB2);
	QRECBlocksInitA(&ECBlocks2,26,&ECB3,&ECB4);
	QRECBlocksInitA(&ECBlocks3,24,&ECB5,&ECB6);
	QRECBlocksInitA(&ECBlocks4,28,&ECB7,&ECB8);
	ret = QRVersionInit(&pVersionGroup[9],10,&alignmentPatternCenters[0],3,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
	if(ret != 0) return ret;

	//11
	alignmentPatternCenters[0] = 6;
	alignmentPatternCenters[1] = 30;
	alignmentPatternCenters[2] = 54;
    QRECBInit(&ECB1,4,81);
	QRECBInit(&ECB2,1,50);
	QRECBInit(&ECB3,4,51);
    QRECBInit(&ECB4,4,22);
	QRECBInit(&ECB5,4,23);
    QRECBInit(&ECB6,3,12);
    QRECBInit(&ECB7,8,13);
	QRECBlocksInit(&ECBlocks1,20,&ECB1);
	QRECBlocksInitA(&ECBlocks2,30,&ECB2,&ECB3);
	QRECBlocksInitA(&ECBlocks3,28,&ECB4,&ECB5);
	QRECBlocksInitA(&ECBlocks4,24,&ECB6,&ECB7);
	ret = QRVersionInit(&pVersionGroup[10],11,&alignmentPatternCenters[0],3,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
	if(ret != 0) return ret;

	//12
	alignmentPatternCenters[0] = 6;
	alignmentPatternCenters[1] = 32;
	alignmentPatternCenters[2] = 58;
    QRECBInit(&ECB1,2,92);
	QRECBInit(&ECB2,2,93);
	QRECBInit(&ECB3,6,36);
    QRECBInit(&ECB4,2,37);
	QRECBInit(&ECB5,4,20);
    QRECBInit(&ECB6,6,21);
    QRECBInit(&ECB7,7,14);
	QRECBInit(&ECB8,4,15);
	QRECBlocksInitA(&ECBlocks1,24,&ECB1,&ECB2);
	QRECBlocksInitA(&ECBlocks2,22,&ECB3,&ECB4);
	QRECBlocksInitA(&ECBlocks3,26,&ECB5,&ECB6);
	QRECBlocksInitA(&ECBlocks4,28,&ECB7,&ECB8);
	ret = QRVersionInit(&pVersionGroup[11],12,&alignmentPatternCenters[0],3,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
	if(ret != 0) return ret;

	//13
	alignmentPatternCenters[0] = 6;
	alignmentPatternCenters[1] = 34;
	alignmentPatternCenters[2] = 62;
    QRECBInit(&ECB1,4,107);
	QRECBInit(&ECB2,8,37);
	QRECBInit(&ECB3,1,38);
    QRECBInit(&ECB4,8,20);
	QRECBInit(&ECB5,4,21);
    QRECBInit(&ECB6,12,11);
    QRECBInit(&ECB7,4,12);
	QRECBlocksInit(&ECBlocks1,26,&ECB1);
	QRECBlocksInitA(&ECBlocks2,22,&ECB2,&ECB3);
	QRECBlocksInitA(&ECBlocks3,24,&ECB4,&ECB5);
	QRECBlocksInitA(&ECBlocks4,22,&ECB6,&ECB7);
	ret = QRVersionInit(&pVersionGroup[12],13,&alignmentPatternCenters[0],3,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
	if(ret != 0) return ret;

	//14
	alignmentPatternCenters[0] = 6;
	alignmentPatternCenters[1] = 26;
	alignmentPatternCenters[2] = 46;
	alignmentPatternCenters[3] = 66;
    QRECBInit(&ECB1,3,115);
	QRECBInit(&ECB2,1,116);
	QRECBInit(&ECB3,4,40);
    QRECBInit(&ECB4,5,41);
	QRECBInit(&ECB5,11,16);
    QRECBInit(&ECB6,5,17);
    QRECBInit(&ECB7,11,12);
	QRECBInit(&ECB8,5,13);
	QRECBlocksInitA(&ECBlocks1,30,&ECB1,&ECB2);
	QRECBlocksInitA(&ECBlocks2,24,&ECB3,&ECB4);
	QRECBlocksInitA(&ECBlocks3,20,&ECB5,&ECB6);
	QRECBlocksInitA(&ECBlocks4,24,&ECB7,&ECB8);
	ret = QRVersionInit(&pVersionGroup[13],14,&alignmentPatternCenters[0],4,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
	if(ret != 0) return ret;

    //15
	alignmentPatternCenters[1] = 0;
	alignmentPatternCenters[2] = 1;
	alignmentPatternCenters[3] = 2;
	alignmentPatternCenters[4] = 3;
    QRECBInit(&ECB1,5,87);
	QRECBInit(&ECB2,1,88);
	QRECBInit(&ECB3,5,41);
    QRECBInit(&ECB4,5,42);
	QRECBInit(&ECB5,5,24);
    QRECBInit(&ECB6,7,25);
    QRECBInit(&ECB7,11,12);
	QRECBInit(&ECB8,7,13);
	QRECBlocksInitA(&ECBlocks1,22,&ECB1,&ECB2);
	QRECBlocksInitA(&ECBlocks2,24,&ECB3,&ECB4);
	QRECBlocksInitA(&ECBlocks3,30,&ECB5,&ECB6);
	QRECBlocksInitA(&ECBlocks4,24,&ECB7,&ECB8);
	ret = QRVersionInit(&pVersionGroup[14],15,&alignmentPatternCenters[0],4,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
	if(ret != 0) return ret;

	//16
	alignmentPatternCenters[0] = 6;
	alignmentPatternCenters[1] = 26;
	alignmentPatternCenters[2] = 50;
	alignmentPatternCenters[3] = 74;
    QRECBInit(&ECB1,5,98);
	QRECBInit(&ECB2,1,99);
	QRECBInit(&ECB3,7,45);
    QRECBInit(&ECB4,3,46);
	QRECBInit(&ECB5,15,19);
    QRECBInit(&ECB6,2,20);
    QRECBInit(&ECB7,3,15);
	QRECBInit(&ECB8,13,16);
	QRECBlocksInitA(&ECBlocks1,24,&ECB1,&ECB2);
	QRECBlocksInitA(&ECBlocks2,28,&ECB3,&ECB4);
	QRECBlocksInitA(&ECBlocks3,24,&ECB5,&ECB6);
	QRECBlocksInitA(&ECBlocks4,30,&ECB7,&ECB8);
	ret = QRVersionInit(&pVersionGroup[15],16,&alignmentPatternCenters[0],4,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
	if(ret != 0) return ret;

	//17
	alignmentPatternCenters[0] = 6;
	alignmentPatternCenters[1] = 30;
	alignmentPatternCenters[2] = 54;
	alignmentPatternCenters[3] = 78;
    QRECBInit(&ECB1,1,107);
	QRECBInit(&ECB2,5,108);
	QRECBInit(&ECB3,10,46);
    QRECBInit(&ECB4,1,47);
	QRECBInit(&ECB5,1,22);
    QRECBInit(&ECB6,15,23);
    QRECBInit(&ECB7,2,14);
	QRECBInit(&ECB8,17,15);
	QRECBlocksInitA(&ECBlocks1,28,&ECB1,&ECB2);
	QRECBlocksInitA(&ECBlocks2,28,&ECB3,&ECB4);
	QRECBlocksInitA(&ECBlocks3,28,&ECB5,&ECB6);
	QRECBlocksInitA(&ECBlocks4,28,&ECB7,&ECB8);
	ret = QRVersionInit(&pVersionGroup[16],17,&alignmentPatternCenters[0],4,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
	if(ret != 0) return ret;

    //18
	alignmentPatternCenters[0] = 6;
	alignmentPatternCenters[1] = 30;
	alignmentPatternCenters[2] = 56;
	alignmentPatternCenters[3] = 82;
    QRECBInit(&ECB1,5,120);
	QRECBInit(&ECB2,1,121);
	QRECBInit(&ECB3,9,43);
    QRECBInit(&ECB4,4,44);
	QRECBInit(&ECB5,17,22);
    QRECBInit(&ECB6,1,23);
    QRECBInit(&ECB7,2,14);
	QRECBInit(&ECB8,19,15);
	QRECBlocksInitA(&ECBlocks1,30,&ECB1,&ECB2);
	QRECBlocksInitA(&ECBlocks2,26,&ECB3,&ECB4);
	QRECBlocksInitA(&ECBlocks3,28,&ECB5,&ECB6);
	QRECBlocksInitA(&ECBlocks4,28,&ECB7,&ECB8);
	ret = QRVersionInit(&pVersionGroup[17],18,&alignmentPatternCenters[0],4,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
	if(ret != 0) return ret;

	//19
	alignmentPatternCenters[0] = 6;
	alignmentPatternCenters[1] = 30;
	alignmentPatternCenters[2] = 58;
	alignmentPatternCenters[3] = 86;
    QRECBInit(&ECB1,3,113);
	QRECBInit(&ECB2,4,114);
	QRECBInit(&ECB3,3,44);
    QRECBInit(&ECB4,11,45);
	QRECBInit(&ECB5,17,21);
    QRECBInit(&ECB6,4,22);
    QRECBInit(&ECB7,9,13);
	QRECBInit(&ECB8,16,14);
	QRECBlocksInitA(&ECBlocks1,28,&ECB1,&ECB2);
	QRECBlocksInitA(&ECBlocks2,26,&ECB3,&ECB4);
	QRECBlocksInitA(&ECBlocks3,26,&ECB5,&ECB6);
	QRECBlocksInitA(&ECBlocks4,26,&ECB7,&ECB8);
	ret = QRVersionInit(&pVersionGroup[18],19,&alignmentPatternCenters[0],4,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
	if(ret != 0) return ret;

	//20
	alignmentPatternCenters[0] = 6;
	alignmentPatternCenters[1] = 34;
	alignmentPatternCenters[2] = 62;
	alignmentPatternCenters[3] = 90;
    QRECBInit(&ECB1,3,107);
	QRECBInit(&ECB2,5,108);
	QRECBInit(&ECB3,3,41);
    QRECBInit(&ECB4,13,42);
	QRECBInit(&ECB5,15,24);
    QRECBInit(&ECB6,5,25);
    QRECBInit(&ECB7,15,15);
	QRECBInit(&ECB8,10,16);
	QRECBlocksInitA(&ECBlocks1,28,&ECB1,&ECB2);
	QRECBlocksInitA(&ECBlocks2,26,&ECB3,&ECB4);
	QRECBlocksInitA(&ECBlocks3,30,&ECB5,&ECB6);
	QRECBlocksInitA(&ECBlocks4,28,&ECB7,&ECB8);
	ret = QRVersionInit(&pVersionGroup[19],20,&alignmentPatternCenters[0],4,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
	if(ret != 0) return ret;

	//21
	alignmentPatternCenters[0] = 6;
	alignmentPatternCenters[1] = 28;
	alignmentPatternCenters[2] = 50;
	alignmentPatternCenters[3] = 72;
	alignmentPatternCenters[4] = 94;
    QRECBInit(&ECB1,4,116);
	QRECBInit(&ECB2,4,117);
	QRECBInit(&ECB3,17,42);
    QRECBInit(&ECB4,17,22);
	QRECBInit(&ECB5,6,23);
    QRECBInit(&ECB6,19,16);
    QRECBInit(&ECB7,6,17);
	QRECBlocksInitA(&ECBlocks1,28,&ECB1,&ECB2);
	QRECBlocksInit(&ECBlocks2,26,&ECB3);
	QRECBlocksInitA(&ECBlocks3,28,&ECB4,&ECB5);
	QRECBlocksInitA(&ECBlocks4,30,&ECB6,&ECB7);
	ret = QRVersionInit(&pVersionGroup[20],21,&alignmentPatternCenters[0],5,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
	if(ret != 0) return ret;

	//22
	alignmentPatternCenters[0] = 6;
	alignmentPatternCenters[1] = 26;
	alignmentPatternCenters[2] = 50;
	alignmentPatternCenters[3] = 74;
	alignmentPatternCenters[4] = 98;
    QRECBInit(&ECB1,2,111);
	QRECBInit(&ECB2,7,112);
	QRECBInit(&ECB3,17,46);
    QRECBInit(&ECB4,7,24);
	QRECBInit(&ECB5,16,25);
    QRECBInit(&ECB6,34,13);
	QRECBlocksInitA(&ECBlocks1,28,&ECB1,&ECB2);
	QRECBlocksInit(&ECBlocks2,28,&ECB3);
	QRECBlocksInitA(&ECBlocks3,30,&ECB4,&ECB5);
	QRECBlocksInit(&ECBlocks4,24,&ECB6);
	ret = QRVersionInit(&pVersionGroup[21],22,&alignmentPatternCenters[0],5,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
	if(ret != 0) return ret;

    //23
	alignmentPatternCenters[0] = 6;
	alignmentPatternCenters[1] = 30;
	alignmentPatternCenters[2] = 54;
	alignmentPatternCenters[3] = 78;
	alignmentPatternCenters[4] = 102;
    QRECBInit(&ECB1,4,121);
	QRECBInit(&ECB2,5,122);
	QRECBInit(&ECB3,4,47);
    QRECBInit(&ECB4,14,48);
	QRECBInit(&ECB5,11,24);
    QRECBInit(&ECB6,14,25);
    QRECBInit(&ECB7,16,15);
	QRECBInit(&ECB8,14,16);
	QRECBlocksInitA(&ECBlocks1,30,&ECB1,&ECB2);
	QRECBlocksInitA(&ECBlocks2,28,&ECB3,&ECB4);
	QRECBlocksInitA(&ECBlocks3,30,&ECB5,&ECB6);
	QRECBlocksInitA(&ECBlocks4,30,&ECB7,&ECB8);
	ret = QRVersionInit(&pVersionGroup[22],23,&alignmentPatternCenters[0],5,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
	if(ret != 0) return ret;

	//24
	alignmentPatternCenters[0] = 6;
	alignmentPatternCenters[1] = 28;
	alignmentPatternCenters[2] = 54;
	alignmentPatternCenters[3] = 80;
	alignmentPatternCenters[4] = 106;
    QRECBInit(&ECB1,6,117);
	QRECBInit(&ECB2,4,118);
	QRECBInit(&ECB3,6,45);
    QRECBInit(&ECB4,14,46);
	QRECBInit(&ECB5,11,24);
    QRECBInit(&ECB6,16,25);
    QRECBInit(&ECB7,30,16);
	QRECBInit(&ECB8,2,17);
	QRECBlocksInitA(&ECBlocks1,30,&ECB1,&ECB2);
	QRECBlocksInitA(&ECBlocks2,28,&ECB3,&ECB4);
	QRECBlocksInitA(&ECBlocks3,30,&ECB5,&ECB6);
	QRECBlocksInitA(&ECBlocks4,30,&ECB7,&ECB8);
	ret = QRVersionInit(&pVersionGroup[23],24,&alignmentPatternCenters[0],5,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
	if(ret != 0) return ret;

	//25
	alignmentPatternCenters[0] = 6;
	alignmentPatternCenters[1] = 32;
	alignmentPatternCenters[2] = 58;
	alignmentPatternCenters[3] = 84;
	alignmentPatternCenters[4] = 110;
    QRECBInit(&ECB1,8,106);
	QRECBInit(&ECB2,4,107);
	QRECBInit(&ECB3,8,47);
    QRECBInit(&ECB4,13,48);
	QRECBInit(&ECB5,7,24);
    QRECBInit(&ECB6,22,25);
    QRECBInit(&ECB7,22,15);
	QRECBInit(&ECB8,13,16);
	QRECBlocksInitA(&ECBlocks1,26,&ECB1,&ECB2);
	QRECBlocksInitA(&ECBlocks2,28,&ECB3,&ECB4);
	QRECBlocksInitA(&ECBlocks3,30,&ECB5,&ECB6);
	QRECBlocksInitA(&ECBlocks4,30,&ECB7,&ECB8);
	ret = QRVersionInit(&pVersionGroup[24],25,&alignmentPatternCenters[0],5,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
	if(ret != 0) return ret;

    //26
	alignmentPatternCenters[0] = 6;
	alignmentPatternCenters[1] = 30;
	alignmentPatternCenters[2] = 58;
	alignmentPatternCenters[3] = 86;
	alignmentPatternCenters[4] = 114;
    QRECBInit(&ECB1,10,114);
	QRECBInit(&ECB2,2,115);
	QRECBInit(&ECB3,19,46);
    QRECBInit(&ECB4,4,47);
	QRECBInit(&ECB5,28,22);
    QRECBInit(&ECB6,6,23);
    QRECBInit(&ECB7,33,16);
	QRECBInit(&ECB8,4,17);
	QRECBlocksInitA(&ECBlocks1,28,&ECB1,&ECB2);
	QRECBlocksInitA(&ECBlocks2,28,&ECB3,&ECB4);
	QRECBlocksInitA(&ECBlocks3,28,&ECB5,&ECB6);
	QRECBlocksInitA(&ECBlocks4,30,&ECB7,&ECB8);
	ret = QRVersionInit(&pVersionGroup[25],26,&alignmentPatternCenters[0],5,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
	if(ret != 0) return ret;

	//27
	alignmentPatternCenters[0] = 6;
	alignmentPatternCenters[1] = 34;
	alignmentPatternCenters[2] = 62;
	alignmentPatternCenters[3] = 90;
	alignmentPatternCenters[4] = 118;
    QRECBInit(&ECB1,8,122);
	QRECBInit(&ECB2,4,123);
	QRECBInit(&ECB3,22,45);
    QRECBInit(&ECB4,3,46);
	QRECBInit(&ECB5,8,23);
    QRECBInit(&ECB6,26,24);
    QRECBInit(&ECB7,12,15);
	QRECBInit(&ECB8,28,16);
	QRECBlocksInitA(&ECBlocks1,30,&ECB1,&ECB2);
	QRECBlocksInitA(&ECBlocks2,28,&ECB3,&ECB4);
	QRECBlocksInitA(&ECBlocks3,30,&ECB5,&ECB6);
	QRECBlocksInitA(&ECBlocks4,30,&ECB7,&ECB8);
	ret = QRVersionInit(&pVersionGroup[26],27,&alignmentPatternCenters[0],5,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
	if(ret != 0) return ret;

	//28
	alignmentPatternCenters[0] = 6;
	alignmentPatternCenters[1] = 26;
	alignmentPatternCenters[2] = 50;
	alignmentPatternCenters[3] = 74;
	alignmentPatternCenters[4] = 98;
	alignmentPatternCenters[5] = 122;
    QRECBInit(&ECB1,3,117);
	QRECBInit(&ECB2,10,118);
	QRECBInit(&ECB3,3,45);
    QRECBInit(&ECB4,23,46);
	QRECBInit(&ECB5,4,24);
    QRECBInit(&ECB6,31,25);
    QRECBInit(&ECB7,11,15);
	QRECBInit(&ECB8,31,16);
	QRECBlocksInitA(&ECBlocks1,30,&ECB1,&ECB2);
	QRECBlocksInitA(&ECBlocks2,28,&ECB3,&ECB4);
	QRECBlocksInitA(&ECBlocks3,30,&ECB5,&ECB6);
	QRECBlocksInitA(&ECBlocks4,30,&ECB7,&ECB8);
	ret = QRVersionInit(&pVersionGroup[27],28,&alignmentPatternCenters[0],6,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
	if(ret != 0) return ret;

	//29
	alignmentPatternCenters[0] = 6;
	alignmentPatternCenters[1] = 30;
	alignmentPatternCenters[2] = 54;
	alignmentPatternCenters[3] = 78;
	alignmentPatternCenters[4] = 102;
	alignmentPatternCenters[5] = 126;
    QRECBInit(&ECB1,7,116);
	QRECBInit(&ECB2,7,117);
	QRECBInit(&ECB3,21,45);
    QRECBInit(&ECB4,7,46);
	QRECBInit(&ECB5,1,23);
    QRECBInit(&ECB6,37,24);
    QRECBInit(&ECB7,19,15);
	QRECBInit(&ECB8,26,16);
	QRECBlocksInitA(&ECBlocks1,30,&ECB1,&ECB2);
	QRECBlocksInitA(&ECBlocks2,28,&ECB3,&ECB4);
	QRECBlocksInitA(&ECBlocks3,30,&ECB5,&ECB6);
	QRECBlocksInitA(&ECBlocks4,30,&ECB7,&ECB8);
	ret = QRVersionInit(&pVersionGroup[28],29,&alignmentPatternCenters[0],6,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
	if(ret != 0) return ret;

    //30
	alignmentPatternCenters[0] = 6;
	alignmentPatternCenters[1] = 26;
	alignmentPatternCenters[2] = 52;
	alignmentPatternCenters[3] = 78;
	alignmentPatternCenters[4] = 104;
	alignmentPatternCenters[5] = 130;
    QRECBInit(&ECB1,5,115);
	QRECBInit(&ECB2,10,116);
	QRECBInit(&ECB3,19,47);
    QRECBInit(&ECB4,10,48);
	QRECBInit(&ECB5,15,24);
    QRECBInit(&ECB6,25,25);
    QRECBInit(&ECB7,23,15);
	QRECBInit(&ECB8,25,16);
	QRECBlocksInitA(&ECBlocks1,30,&ECB1,&ECB2);
	QRECBlocksInitA(&ECBlocks2,28,&ECB3,&ECB4);
	QRECBlocksInitA(&ECBlocks3,30,&ECB5,&ECB6);
	QRECBlocksInitA(&ECBlocks4,30,&ECB7,&ECB8);
	ret = QRVersionInit(&pVersionGroup[29],30,&alignmentPatternCenters[0],6,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
	if(ret != 0) return ret;

	//31
	alignmentPatternCenters[0] = 6;
	alignmentPatternCenters[1] = 30;
	alignmentPatternCenters[2] = 56;
	alignmentPatternCenters[3] = 82;
	alignmentPatternCenters[4] = 108;
	alignmentPatternCenters[5] = 134;
    QRECBInit(&ECB1,13,115);
	QRECBInit(&ECB2,3,116);
	QRECBInit(&ECB3,2,46);
    QRECBInit(&ECB4,29,47);
	QRECBInit(&ECB5,42,24);
    QRECBInit(&ECB6,1,25);
    QRECBInit(&ECB7,23,15);
	QRECBInit(&ECB8,28,16);
	QRECBlocksInitA(&ECBlocks1,30,&ECB1,&ECB2);
	QRECBlocksInitA(&ECBlocks2,28,&ECB3,&ECB4);
	QRECBlocksInitA(&ECBlocks3,30,&ECB5,&ECB6);
	QRECBlocksInitA(&ECBlocks4,30,&ECB7,&ECB8);
	ret = QRVersionInit(&pVersionGroup[30],31,&alignmentPatternCenters[0],6,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
	if(ret != 0) return ret;

	//32
	alignmentPatternCenters[0] = 6;
	alignmentPatternCenters[1] = 34;
	alignmentPatternCenters[2] = 60;
	alignmentPatternCenters[3] = 86;
	alignmentPatternCenters[4] = 112;
	alignmentPatternCenters[5] = 138;
    QRECBInit(&ECB1,17,115);
	QRECBInit(&ECB2,10,46);
	QRECBInit(&ECB3,23,47);
    QRECBInit(&ECB4,10,24);
	QRECBInit(&ECB5,35,25);
    QRECBInit(&ECB6,19,15);
    QRECBInit(&ECB7,35,16);
	QRECBlocksInit(&ECBlocks1,30,&ECB1);
	QRECBlocksInitA(&ECBlocks2,28,&ECB2,&ECB3);
	QRECBlocksInitA(&ECBlocks3,30,&ECB4,&ECB5);
	QRECBlocksInitA(&ECBlocks4,30,&ECB6,&ECB7);
	ret = QRVersionInit(&pVersionGroup[31],32,&alignmentPatternCenters[0],6,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
	if(ret != 0) return ret;

	//33
	alignmentPatternCenters[0] = 6;
	alignmentPatternCenters[1] = 30;
	alignmentPatternCenters[2] = 58;
	alignmentPatternCenters[3] = 86;
	alignmentPatternCenters[4] = 114;
	alignmentPatternCenters[5] = 142;
    QRECBInit(&ECB1,17,115);
	QRECBInit(&ECB2,1,116);
	QRECBInit(&ECB3,14,46);
    QRECBInit(&ECB4,21,47);
	QRECBInit(&ECB5,29,24);
    QRECBInit(&ECB6,19,25);
    QRECBInit(&ECB7,11,15);
	QRECBInit(&ECB8,46,16);
	QRECBlocksInitA(&ECBlocks1,30,&ECB1,&ECB2);
	QRECBlocksInitA(&ECBlocks2,28,&ECB3,&ECB4);
	QRECBlocksInitA(&ECBlocks3,30,&ECB5,&ECB6);
	QRECBlocksInitA(&ECBlocks4,30,&ECB7,&ECB8);
	ret = QRVersionInit(&pVersionGroup[32],33,&alignmentPatternCenters[0],6,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
	if(ret != 0) return ret;

	//34
	alignmentPatternCenters[0] = 6;
	alignmentPatternCenters[1] = 34;
	alignmentPatternCenters[2] = 62;
	alignmentPatternCenters[3] = 90;
	alignmentPatternCenters[4] = 118;
	alignmentPatternCenters[5] = 146;
    QRECBInit(&ECB1,13,115);
	QRECBInit(&ECB2,6,116);
	QRECBInit(&ECB3,14,46);
    QRECBInit(&ECB4,23,47);
	QRECBInit(&ECB5,44,24);
    QRECBInit(&ECB6,7,25);
    QRECBInit(&ECB7,59,16);
	QRECBInit(&ECB8,1,17);
	QRECBlocksInitA(&ECBlocks1,30,&ECB1,&ECB2);
	QRECBlocksInitA(&ECBlocks2,28,&ECB3,&ECB4);
	QRECBlocksInitA(&ECBlocks3,30,&ECB5,&ECB6);
	QRECBlocksInitA(&ECBlocks4,30,&ECB7,&ECB8);
	ret = QRVersionInit(&pVersionGroup[33],34,&alignmentPatternCenters[0],6,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
	if(ret != 0) return ret;

	//35
	alignmentPatternCenters[0] = 6;
	alignmentPatternCenters[1] = 30;
	alignmentPatternCenters[2] = 54;
	alignmentPatternCenters[3] = 78;
	alignmentPatternCenters[4] = 102;
	alignmentPatternCenters[5] = 126;
	alignmentPatternCenters[6] = 150;
    QRECBInit(&ECB1,12,121);
	QRECBInit(&ECB2,7,122);
	QRECBInit(&ECB3,12,47);
    QRECBInit(&ECB4,26,48);
	QRECBInit(&ECB5,39,24);
    QRECBInit(&ECB6,14,25);
    QRECBInit(&ECB7,22,15);
	QRECBInit(&ECB8,41,16);
	QRECBlocksInitA(&ECBlocks1,30,&ECB1,&ECB2);
	QRECBlocksInitA(&ECBlocks2,28,&ECB3,&ECB4);
	QRECBlocksInitA(&ECBlocks3,30,&ECB5,&ECB6);
	QRECBlocksInitA(&ECBlocks4,30,&ECB7,&ECB8);
	ret = QRVersionInit(&pVersionGroup[34],35,&alignmentPatternCenters[0],7,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
	if(ret != 0) return ret;

	//36
	alignmentPatternCenters[0] = 6;
	alignmentPatternCenters[1] = 24;
	alignmentPatternCenters[2] = 50;
	alignmentPatternCenters[3] = 76;
	alignmentPatternCenters[4] = 102;
	alignmentPatternCenters[5] = 128;
	alignmentPatternCenters[6] = 154;
    QRECBInit(&ECB1,6,121);
	QRECBInit(&ECB2,14,122);
	QRECBInit(&ECB3,6,47);
    QRECBInit(&ECB4,34,48);
	QRECBInit(&ECB5,46,24);
    QRECBInit(&ECB6,10,25);
    QRECBInit(&ECB7,2,15);
	QRECBInit(&ECB8,64,16);
	QRECBlocksInitA(&ECBlocks1,30,&ECB1,&ECB2);
	QRECBlocksInitA(&ECBlocks2,28,&ECB3,&ECB4);
	QRECBlocksInitA(&ECBlocks3,30,&ECB5,&ECB6);
	QRECBlocksInitA(&ECBlocks4,30,&ECB7,&ECB8);
	ret = QRVersionInit(&pVersionGroup[35],36,&alignmentPatternCenters[0],7,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
	if(ret != 0) return ret;

	//37
	alignmentPatternCenters[0] = 6;
	alignmentPatternCenters[1] = 28;
	alignmentPatternCenters[2] = 54;
	alignmentPatternCenters[3] = 80;
	alignmentPatternCenters[4] = 106;
	alignmentPatternCenters[5] = 132;
	alignmentPatternCenters[6] = 158;
    QRECBInit(&ECB1,17,122);
	QRECBInit(&ECB2,4,123);
	QRECBInit(&ECB3,29,46);
    QRECBInit(&ECB4,14,47);
	QRECBInit(&ECB5,49,24);
    QRECBInit(&ECB6,10,25);
    QRECBInit(&ECB7,24,15);
	QRECBInit(&ECB8,46,16);
	QRECBlocksInitA(&ECBlocks1,30,&ECB1,&ECB2);
	QRECBlocksInitA(&ECBlocks2,28,&ECB3,&ECB4);
	QRECBlocksInitA(&ECBlocks3,30,&ECB5,&ECB6);
	QRECBlocksInitA(&ECBlocks4,30,&ECB7,&ECB8);
	ret = QRVersionInit(&pVersionGroup[36],37,&alignmentPatternCenters[0],7,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
	if(ret != 0) return ret;

	//38
	alignmentPatternCenters[0] = 6;
	alignmentPatternCenters[1] = 32;
	alignmentPatternCenters[2] = 58;
	alignmentPatternCenters[3] = 84;
	alignmentPatternCenters[4] = 110;
	alignmentPatternCenters[5] = 136;
	alignmentPatternCenters[6] = 162;
    QRECBInit(&ECB1,4,122);
	QRECBInit(&ECB2,18,123);
	QRECBInit(&ECB3,13,46);
    QRECBInit(&ECB4,32,47);
	QRECBInit(&ECB5,48,24);
    QRECBInit(&ECB6,14,25);
    QRECBInit(&ECB7,42,15);
	QRECBInit(&ECB8,32,16);
	QRECBlocksInitA(&ECBlocks1,30,&ECB1,&ECB2);
	QRECBlocksInitA(&ECBlocks2,28,&ECB3,&ECB4);
	QRECBlocksInitA(&ECBlocks3,30,&ECB5,&ECB6);
	QRECBlocksInitA(&ECBlocks4,30,&ECB7,&ECB8);
	ret = QRVersionInit(&pVersionGroup[37],38,&alignmentPatternCenters[0],7,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
	if(ret != 0) return ret;

	//39
	alignmentPatternCenters[0] = 6;
	alignmentPatternCenters[1] = 26;
	alignmentPatternCenters[2] = 54;
	alignmentPatternCenters[3] = 82;
	alignmentPatternCenters[4] = 110;
	alignmentPatternCenters[5] = 138;
	alignmentPatternCenters[6] = 166;
    QRECBInit(&ECB1,20,117);
	QRECBInit(&ECB2,4,118);
	QRECBInit(&ECB3,40,47);
    QRECBInit(&ECB4,7,48);
	QRECBInit(&ECB5,43,24);
    QRECBInit(&ECB6,22,25);
    QRECBInit(&ECB7,10,15);
	QRECBInit(&ECB8,67,16);
	QRECBlocksInitA(&ECBlocks1,30,&ECB1,&ECB2);
	QRECBlocksInitA(&ECBlocks2,28,&ECB3,&ECB4);
	QRECBlocksInitA(&ECBlocks3,30,&ECB5,&ECB6);
	QRECBlocksInitA(&ECBlocks4,30,&ECB7,&ECB8);
	ret = QRVersionInit(&pVersionGroup[38],39,&alignmentPatternCenters[0],7,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);
	if(ret != 0) return ret;

	//40
	alignmentPatternCenters[0] = 6;
	alignmentPatternCenters[1] = 30;
	alignmentPatternCenters[2] = 58;
	alignmentPatternCenters[3] = 86;
	alignmentPatternCenters[4] = 114;
	alignmentPatternCenters[5] = 142;
	alignmentPatternCenters[6] = 170;
    QRECBInit(&ECB1,19,118);
	QRECBInit(&ECB2,6,119);
	QRECBInit(&ECB3,18,47);
    QRECBInit(&ECB4,31,48);
	QRECBInit(&ECB5,34,24);
    QRECBInit(&ECB6,34,25);
    QRECBInit(&ECB7,20,15);
	QRECBInit(&ECB8,61,16);
	QRECBlocksInitA(&ECBlocks1,30,&ECB1,&ECB2);
	QRECBlocksInitA(&ECBlocks2,28,&ECB3,&ECB4);
	QRECBlocksInitA(&ECBlocks3,30,&ECB5,&ECB6);
	QRECBlocksInitA(&ECBlocks4,30,&ECB7,&ECB8);
	ret = QRVersionInit(&pVersionGroup[39],40,&alignmentPatternCenters[0],7,&ECBlocks1,&ECBlocks2,&ECBlocks3,&ECBlocks4);


	return ret;
}

void QRVersionDestroyVersions(QRVersion *pVersionGroup)
{
	int i = 0;
	if(pVersionGroup)
	{
		for(i=0;i<40;i++)
            QRVersionUninit(&pVersionGroup[i]);
	}
}

QRVersion *QRVersionDecodeVersionInformation(QRVersion *pVersionGroup,unsigned int versionBits)
{
    int bestDifference = 2147483647,i,bitsDifference;
	unsigned int bestVersion = 0,targetVersion;

	for (i=0;i<N_VERSION_DECODE_INFOS;i++)
	{
		targetVersion = VERSION_DECODE_INFO[i];
		if(targetVersion == versionBits)
			return QRVersionGetVersionForNumber(pVersionGroup,i+7);

		bitsDifference = QRFormatInfoNumBitsDiffering(versionBits,targetVersion);
		if(bitsDifference < bestDifference){
			bestVersion = i+7;
			bestDifference = bitsDifference;
		}
	}

	if(bestDifference <= 3)
		return QRVersionGetVersionForNumber(pVersionGroup,bestVersion);

	return NULL;
}

int QRVersionBuildFunctionPattern(Bitmatrix **ppBitmatrix,QRVersion *pVersion)
{
	int dimension,ret = 0,i;
	Bitmatrix *functionPattern = NULL;
	unsigned int nmax,x,y;

	MALLOC(*ppBitmatrix,Bitmatrix,1);
	functionPattern = *ppBitmatrix;

	dimension = QRVersionGetDimensionForVersion(pVersion);

	ret = BitMatrixInit(functionPattern,dimension,dimension);
	if(ret != 0) {FREE(*ppBitmatrix);return ret;};

	ret = BitMatrixSetRegion(functionPattern,0,0,9,9);
	if(ret != 0) {BitMatrixUninit(functionPattern);FREE(*ppBitmatrix);return ret;}

	ret = BitMatrixSetRegion(functionPattern,dimension-8,0,8,9);
	if(ret != 0) {BitMatrixUninit(functionPattern);FREE(*ppBitmatrix);return ret;}

	ret = BitMatrixSetRegion(functionPattern,0,dimension-8,9,8);
	if(ret != 0) {BitMatrixUninit(functionPattern);FREE(*ppBitmatrix);return ret;}
    // Alignment patterns
	nmax = pVersion->alignmentPatternCentersSize_;
	for (x=0;x<nmax;x++)
	{
		i = pVersion->alignmentPatternCenters_[x] - 2;
		for (y=0;y<nmax;y++)
		{
			if((x == 0 && (y == 0 || y == nmax-1)) || (x == nmax-1 && y==0))
			{
				// No alignment patterns near the three finder patterns
				continue;
			}
            //functionPattern->setRegion(alignmentPatternCenters_[y] - 2, i, 5, 5);
			ret = BitMatrixSetRegion(functionPattern,pVersion->alignmentPatternCenters_[y] - 2, i, 5, 5);
			if(ret != 0) {BitMatrixUninit(functionPattern);FREE(*ppBitmatrix);return ret;}
		}
	}

	// Vertical timing pattern
	ret = BitMatrixSetRegion(functionPattern,6, 9, 1, dimension - 17);
	if(ret != 0) {BitMatrixUninit(functionPattern);FREE(*ppBitmatrix);return ret;}
	// Horizontal timing pattern
	ret = BitMatrixSetRegion(functionPattern,9, 6, dimension - 17, 1);
	if(ret != 0) {BitMatrixUninit(functionPattern);FREE(*ppBitmatrix);return ret;}
	
	if (pVersion->versionNumber_ > 6) {
		// Version info, top right
		ret = BitMatrixSetRegion(functionPattern,dimension - 11, 0, 3, 6);
		if(ret != 0) {BitMatrixUninit(functionPattern);FREE(*ppBitmatrix);return ret;}
		// Version info, bottom left
		ret = BitMatrixSetRegion(functionPattern,0, dimension - 11, 6, 3);
		if(ret != 0) {BitMatrixUninit(functionPattern);FREE(*ppBitmatrix);return ret;}
  }

ERROR_EXIT:
  return ret;
}

void QRVersionDestroyFunctionPattern(Bitmatrix *pBitmatrix)
{
	if(pBitmatrix)
	{
		BitMatrixUninit(pBitmatrix);
		FREE(pBitmatrix);
	}
}
