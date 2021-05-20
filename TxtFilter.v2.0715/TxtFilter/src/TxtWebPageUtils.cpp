#include <sys/types.h>
#include <regex.h>
#include <wchar.h>
#include <stdio.h>
#include <string.h>
#include "TxtWebPageUtils.h"

namespace TxtFilter {
	namespace WebPageUtils {

	const char* g_strRegExtractLinks = "<a[^<]*</a>";
	const char* g_strRegHtmlTags = "<[^<]*>";
	const char* g_strRegNewline = "[\r\n]";

	regex_t g_pregExtractLinks;
	regex_t g_pregHtmlTags;
	regex_t g_pregNewline;

	bool g_bInited = false;

	int Initialize() {

		if (g_bInited) return 0;

		if (0 != regcomp(&g_pregExtractLinks, g_strRegExtractLinks, REG_ICASE)) {
			fprintf(stderr, "WebPageUtils::Initialize: cannot initialize regex.\n");
			return -1;
		}

		if (0 != regcomp(&g_pregHtmlTags, g_strRegHtmlTags, REG_ICASE)) {
			fprintf(stderr, "WebPageUtils::Initialize: cannot initialize regex.\n");
			return -1;
		}

		if (0 != regcomp(&g_pregNewline, g_strRegNewline, 0)) {
			fprintf(stderr, "WebPageUtils::Initialize: cannot initialize regex.\n");
			return -1;
		}

		g_bInited = true;

		return 0;
	}

	int Uninitialize() {

		if (!g_bInited) return -1;

		regfree(&g_pregExtractLinks);
		regfree(&g_pregHtmlTags);
		regfree(&g_pregNewline);

		g_bInited = false;

		return 0;
	}

	int RemoveMatchedStr(const char *strInput, char *strOutput, regex_t *pRegex) {

		if (NULL == strInput || NULL == strOutput || NULL == pRegex) {
			fprintf(stderr, "WebPageUtils::RemoveMatchedStr: strInput or strOutput or pRegex cannot be NULL.\n");
			return -1;
		}

		if (!g_bInited) {
			fprintf(stderr, "WebPageUtils::RemoveMatchedStr: not initialized.\n");
			return -1;
		}

		char* pInput = (char*)strInput;
		char* pScan = (char*)strOutput;
		regmatch_t matcht;

		while (0 == regexec(pRegex, pInput, 1, &matcht, 0) && matcht.rm_so != -1)
		{
			for (int scanIdx = 0; scanIdx < matcht.rm_so; scanIdx ++) {
				*pScan = pInput[scanIdx];
				pScan ++;
			}
			pInput += matcht.rm_eo;
		}

		if (strOutput[0]) {
			while (*pInput) {
				*pScan = *pInput;
				pScan ++;
				pInput ++;
			}
		}

		return 0;
	}

	void RemoveNewline(char *str) {
		if (NULL == str) return;

		int strLen = strlen(str);
		if (strLen <= 0) return;

		char *buf = new char[strLen + 1];
		memset(buf, 0, strLen + 1);
		char *bufScan = buf;

		for (int idx = 0; idx < strLen; idx ++) {
			if (str[idx] == '\r' || str[idx] == '\n') continue;
			*bufScan++ = str[idx];
		}

		bufScan = buf;
		memset(str, 0, strLen);
		while (*bufScan) *str++ = *bufScan++;
		delete [] buf;
	}

	int ExtractBody(const char *strInput, char *strOutput, bool removeLinkText) {

		if (NULL == strInput || NULL == strOutput) {
			fprintf(stderr, "WebPageUtils::ExtractBody: strInput or strOutput cannot be NULL.\n");
			return -1;
		}

		if (!g_bInited) {
			fprintf(stderr, "WebPageUtils::ExtractBody: not initialized.\n");
			return -1;
		}


		strOutput[0] = '\0';

		if (removeLinkText) {
			int bufLen = strlen(strInput);
			char *buf = new char[bufLen];
			memset(buf, 0, bufLen);

			RemoveMatchedStr(strInput, buf, &g_pregExtractLinks);

			RemoveMatchedStr(buf, strOutput, &g_pregHtmlTags);

			delete [] buf;
		}
		else {

			RemoveMatchedStr(strInput, strOutput, &g_pregHtmlTags);
		}

		RemoveNewline(strOutput);

		if (strOutput[0] == '\0'){
			//fprintf(stderr, "WebPageUtils::ExtractBody: cannot find a match.\n");
			return -2;
		}

		return 0;
	}

	}
}
