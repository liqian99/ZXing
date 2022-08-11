#include "wcmbswap.h"

/**
    EUC-CN��GB2312��õı�ʾ�����������������ϵġ�GB2312����ͨ������ָ��EUC-CN����ʾ���� 
����GB 2312��Ԫʹ�������ֽ�����ʾ��
��������һλ�ֽڡ�ʹ��0xA1-0xFE 
�������ڶ�λ�ֽڡ�ʹ��0xA1-0xFE 
����������˵������������GB 2312֮�еĵ�һ�����֣�������λ����1601�� 
������EUC-CN֮�У�����0xA0+16=0xB0,0xA0+1=0xA1���ó�0xB0A1��
**/

int euc_cn_mbtowc(unsigned long *pwc, const unsigned char *s, int n)
{
	unsigned char c = *s;
	/* Code set 0 (ASCII or GB 1988-89) */
	if (c < 0x80)
		return ascii_mbtowc(pwc,s,n);
	/* Code set 1 (GB 2312-1980) */
	if (c >= 0xa1 && c < 0xff) {
		if (n < 2)
			return RET_TOOFEW(0);
		{
			unsigned char c2 = s[1];
			if (c2 >= 0xa1 && c2 < 0xff) {
				unsigned char buf[2];
				buf[0] = c-0x80; buf[1] = c2-0x80;
				return gb2312_mbtowc(pwc,buf,2);
			} else
				return RET_ILSEQ;
		}
	}
	return RET_ILSEQ;
}

int euc_cn_wctomb(unsigned char *r, unsigned long wc, int n)
{
	unsigned char buf[2];
	int ret;
	
	/* Code set 0 (ASCII or GB 1988-89) */
	ret = ascii_wctomb(r,wc,n);
	if (ret != RET_ILUNI)
		return ret;
	
	/* Code set 1 (GB 2312-1980) */
	ret = gb2312_wctomb(buf,wc,2);
	if (ret != RET_ILUNI) {
		if (ret != 2) return RET_ABORT;
		if (n < 2)
			return RET_TOOSMALL;
		r[0] = buf[0]+0x80;
		r[1] = buf[1]+0x80;
		return 2;
	}
	
	return RET_ILUNI;
}
