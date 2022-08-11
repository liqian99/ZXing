#include "utfswap.h"
#include "wcmbswap.h"
/*
UCS-2编码    UTF-8 字节流(二进制) 
0000 - 007F  0xxxxxxx
0080 - 07FF 110xxxxx 10xxxxxx
0800 - FFFF 1110xxxx 10xxxxxx 10xxxxxx
*/

/**
* 0     ----Little Endian UTF-16 串(低字节在前)
* != 0  ----Big    Endian UTF-16 串(高字节在前)
**/
static int isLittleEndian =  0;

/**
*  功  能: 设置 UTF-16 编码方式
*  输  入: 
*        0     ----Little Endian UTF-16 串(低字节在前)
*        1     ----Big    Endian UTF-16 串(高字节在前)
*  输  出:
*  返  回:
**/
int SetEndian()
{
	unsigned char   *p = NULL;
	unsigned short  tst = 0x1234;
	
	/* 获取CPU字节码顺序 */
	p = (unsigned char *)&tst;
	
	if     (*p == 0x12) isLittleEndian = 1;
	else if(*p == 0x34) isLittleEndian = 0;
	else
	{
		// 		fprintf(stderr, "Unknown CPU Bytes Order!\n");
		return -1;
	}
	
	return 0;
}

int UTF16ToUTF8(const unsigned char *inb, int inlenb,unsigned char *out, int *outlen)
{
	unsigned char* outstart = out;
	const unsigned char* processed = inb;
	unsigned char* outend = out + *outlen;
	unsigned short* in = (unsigned short*)inb;
	unsigned short* inend;
	unsigned int c, d, inlen;
	unsigned char *tmp;
	int bits;

	if((inlenb % 2) == 1) (inlenb)--;
	inlen = inlenb / 2;
	inend = in + inlen;
	while((in < inend) && (out - outstart + 5 < *outlen))
	{
		if(isLittleEndian)
		{
			c= *in++;
		}
		else
		{
			tmp = (unsigned char *) in;
			c   = *tmp++;
			c   = c | (((unsigned int)*tmp) << 8);
			in++;
		}
		if((c & 0xFC00) == 0xD800)
		{
			if(in >= inend) break;
			if(isLittleEndian) { d = *in++; } 
			else 
			{
				tmp = (unsigned char *) in;
				d   = *tmp++;
				d   = d | (((unsigned int)*tmp) << 8);
				in++;
			}
			if((d & 0xFC00) == 0xDC00)
			{
				c &= 0x03FF;
				c <<= 10;
				c |= d & 0x03FF;
				c += 0x10000;
			}
			else
			{
				*outlen = out - outstart;
				return -1;
			}
		}
		
		if(out >= outend)    break;
		if     (c <    0x80) { *out++ =  c;                        bits= -6; }
		else if(c <   0x800) { *out++ = ((c >>  6) & 0x1F) | 0xC0; bits=  0; }
		else if(c < 0x10000) { *out++ = ((c >> 12) & 0x0F) | 0xE0; bits=  6; }
		else                 { *out++ = ((c >> 18) & 0x07) | 0xF0; bits= 12; }
		
		for(; bits >= 0; bits-= 6)
		{
			if (out >= outend)
				break;
			*out++ = ((c >> bits) & 0x3F) | 0x80;
		}
		processed = (const unsigned char*) in;
	}
	
	*outlen = out - outstart;
	
	return *outlen;
}


int UTF8ToUTF16(const unsigned char *in, int inlen,unsigned char *outb, int *outlen)
{
	unsigned short *out = (unsigned short*)outb;
	const unsigned char* processed = in;
	unsigned short *outstart= out, *outend;
	const unsigned char* inend;
	unsigned int c, d;
	int trailing;
	unsigned char *tmp;
	unsigned short tmp1, tmp2;

	if((out == NULL) || (outlen == NULL) || (inlen == 0)) return -1;
	if(in == NULL)
		{
		*outlen = 0;
		return 0;
		}
	inend= in + inlen;
	outend = out + (*outlen / 2);
	while(in < inend)
		{
		d= *in++;
		if     (d < 0x80) { c= d; trailing= 0; }
		else if(d < 0xC0)
			{
			*outlen = (out - outstart) * 2;
			return -1;
			}
		else if(d < 0xE0)  { c= d & 0x1F; trailing= 1; }
		else if(d < 0xF0)  { c= d & 0x0F; trailing= 2; }
		else if(d < 0xF8)  { c= d & 0x07; trailing= 3; }
		else 
			{
			*outlen = (out - outstart) * 2;
			return -1;
			}

		if(inend - in < trailing)  break;

		for(; trailing; trailing--)
			{
			if((in >= inend) || (((d= *in++) & 0xC0) != 0x80))
				break;
			c <<= 6;
			c |= d & 0x3F;
			}

		if(c < 0x10000)
			{
			if(out >= outend) break;
			if(isLittleEndian) { *out++ = c; } 
			else
				{
				tmp        = (unsigned char *) out;
				*tmp       = c ;
				*(tmp + 1) = c >> 8 ;
				out++;
				}
			}
		else if(c < 0x110000)
			{
			if(out+1 >= outend) break;
			c -= 0x10000;
			if(isLittleEndian)
				{
				*out++ = 0xD800 | (c >> 10);
				*out++ = 0xDC00 | (c & 0x03FF);
				} 
			else
				{
				tmp1       = 0xD800 | (c >> 10);
				tmp        = (unsigned char *) out;
				*tmp       = (unsigned char) tmp1;
				*(tmp + 1) = tmp1 >> 8;
				out++;

				tmp2       = 0xDC00 | (c & 0x03FF);
				tmp        = (unsigned char *) out;
				*tmp       = (unsigned char) tmp2;
				*(tmp + 1) = tmp2 >> 8;
				out++;
				}
			}
		else break;

		processed = in;
		}

	*outlen = (out - outstart) * 2;

	return *outlen;
}

int UTF16ToGB2312(const unsigned char *in, int inlen,unsigned char *out, int *outlen)
{
	int i,len = 0,ret = 0;
    unsigned long wc = 0;
    

	if((in == NULL) || (inlen < 2) || (out == NULL) || (outlen == NULL))
		return -1;

	
	for(i=0;i<inlen;i+=2)
	{
		wc = (in[i+1]<<8) + in[i];
		
		ret =  euc_cn_wctomb(out+len,wc,2);
		if(ret > 0)
		{
			len += ret;
		}else{
			len = -1;
			break;
		}
	}

   *outlen = len;
   return len;
}

int GB2312ToUTF16(const unsigned char *in, int inlen,unsigned char *out, int *outlen)
{
	int ret = 0,i=0,len=0;
	unsigned long wc = 0;
   
    if((in==NULL) || (inlen==0) || (out==NULL) || outlen==NULL)
		return -1;

    for (i=0;i<inlen;i++)
    {
		wc = 0;
		ret = euc_cn_mbtowc(&wc,in+i,2);
		if(ret > 0){
			out[len++] = (unsigned char)(wc&0xFF);
			out[len++] = (unsigned char)((wc&0xFF00)>>8);
		}
		if(wc >= 0x80)
			i++;
    }
    
    *outlen = len;
    if(len==0)
		len = -1;

	return len;
}

