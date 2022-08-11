#ifndef __BASE_TYPE_DEFT_H__
#define __BASE_TYPE_DEFT_H__

#include <math.h>


#ifndef MAX
#define MAX(x,y) ((x)>(y)?(x):(y))
#endif

#ifndef MIN
#define MIN(x,y) ((x)<(y)?(x):(y))
#endif

#ifndef ABS
#define ABS(x) ((x)>0?(x):(-(x)))
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

typedef int BOOL;

// #define F2I(f)  (*((int *)&f))
//#define isnan(a) (F2I(a)==0)
//#define nan()  ((float)sqrt(-1.0f))
// #define isnan(fN) (!((fN)==(fN))?FALSE:TRUE)

#ifndef INT_MIN
#define INT_MIN     (-2147483647 - 1) /* minimum (signed) int value */
#endif

#ifndef INT_MAX
#define INT_MAX       2147483647 
#endif

#define nan() ((float)(0.0f))
#define isnan(fN)  (fN==nan()?TRUE:FALSE)

// static const int g_const_logBits = 5;//ZX_LOG_DIGITS(32);
// static const int g_const_bitsMask = 31;//(1 << logBits) - 1;

#ifdef _DEBUG
#include <stdlib.h>
#include <stdio.h>
#endif

#define bitsPerWord 32
#define logBits     5
#define bitsMask    31



#define RT_ERR_BITARRAY_INIT   (-1001)  //初始化bitArray错误
#define RT_ERR_BITMATRIX_INIT  (-1002)  //初始化bitMatrix错误码
#define RT_ERR_TOPLEFT         (-1003)  //左上错误
#define RT_ERR_WIDTHHEIGHT     (-1004)  //宽高错误
#define RT_ERR_FITREGION       (-1005)  //区域超出范围
#define RT_ERR_PARANULL        (-1006)  //参数指针为NULL
#define RT_ERR_PARAFIT         (-1007)  //传入的参数不匹配
#define RT_ERR_FOUNDPATTERN    (-1008)  //没有找到PATTERN
#define RT_ERR_DECODE93        (-1009)  //解码Code93失败
#define RT_ERR_QR_DIMENSION     (-1010)
#define RT_ERR_QR_DOFORMATINFO  (-1011)
#define RT_ERR_QR_READFORMATINFO  (-1012)
#define RT_ERR_QR_READVERSION   (-1013)
#define RT_ERR_QR_CODEWORDS     (-1014)
#define RT_ERR_QR_DBMORETHANONE (-1015)
#define RT_ERR_QR_CODESIZE      (-1016)
#define RT_ERR_READBITS         (-1017)
#define RT_ERR_QR_FORMAT        (-1018)
#define RT_ERR_QR_ENOUGHPATTERN (-1019)
#define RT_ERR_QR_BADDIMENSION  (-1020)
#define RT_ERR_OUTBOUND         (-1021)
#define RT_ERR_QR_BADMODULESIZE (-1022)
#define RT_ERR_QR_DIMENSIONFORVERSION (-1023)
#define RT_ERR_QR_REGIONTOOSMALL (-1024)
#define RT_ERR_QR_VERSION        (-1025)
#define RT_ERR_QR_DATABLOCK      (-1026)
#define RT_ERR_QR_NEEDCOEFFICIENTS (-1027)
#define RT_ERR_MUST_NON_NEGATIVE (-1028)
#define RT_ERR_DEGREENOTMATCH    (-1029)
#define RT_ERR_DIVIDEBYZERO      (-1030)
#define RT_ERR_ISZERO            (-1031)
#define RT_ERR_ALGORITHM         (-1032) 
#define RT_ERR_BADERRLOCATION    (-1033) 
#define RT_ERR_ILLEGALCHARACTER  (-1034)
#define RT_ERR_DECODEONED        (-1035)
#define RT_ERR_DECODE39          (-1036)  //解码Code39失败
#define RT_ERR_DECODE128         (-1037)  
#define RT_ERR_FORMAT            (-1038)
#define RT_ERR_DECODACODE        (-1039)  
#define RT_ERR_DECODEUPCEAN      (-1040)
#define RT_ERR_CHECKSUM          (-1041)
#define RT_ERR_DECODEEAN13       (-1042)
#define RT_ERR_DECODEEAN8        (-1043)
#define RT_ERR_DECODEUPCA        (-1044)
#define RT_ERR_DECODEUPCE        (-1045)
#define RT_ERR_DECODEITF         (-1046)




#define RT_ERR_UNKNOW          (-2000)



#define ZXING_ARRAY_LEN(v) ((int)(sizeof(v)/sizeof(v[0])))

#endif /*__BASE_TYPE_DEFT_H__*/