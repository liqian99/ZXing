#include "wcmbswap.h"

int ascii_mbtowc (unsigned long *pwc, const unsigned char *s, int n)
{
	unsigned char c = *s;
	if (c < 0x80) {
		*pwc = (unsigned long) c;
		return 1;
	}
	return RET_ILSEQ;
}

int ascii_wctomb (unsigned char *r, unsigned long wc, int n)
{
	if (wc < 0x0080) {
		*r = (unsigned char)wc;
		return 1;
	}
	return RET_ILUNI;
}