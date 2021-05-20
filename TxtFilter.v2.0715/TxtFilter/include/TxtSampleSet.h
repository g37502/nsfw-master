#pragma once
#include <stdio.h>
#define MAX_SAMPLETAG 2

namespace TxtFilter {

	class TxtFeatureSet;
	class TxtFeatureExtraction;

	class TxtSampleSet {
	public:
		TxtSampleSet(TxtFeatureExtraction *pExtractor, int nSampleTags = MAX_SAMPLETAG);
		virtual ~TxtSampleSet();

	public:
		int AddSample(unsigned char *strSample, int sizeStr, int nSampleTag);
		int SaveToFile(FILE *pFile);
		int LoadFromFile(FILE *pFile);
		int GetNumTags();
		TxtFeatureSet* GetFeatureSet(int tag);
		double GetClassifyScore(TxtFeature *pFeature, TxtSimFunc pSimFunc, int K);

	private:
		TxtFeatureExtraction *m_pExtractor;
		TxtFeatureSet *m_featureSetList;
		int m_nSampleTags;
	};

}
