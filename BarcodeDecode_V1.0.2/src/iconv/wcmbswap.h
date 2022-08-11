/**
*  ¿í×Ö·ûÍ¬¶à×Ö½Ú×Ö·û»¥»»
**/
#ifndef __WCMBSWAP_H__
#define __WCMBSWAP_H__

#ifndef NULL
#ifdef  __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

typedef struct {
	unsigned short indx; /* index into big table */
	unsigned short used; /* bitmask of used entries */
} Summary16;
#define RET_ILUNI      -1
/* Return code if output buffer is too small. (xxx_wctomb, xxx_reset) */
#define RET_TOOSMALL   -2

#define RET_ABORT      -3

#define RET_SHIFT_ILSEQ(n)  (-1-2*(n))
/* Return code if invalid. (xxx_mbtowc) */
#define RET_ILSEQ           RET_SHIFT_ILSEQ(0)
/* Return code if only a shift sequence of n bytes was read. (xxx_mbtowc) */
#define RET_TOOFEW(n)       (-2-2*(n))
/* Retrieve the n from the encoded RET_... value. */
#define DECODE_SHIFT_ILSEQ(r)  ((unsigned int)(RET_SHIFT_ILSEQ(0) - (r)) / 2)
#define DECODE_TOOFEW(r)       ((unsigned int)(RET_TOOFEW(0) - (r)) / 2)

//º¯Êý
int ascii_mbtowc (unsigned long *pwc, const unsigned char *s, int n);
int ascii_wctomb (unsigned char *r, unsigned long wc, int n);

int gb2312_wctomb(unsigned char *r, unsigned long wc, int n);
int gb2312_mbtowc(unsigned long *pwc,const unsigned char *s,int n);

int euc_cn_mbtowc(unsigned long *pwc, const unsigned char *s, int n);
int euc_cn_wctomb(unsigned char *r, unsigned long wc, int n);

#endif