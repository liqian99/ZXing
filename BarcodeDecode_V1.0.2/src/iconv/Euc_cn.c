#include "wcmbswap.h"

/**
    EUC-CN是GB2312最常用的表示方法。浏览器编码表上的“GB2312”，通常都是指“EUC-CN”表示法。 
　　GB 2312字元使用两个字节来表示。
　　“第一位字节”使用0xA1-0xFE 
　　“第二位字节”使用0xA1-0xFE 
　　举例来说，“啊”字是GB 2312之中的第一个汉字，它的区位码是1601。 
　　在EUC-CN之中，它把0xA0+16=0xB0,0xA0+1=0xA1，得出0xB0A1。
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
