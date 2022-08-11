#ifndef __UTFSWAP_H__
#define __UTFSWAP_H__

/**
*  ����: �� UTF-16 ����ת�� UTF-8 ����
*  ����: inb[in]       ----utf-16�ִ�
*        inlenb[in]    ----utf-16�ִ�����(�ֽ���)
*        out[out]      ----utf-8�ִ�,����ǰ����ռ䡣
*        outlen[out]   ----utf-8�ִ�����,��Ϊ��������ʱΪ��ǰ����ռ�Ĵ�С��
*  �� ��: >= 0     ----ת����utf-8�ִ�����
*          -1       ----ת������
**/
int UTF16ToUTF8(const unsigned char *inb, int inlenb,unsigned char *out, int *outlen);

/**
*  ����: �� UTF-8 ����ת�� UTF-16 ����
*  ����: inb[in]       ----utf-8�ִ�
*        inlenb[in]    ----utf-8�ִ�����(�ֽ���)
*        out[out]      ----utf-16�ִ�,����ǰ����ռ䡣
*        outlen[out]   ----utf-16�ִ�����,��Ϊ��������ʱΪ��ǰ����ռ�Ĵ�С��
*  �� ��: >= 0     ----ת����utf-16�ִ�����
*          -1       ----ת������
**/
int UTF8ToUTF16(const unsigned char *in, int inlen,unsigned char *outb, int *outlen);

/**
*  ����: �� UTF-16 ����ת�� GB2312 
*  ����: inb[in]       ----utf-16�ִ�
*        inlenb[in]    ----utf-16�ִ�����(�ֽ���)
*        out[out]      ----GB2312�ִ�,����ǰ����ռ䡣
*        outlen[out]   ----GB2312�ִ�����,��Ϊ��������ʱΪ��ǰ����ռ�Ĵ�С��
*  �� ��: >= 0     ----ת����GB2312�ִ�����
*          -1       ----ת������
**/
int UTF16ToGB2312(const unsigned char *in, int inlen,unsigned char *out, int *outlen);

/**
*  ����: �� GB2312 ����ת�� UTF16 
*  ����: inb[in]       ----GB2312�ִ�
*        inlenb[in]    ----GB2312�ִ�����(�ֽ���)
*        out[out]      ----UTF16�ִ�,����ǰ����ռ䡣
*        outlen[out]   ----UTF16�ִ�����,��Ϊ��������ʱΪ��ǰ����ռ�Ĵ�С��
*  �� ��: >= 0     ----ת����UTF16�ִ�����
*          -1       ----ת������
**/
int GB2312ToUTF16(const unsigned char *in, int inlen,unsigned char *out, int *outlen);


#endif