#pragma once

extern "C" {
#include "trie.h"
}

namespace TxtFilter
{
	class TxtFeature;
	class TxtFeatureExtraction
	{
	public:
		TxtFeatureExtraction(void);
		~TxtFeatureExtraction(void);

	public:
		int InitializeFromKeywords(const char *filename);
		int Uninitialize();

		int ProcessUTF8Sample(unsigned char *strSample, int sizeStr, TxtFeature **ppFeatureOut, bool bAccumulateDF);

		int ClearDF();
		int SaveDF(FILE *pFile);
		int LoadDF(FILE *pFile);
		int GetDF(int idx);

		int GetStringId(unsigned char *str);

		int GetDim();

	private:
		Trie *m_trie;
		int *m_df;

	public:
		int m_dim;
	};
}
