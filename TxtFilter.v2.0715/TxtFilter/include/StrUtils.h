#pragma once
#include "alpha-map.h"
namespace TxtFilter
{
	namespace StrUtils
	{
		wchar_t * StringTrimW (wchar_t *s);
		char * StringTrim(char *s);

		char *w2c(const wchar_t *pw);
		int UTF82W(const char* a_szSrc, wchar_t* a_szDest, int a_nDestSize);
		int W2UTF8(const wchar_t* a_szSrc, int a_nSrcSize, char* a_szDest, int a_nDestSize);

		int ChineseCharNum(wchar_t *pStr);
	}
}
