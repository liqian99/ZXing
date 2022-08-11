#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include "..//src//HybridBinarizer.h"
#include "..//src//oneD//Code93Reader.h"
#include "..//src//QR//QRCodeReader.h"
#include "..//src//DiyMalloc.h"
#include "..//src//oneD//OneDReader.h"


#define USE_MODE_GRAY  1
#define USE_MODE_BW    2

#define	USE_MODE  USE_MODE_GRAY


int Muti_GrayImg_DecodeNew(unsigned char *p,int w, int h,Bitmatrix *pBitmatrix,unsigned char *result, int *result_len,int *BarcodeType,int inBarcodeType)
{
	int ret = -1;

	ret = HybridBinarizerGetBlackMatrix(p,w,h,pBitmatrix);
	if(ret >=0)
	{
		ret = QRCodeReaderDecodeBW(pBitmatrix,result,result_len);
		if(ret >= 0)
		{
			*BarcodeType = 1;
			return ret;
		}else
			return -1;
	}
	else
		return -1;
	
	
}

#define QR_DECODE  1

//QR
void DecodeTest()
{
	unsigned char *pImg = NULL,*pImgT = NULL;
    long w=320,h=240,bitCount=8,offset=1078;
    int ret;
    char result[2048] = {0};
	int result_len = 2048;
	int ntype = 0;
	int i = 0,j=0;
	unsigned int dwStart = 0;
	FILE *fp = NULL;
	Bitmatrix bitMatrix;

	memset(&bitMatrix,0,sizeof(Bitmatrix));


	pImg = pImgT = (unsigned char *)malloc(w*h+offset);
#ifdef QR_DECODE
	fp = fopen("qr.bmp","rb");
#else
	fp = fopen("code93.bmp","rb");
#endif
	if(fp)
	{
        printf("��ͼ���ļ��ɹ� w=%d  h=%d  bitCount=%d\n",w,h,bitCount);
		fread(pImg,1,w*h+offset,fp);
		fclose(fp);
		pImg += offset;
#if USE_MODE==USE_MODE_BW  // ��ֽ����ֵ��ͼ��		
		ret = QRCodeReaderBuildAllVariable(w,h,&bitMatrix);
		if(ret >= 0)
        {
			printf("��ʼ��ȫ�ֱ����ɹ�:\n");
			for(i=0;i<1;i++)
			{
				dwStart = GetTickCount();
				ret = HybridBinarizerGetBlackMatrix(pImg,w,h,&bitMatrix);
				if(ret >= 0)
				{
					ret = QRCodeReaderDecodeBW(&bitMatrix,result,&result_len);
					if(ret >= 0)
					{
						printf("����ɹ�: i=%d ret=%d len=%d %d  CostTime=%d\n",i,ret,result_len,strlen((char *)result),GetTickCount()-dwStart);
						printf("�������ݣ�%s\n",(char *)result);
						//                     fp = fopen("G:\\BarcodeImg\\out.txt","w");
						// 					for(j=0;j<result_len;j++)
						// 					{
						// 						printf("%d  %02x\n",j,result[j]);
						// 						fprintf(fp,"%02x,",result[j]);
						// 					}
						// 					fclose(fp);
					}else{
						printf("����ʧ��: i=%d ret=%d  CostTime=%d\n",i,ret,GetTickCount()-dwStart);
					}
				}else{
				    printf("��ֵ��ʧ�� ret=%d:\n",ret);
				}

				Sleep(10);

			}


		}else{
			printf("��ʼ��ȫ�ֱ���ʧ��: ret=%d\n",ret);
		}
		QRCodeReaderDestroyAllVariable(&bitMatrix);
#else
	    //ֱ�ӽ���Ҷ�ͼ��
        for(i=0;i<1;i++)
		{
			dwStart = GetTickCount();
#ifdef QR_DECODE
			ret = QRCodeReaderDecodeGray(pImg,w,h,(unsigned char *)result,&result_len);
#else
			ret = OneDReaderDoDecode(pImg,w,h,(unsigned char *)result,&result_len,&ntype);
#endif
			if(ret >= 0)
			{
				printf("����ɹ�: i=%d ret=%d len=%d %d  CostTime=%d\n",i,ret,result_len,strlen((char *)result),GetTickCount()-dwStart);
				printf("�������ݣ�%s\n",(char *)result);
			}else{
				printf("����ʧ��: i=%d ret=%d  CostTime=%d\n",i,ret,GetTickCount()-dwStart);
			}
	
			Sleep(10);
			
	}
#endif
	}else{
        printf("��ͼ���ļ�ʧ�� ret=%d\n",ret);
	}
    
	pImg = pImgT;
	free(pImg);
}



int main()
{
	DecodeTest();

	while(1);
}