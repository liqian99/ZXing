#include "QRCodeReader.h"
#include "../DiyMalloc.h"

//����Ҷ�ͼ��
int QRCodeReaderDecodeGray(unsigned char *pImgGray,int w,int h,unsigned char *pResult,int *pResultLen)
{
    Bitmatrix bitmatrix;
	QRDetector qrdetector;
	DecodeHints hints;
	DetectorResult *pdetectorresult = NULL;
	int ret = 0;
	
    //�������б���
	diy_memset(&bitmatrix,0,sizeof(Bitmatrix));
    ret = QRCodeReaderBuildAllVariable(w,h,&bitmatrix);
    if(ret != 0) goto ERROR_EXIT;

    //��ֵ��
    ret = HybridBinarizerGetBlackMatrix(pImgGray,w,h,&bitmatrix);
	if(ret!=0) goto ERROR_EXIT;
#ifdef IMAGE_HELPER
	init_image_helper();
	ClearImage();
	BWHighLowReverse((unsigned char *)bitmatrix.bits,w,h);
	DrawBWImage((unsigned char *)bitmatrix.bits,w,h);
	BWHighLowReverse((unsigned char *)bitmatrix.bits,w,h);
#endif
   
 
 	//����Ƿ���QR��ͼ
    diy_memset(&qrdetector,0,sizeof(QRDetector));
	qrdetector.image_ = &bitmatrix;

	diy_memset(&hints,0,sizeof(DecodeHints));
 	ret = QRDetectorDetect(&qrdetector,&hints,&pdetectorresult);
	if(ret==0)
	{
        ret = QRDecode(pdetectorresult->bits_,pResult,pResultLen);     
	}

ERROR_EXIT:
	DetectorResultUninit(pdetectorresult);
    QRCodeReaderDestroyAllVariable(&bitmatrix);
	return ret;
}



//�����ֵ��ͼ��
int QRCodeReaderDecodeBW(Bitmatrix *pBitmatrix,unsigned char *pResult,int *pResultLen)
{
	QRDetector qrdetector;
	DecodeHints hints;
	DetectorResult *pdetectorresult = NULL;
	int ret = 0;

	//����Ƿ���QR��ͼ
    diy_memset(&qrdetector,0,sizeof(QRDetector));
	qrdetector.image_ = pBitmatrix;
	
	diy_memset(&hints,0,sizeof(DecodeHints));
	ret = QRDetectorDetect(&qrdetector,&hints,&pdetectorresult);
	if(ret==0)
	{
        ret = QRDecode(pdetectorresult->bits_,pResult,pResultLen);     
	}
	
	
	DetectorResultUninit(pdetectorresult);
	return ret;
}


/**
*  ������Ҫ�����б���
**/
int QRCodeReaderBuildAllVariable(int nImgW,int nImgH,Bitmatrix *pBitmatrix)
{
	int ret = 0;
     
	//��ֵ��
	if(pBitmatrix)
	{
		diy_memset(pBitmatrix,0,sizeof(Bitmatrix));
		ret = BitMatrixInit(pBitmatrix,nImgW,nImgH);
		if(ret!=0) goto ERROR_EXIT;
	}

	//ȫ�ְ汾����
	if(g_pVersionGroup == NULL)
	{
		MALLOC(g_pVersionGroup,QRVersion,N_VERSIONS);
		ret = QRVersionBuildVersions(g_pVersionGroup);
		if(ret != 0) {goto ERROR_EXIT;}
	}

	ret = GenericGFBuildAll();
ERROR_EXIT:
	return ret;
}

/**
*  ���ٴ�����Ҫ�����б���
**/
void QRCodeReaderDestroyAllVariable(Bitmatrix *pBitmatrix)
{
	if(g_pVersionGroup)
	{
         QRVersionDestroyVersions(g_pVersionGroup);
		 FREE(g_pVersionGroup);
	}
	
	GenericGFDestroyAll();

	if(pBitmatrix)
	    BitMatrixUninit(pBitmatrix);
}