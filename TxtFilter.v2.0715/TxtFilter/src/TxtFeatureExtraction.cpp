#include <stdio.h>
#include <wctype.h>
#include <ctype.h>
#include <string.h>
#include <wchar.h>
#include <stdlib.h>
#include <stdlib.h>
#include <locale.h>
#include "TxtFeature.h"
#include "TxtFeatureExtraction.h"
#include "StrUtils.h"
namespace TxtFilter
{
	TxtFeatureExtraction::TxtFeatureExtraction(void) :
	m_trie(NULL),
	m_df(NULL)
	{
	}

	TxtFeatureExtraction::~TxtFeatureExtraction(void)
	{
		Uninitialize();
	}

	int TxtFeatureExtraction::Uninitialize()
	{

		trie_free(m_trie);

		if (m_df) delete [] m_df;

		return 0;
	}

	int TxtFeatureExtraction::InitializeFromKeywords(const char *filename)
	{
		if (NULL == filename) {
			fprintf(stderr, "TxtFeatureExtraction::InitializeFromKeywords: Input filename is NULL\n");
			return -1;
		}

		FILE *keywordsFile = NULL;
		AlphaMap *alphaMap = NULL;
		unsigned char line[512] = {0};
		
		m_dim = 0;

		keywordsFile = fopen(filename, "r");
		if (!keywordsFile) {
			fprintf(stderr, "TxtFeatureExtraction::InitializeFromKeywords: Cannot open keywords file\n");
			return -1;
		}

		alphaMap = alpha_map_new();
		/*
		alpha_map_add_range(alphaMap, 0x30, 0x39); // numbers
		alpha_map_add_range(alphaMap, 0x61, 0x7a); // a-z
		alpha_map_add_range(alphaMap, 0x41, 0x5a); // A-Z
		alpha_map_add_range(alphaMap, 0x2400, 0x4db5); // CJK Unified Ideographs Extension A
		alpha_map_add_range(alphaMap, 0x4e00, 0x9fbb); // CJK Unified Ideographs 1.1 & 4.1
		alpha_map_add_range(alphaMap, 0xf900, 0xfad9); // CJK Compatibility Ideographs 1.1, 3.2 & 4.1
		alpha_map_add_range(alphaMap, 0xff00, 0xffef); // Misc
		alpha_map_add_range(alphaMap, 0x20000, 0x2a6d6); // CJK Unified Ideographs Extension B
		alpha_map_add_range(alphaMap, 0x2f800, 0x2fa1d); // CJK Compatibility Supplement
		*/
		alpha_map_add_range(alphaMap, 0x00, 0xff);

		m_trie = trie_new(alphaMap);

		alpha_map_free(alphaMap);

		if (!m_trie)
		{
			fprintf(stderr, "TxtFeatureExtraction::InitializeFromKeywords: Cannot allocate new trie.\n");
			return -1;
		}

		while (fgets((char*)line, 512, keywordsFile))
		{
			unsigned char *key = NULL;
			unsigned char *value = NULL;
			AlphaChar keyAlpha[512] = {0};

			key = line;

			if ('\0' != *key) {
				for (value = key; *value && !strchr("\t", *value); ++ value);
				if ('\0' != *value) {
					*value ++= '\0';
					while (isspace(*value)) ++value;
				}

				int idx = 0;
				while (key[idx]) {
					keyAlpha[idx] = key[idx];
					idx ++;
				}

				if (!trie_store(m_trie, keyAlpha, m_dim))
				{
					fprintf(stderr, "TxtFeatureExtraction::InitializeFromKeywords: Failed to add key.\n");
				}
			}
			m_dim++;
		}

		fclose(keywordsFile);

		if (m_dim == 0) {
			fprintf(stderr, "TxtFeatureExtraction::InitializeFromKeywords: Cannot read input file.\n");
			return -2;
		}

		if (m_df) delete [] m_df;
		m_df = new int[m_dim];
		memset(m_df, 0, sizeof(m_df[0]) * m_dim);

		return 0;
	}

	int TxtFeatureExtraction::GetStringId(unsigned char *str) {
		AlphaChar keyAlpha[256] = {0};
		TrieData data;

		if (str == NULL) {
			fprintf(stderr, "TxtFeatureExtraction::GetStringId: no strSample specified.\n");
			return -1;
		}

		int idx = 0;
		while (str[idx])
		{
			keyAlpha[idx] = str[idx];
			idx ++;
		}

		if (trie_retrieve(m_trie, keyAlpha, &data)) {
			return (int)data;
		}
		return -1;
	}

	int TxtFeatureExtraction::GetDim() {
		return m_dim;
	}

	int TxtFeatureExtraction::ProcessUTF8Sample(unsigned char *strSample, int sizeStr, TxtFeature **ppFeatureOut, bool bAccumulateDF) {

		if (strSample == NULL || ppFeatureOut == NULL) {
			fprintf(stderr, "TxtFeatureExtraction::ProcessTxtSample: no strSample/pFeatureOut specified.\n");
			return -1;
		}

		if (m_trie == NULL) {
			fprintf(stderr, "TxtFeatureExtraction::ProcessTxtSample: TxtFeatureExtraction not initialized.\n");
			return -1;
		}

		TrieState *s;
		int strLen = strlen((char*)strSample) + 1;

		if (strLen <= 0) return -1;

		AlphaChar *text = new AlphaChar[strLen];
		memset(text, 0, sizeof(text[0]) * strLen);
		AlphaChar *p = NULL;
		AlphaChar *base = text;

		for (int idx = 0; idx < strLen; idx ++) text[idx] = strSample[idx];

		int hitNumber = 0;
		*ppFeatureOut = new TxtFeature();

		while (*text && text - base < strLen) {

			s = trie_root(m_trie);
			p = text;

			if (!trie_state_is_walkable(s, *p)) {
				text ++;
				trie_state_free(s);
				continue;
			}
			else {
				trie_state_walk(s, *p++);
			}

			while (trie_state_is_walkable(s, *p) && !trie_state_is_terminal(s) && p - base < strLen) {
				trie_state_walk(s, *p++);
			}

			if (trie_state_is_terminal(s)) {
				// find a match
				hitNumber ++;

				int dataIdx = trie_state_get_data(s);
				if (dataIdx >= m_dim || dataIdx < 0) {
					text = p;
					trie_state_free(s);
					continue;
				}

				if (bAccumulateDF) {

					(*ppFeatureOut)->IncValue(dataIdx, 1); // accumulate TF
					m_df[dataIdx] += 1; // accumulate DF
				}
				else {

					(*ppFeatureOut)->IncValue(dataIdx, 1 / (double)(m_df[dataIdx]));
				}
			}
			text = p;
			trie_state_free(s);
		}

		delete [] base;

		return 0;
	}

	int TxtFeatureExtraction::ClearDF() {

		if (m_df) delete [] m_df;
		m_df = NULL;

		return 0;
	}

	int TxtFeatureExtraction::SaveDF(FILE *pFile) {

		if (NULL == pFile) {

			fprintf(stderr, "TxtFeatureExtraction::SaveDF: input param cannot be NULL.\n");
			return -1;
		}

		if (NULL == m_df || m_dim <= 0) {

			fprintf(stderr, "TxtFeatureExtraction::SaveDF: data is wrong, cannot be saved.\n");
			return -2;
		}

		fprintf(pFile, "DF:\n");
		for (int idx = 0; idx < m_dim; idx ++) {

			fprintf(pFile, "%d,", m_df[idx]);
		}
		fprintf(pFile, "\n");

		return 0;
	}

	int TxtFeatureExtraction::LoadDF(FILE *pFile) {

		if (NULL == pFile) {

			fprintf(stderr, "TxtFeatureExtraction::LoadDF: input param cannot be NULL.\n");
			return -1;
		}

		if (m_dim <= 0) {

			fprintf(stderr, "TxtFeatureExtraction::LoadDF: data is wrong, cannot be saved.\n");
			return -2;
		}

		if (NULL != m_df) delete m_df;
		m_df = new int[m_dim];
		memset(m_df, 0, sizeof(m_df[0]) * m_dim);

		if (fscanf(pFile, "DF:\n") < 0) goto exitFileWrong;
		for (int idx = 0; idx < m_dim; idx ++) {
			if (fscanf(pFile, "%d,", &m_df[idx]) <= 0) goto exitFileWrong;
		}

		if (fscanf(pFile, "\n") < 0) goto exitFileWrong;

		return 0;
exitFileWrong:
		fprintf(stderr, "TxtFeatureExtraction::LoadDF: file content wrong.\n");
		return -3;
	}

	int TxtFeatureExtraction::GetDF(int idx) {

		if (idx >= 0 && idx < m_dim && m_df) {

			return m_df[idx];
		}
		else {

			return 0;
		}
	}

}
