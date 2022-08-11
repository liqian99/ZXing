#include "CharacterSetECI.h"

CharacterSetECI g_CharacterSetECI[] = {
	0,   "Cp437",               "",                 "",       "",
    1,   "ISO8859_1" ,          "ISO-8859-1",       "",       "",
	2,   "Cp437",               "",                 "",       "",
	3,   "ISO8859_1",           "ISO-8859-1",       "",       "",
	4,   "ISO8859_2",           "ISO-8859-2",       "",       "",
	5,   "ISO8859_3",           "ISO-8859-3",       "",       "",
	6,   "ISO8859_4",           "ISO-8859-4",       "",       "",
	7,   "ISO8859_5",           "ISO-8859-5",       "",       "",
	8,   "ISO8859_6",           "ISO-8859-6",       "",       "",
	9,   "ISO8859_7",           "ISO-8859-7",       "",       "",
	10,  "ISO8859_8",           "ISO-8859-8",       "",       "",
	11,  "ISO8859_9",           "ISO-8859-9",       "",       "",
	12,  "ISO8859_10",          "ISO-8859-10",      "",       "",
	13,  "ISO8859_11",          "ISO-8859-11",      "",       "",
	15,  "ISO8859_13",          "ISO-8859-13",      "",       "",
	16,  "ISO8859_14",          "ISO-8859-14",      "",       "",
	17,  "ISO8859_15",          "ISO-8859-15",      "",       "",
	18,  "ISO8859_16",          "ISO-8859-16",      "",       "",
	20,  "SJIS",                "Shift_JIS",        "",       "",
	21,  "Cp1250",              "windows-1250",     "",       "",
	22,  "Cp1251",              "windows-1251",     "",       "",
	23,  "Cp1252",              "windows-1252",     "",       "",
	24,  "Cp1256",              "windows-1256",     "",       "",
	25,  "UnicodeBigUnmarked",  "UTF-16BE",         "UnicodeBig",   "",      
	26,  "UTF8",                "UTF-8",            "",       "",
	27,  "ASCII",               "US-ASCII",         "",       "",
	28,  "Big5",                "",                 "",             "",
	29,  "GB18030",             "GB2312",           "EUC_CN",       "GBK",
    30,  "EUC_KR",              "EUC-KR",           "",              "",
    170, "ASCII",               "US-ASCII",         "",              "",
};

char const* CharacterSetECIGetName(CharacterSetECI *pECI)
{
    return pECI->str[0];
}

int CharacterSetECIGetValue(CharacterSetECI *pECI)
{
	return pECI->index;
}

CharacterSetECI* CharacterSetECIGetByValue(int val)
{
	if(val < 0 || val>=900)
		return NULL;

	if(val<=13)
		return &g_CharacterSetECI[val];
	else if(val>=15 && val<=18)
		return &g_CharacterSetECI[val-1];
	else if(val>=20 && val<=30)
        return &g_CharacterSetECI[val-2];
	else if(val==170)
		return &g_CharacterSetECI[29];
	else
		return NULL;
}