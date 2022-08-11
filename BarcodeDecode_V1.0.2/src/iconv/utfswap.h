#ifndef __UTFSWAP_H__
#define __UTFSWAP_H__

/**
*  描述: 将 UTF-16 编码转成 UTF-8 编码
*  参数: inb[in]       ----utf-16字串
*        inlenb[in]    ----utf-16字串长度(字节数)
*        out[out]      ----utf-8字串,需提前分配空间。
*        outlen[out]   ----utf-8字串长度,做为参数传入时为提前分配空间的大小。
*  返 回: >= 0     ----转换后utf-8字串长度
*          -1       ----转换出错
**/
int UTF16ToUTF8(const unsigned char *inb, int inlenb,unsigned char *out, int *outlen);

/**
*  描述: 将 UTF-8 编码转成 UTF-16 编码
*  参数: inb[in]       ----utf-8字串
*        inlenb[in]    ----utf-8字串长度(字节数)
*        out[out]      ----utf-16字串,需提前分配空间。
*        outlen[out]   ----utf-16字串长度,做为参数传入时为提前分配空间的大小。
*  返 回: >= 0     ----转换后utf-16字串长度
*          -1       ----转换出错
**/
int UTF8ToUTF16(const unsigned char *in, int inlen,unsigned char *outb, int *outlen);

/**
*  描述: 将 UTF-16 编码转成 GB2312 
*  参数: inb[in]       ----utf-16字串
*        inlenb[in]    ----utf-16字串长度(字节数)
*        out[out]      ----GB2312字串,需提前分配空间。
*        outlen[out]   ----GB2312字串长度,做为参数传入时为提前分配空间的大小。
*  返 回: >= 0     ----转换后GB2312字串长度
*          -1       ----转换出错
**/
int UTF16ToGB2312(const unsigned char *in, int inlen,unsigned char *out, int *outlen);

/**
*  描述: 将 GB2312 编码转成 UTF16 
*  参数: inb[in]       ----GB2312字串
*        inlenb[in]    ----GB2312字串长度(字节数)
*        out[out]      ----UTF16字串,需提前分配空间。
*        outlen[out]   ----UTF16字串长度,做为参数传入时为提前分配空间的大小。
*  返 回: >= 0     ----转换后UTF16字串长度
*          -1       ----转换出错
**/
int GB2312ToUTF16(const unsigned char *in, int inlen,unsigned char *out, int *outlen);


#endif