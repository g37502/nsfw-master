#include <stdlib.h>
#include <wctype.h>
#include <ctype.h>
#include <wchar.h>
#include <string.h>
#include <locale.h>
#include <iconv.h>
#include <errno.h>
#if defined(HAVE_LOCALE_CHARSET)
# include <localcharset.h>
#elif defined (HAVE_LANGINFO_CODESET)
# include <langinfo.h>
# define locale_charset()  nl_langinfo(CODESET)
#endif
#include "StrUtils.h"

namespace TxtFilter
{
	namespace StrUtils
	{
		char * StringTrim(char *s) {
			char *p;

			while (*s && isspace(*s))
				++s;

			p = s + strlen(s) - 1;
			while (isspace(*p))
				--p;
			*++p = '\0';

			return s;
		}

		wchar_t * StringTrimW(wchar_t *s)
		{
			wchar_t *p;

			/* skip leading white spaces */
			while (*s && iswspace (*s))
				++s;

			/* trim trailing white spaces */
			p = s + wcslen (s) - 1;
			while (iswspace (*p))
				--p;
			*++p = L'\0';

			return s;
		}

		char *w2c(const wchar_t *pw)
		{
			if(!pw) return NULL;

			size_t size= wcslen(pw)*sizeof(wchar_t);
			char *pc;
			if(!(pc = (char*)malloc(size)))
			{
				fprintf(stderr, "malloc fail");
				return NULL;
			}
			wcstombs(pc,pw,size);
			return pc;
		}

		int UTF82W(const char* a_szSrc, wchar_t* a_szDest, int a_nDestSize)
		{
			size_t result;
			iconv_t env;
			int size = strlen(a_szSrc)+1;
			int one = 1;
			env = iconv_open("WCHAR_T","UTF-8");
			if (env==(iconv_t)-1)
			{
				fprintf(stderr, "StrUtils::UTF82W: iconv_open UTF8->WCHAR_T error (%s).\n", strerror(errno));
				return -1;
			}
			
			if(iconvctl(env,ICONV_SET_DISCARD_ILSEQ,&one))
			{
                fprintf(stderr, "StrUtils::UTF82W: iconvctl error (%s).\n", strerror(errno));
                iconv_close(env);
				return -1;
			}
			
			result = iconv(env,(char**)&a_szSrc,(size_t*)&size,(char**)&a_szDest,(size_t*)&a_nDestSize);
			if (result==(size_t)-1)
			{
				fprintf(stderr, "StrUtils::UTF82W: iconv UTF8->WCHAR_T error (%s).\n", strerror(errno));
                iconv_close(env);
				return -1;
			}
			iconv_close(env);
			return (int)result;
		}

		int W2UTF8(const wchar_t* a_szSrc, int a_nSrcSize, char* a_szDest, int a_nDestSize)
		{
			size_t result;
			iconv_t env;
			env = iconv_open("UTF-8","WCHAR_T");
			if (env==(iconv_t)-1)
			{
				fprintf(stderr, "StrUtils::W2UTF8: iconv_open WCHAR_T->UTF8 error (%s).\n",strerror(errno));
				return -1;
			}
			result = iconv(env,(char**)&a_szSrc,(size_t*)&a_nSrcSize,(char**)&a_szDest,(size_t*)&a_nDestSize);
			if (result==(size_t)-1)
			{
				fprintf(stderr, "StrUtils::W2UTF8: iconv WCHAR_T->UTF8 error (%s)\n", strerror(errno));
                iconv_close(env);
				return -1;
			}
			iconv_close(env);
			return (int)result;
		}

		int ChineseCharNum(wchar_t *pStr) {
			if (NULL == pStr) {
				fprintf(stderr, "StrUtils::ChineseCharNum: input param cannot be NULL.\n");
				return -1;
			}

			int count = 0;

			while (*pStr) {

				if ((*pStr >= 0x2400 && *pStr <= 0x4db5) ||
					(*pStr >= 0x4e00 && *pStr <= 0x9fbb) ||
					(*pStr >= 0xf900 && *pStr <= 0xfad9) ||
					(*pStr >= 0x20000 && *pStr <= 0x2a6d6) ||
					(*pStr >= 0x2f800 && *pStr <= 0x2fa1d)) {

					count ++;
				}

				pStr ++;
			}

			return count;
		}
	}
}
