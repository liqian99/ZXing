#include "gb2312.h"


int gb2312_wctomb(unsigned char *r, unsigned long wc, int n)
{
	if (n >= 2) {
		const Summary16 *summary = NULL;
		if (wc >= 0x0000 && wc < 0x0460)
			summary = &gb2312_uni2indx_page00[(wc>>4)];
		else if (wc >= 0x2000 && wc < 0x2650)
			summary = &gb2312_uni2indx_page20[(wc>>4)-0x200];
		else if (wc >= 0x3000 && wc < 0x3230)
			summary = &gb2312_uni2indx_page30[(wc>>4)-0x300];
		else if (wc >= 0x4e00 && wc < 0x9cf0)
			summary = &gb2312_uni2indx_page4e[(wc>>4)-0x4e0];
		else if (wc >= 0x9e00 && wc < 0x9fb0)
			summary = &gb2312_uni2indx_page9e[(wc>>4)-0x9e0];
		else if (wc >= 0xff00 && wc < 0xfff0)
			summary = &gb2312_uni2indx_pageff[(wc>>4)-0xff0];
		if (summary) {
			unsigned short used = summary->used;
			unsigned int i = wc & 0x0f;
			if (used & ((unsigned short) 1 << i)) {
				unsigned short c;
				/* Keep in `used' only the bits 0..i-1. */
				used &= ((unsigned short) 1 << i) - 1;
				/* Add `summary->indx' and the number of bits set in `used'. */
				used = (used & 0x5555) + ((used & 0xaaaa) >> 1);
				used = (used & 0x3333) + ((used & 0xcccc) >> 2);
				used = (used & 0x0f0f) + ((used & 0xf0f0) >> 4);
				used = (used & 0x00ff) + (used >> 8);
				c = gb2312_2charset[summary->indx + used];
				r[0] = (c >> 8); r[1] = (c & 0xff);
				return 2;
			}
		}
		return RET_ILUNI;
	}
	return RET_TOOSMALL;
}



int gb2312_mbtowc(unsigned long *pwc,const unsigned char *s,int n)
{
	unsigned char c1 = s[0];
	if ((c1 >= 0x21 && c1 <= 0x29) || (c1 >= 0x30 && c1 <= 0x77)) {
		if (n >= 2) {
			unsigned char c2 = s[1];
			if (c2 >= 0x21 && c2 < 0x7f) {
				unsigned int i = 94 * (c1 - 0x21) + (c2 - 0x21);
				unsigned short wc = 0xfffd;
				if (i < 1410) {
					if (i < 831)
						wc = gb2312_2uni_page21[i];
				} else {
					if (i < 8178)
						wc = gb2312_2uni_page30[i-1410];
				}
				if (wc != 0xfffd) {
					*pwc = (unsigned long) wc;
					return 2;
				}
			}
			return RET_ILSEQ;
		}
		return RET_TOOFEW(0);
	}
	return RET_ILSEQ;
}
