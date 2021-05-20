#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include <string.h>


#include "TxtFeature.h"
#include "TxtFeatureSet.h"
#include "TxtFeatureExtraction.h"
#include "TxtSampleSet.h"

namespace TxtFilter {

	TxtSampleSet::TxtSampleSet(TxtFeatureExtraction *pExtractor, int numSampleTags) :
	m_nSampleTags(numSampleTags) {
		m_featureSetList = new TxtFeatureSet[numSampleTags];
		m_pExtractor = pExtractor;
	}

	TxtSampleSet::~TxtSampleSet() {
		delete [] m_featureSetList;
	}

	int TxtSampleSet::AddSample(unsigned char *strSample, int sizeStr, int sampleTag) {
		if (NULL == strSample) {
			fprintf(stderr, "TxtSampleSet::AddSample: strSample is NULL.\n");
			return -1;
		}

		if (sampleTag < 0 || sampleTag >= m_nSampleTags) {
			fprintf(stderr, "TxtSampleSet::AddSample: sampleTag not in valid range.\n");
			return -1;
		}


		TxtFeature *pFeature = NULL;

		if (0 == m_pExtractor->ProcessUTF8Sample(strSample, sizeStr, &pFeature, true)) {

			if (pFeature->Norm() < 2) return 0; // skip adding small sample

			m_featureSetList[sampleTag].AddFeature(pFeature);

			m_featureSetList[sampleTag].SetTag(sampleTag);

		}
		else {

			fprintf(stderr, "TxtSampleSet::AddSample: cannot call feature extraction.\n");
			return -2;
		}

		return 0;
	}

	int TxtSampleSet::GetNumTags() {
		return m_nSampleTags;
	}

	TxtFeatureSet* TxtSampleSet::GetFeatureSet(int tag) {
		if (tag >= 0 && tag < m_nSampleTags) {
			return &(m_featureSetList[tag]);
		}
		else {
			return NULL;
		}
	}

	int TxtSampleSet::SaveToFile(FILE *pFile) {
		if (NULL == pFile) {
			fprintf(stderr, "TxtSampleSet::SaveToFile: pFile cannot be NULL.\n");
			return -1;
		}

		if (m_pExtractor) m_pExtractor->SaveDF(pFile);

		for (int idx = 0; idx < m_nSampleTags; idx ++) {
			if (m_featureSetList[idx].Count() > 0) {
				fprintf(pFile, "Tag:%d\n", idx);
				if (m_featureSetList[idx].SaveToFile(pFile) == 0) {
					continue;
				}
				else {
					return -1;
				}
			}
		}

		return 0;
	}

	int TxtSampleSet::LoadFromFile(FILE *pFile) {

		if (NULL == pFile) {
			fprintf(stderr, "TxtSampleSet::LoadFromFile: pFile cannot be NULL.\n");
			return -1;
		}

		if (m_pExtractor) m_pExtractor->LoadDF(pFile);

		int nTag = -1;
		while (EOF != fscanf(pFile, "Tag:%d\n", &nTag)) {

			if (nTag >= 0 && nTag < m_nSampleTags) {

				if (m_featureSetList[nTag].LoadFromFile(pFile) == 0) {
					continue;
				}
				else {
					return -1;
				}
			}
			else {
				fprintf(stderr, "TxtSampleSet::LoadFromFile: File wrong.\n");
				return -1;
			}
		}

		return 0;
	}

	bool isLarger(const std::pair<int, double> p1, const std::pair<int, double> p2) {

		return p1.second > p2.second;
	}

	double TxtSampleSet::GetClassifyScore(TxtFeature *pFeature, TxtSimFunc pSimFunc, int K) {

		if (NULL == pFeature || pSimFunc == NULL) {

			fprintf(stderr, "TxtSampleSet::GetClassifyScore: pFeature or pSimFunc cannot be NULL.\n");
			return -1;
		}

		if (m_nSampleTags <= 0 || m_featureSetList == NULL) {

			return 0;
		}

		std::vector<std::pair<int, double> > simScoreList;

		double pFeatureNorm = pFeature->Norm();
		if (pFeatureNorm == 0) {

			return 0;
		}

		for (int idx = 0; idx < m_nSampleTags; idx ++) {

			int count = m_featureSetList[idx].Count();
			for (int featureIdx = 0; featureIdx < count; featureIdx ++) {

				TxtFeature* p = m_featureSetList[idx].GetFeature(featureIdx);

				if (p) {

					double pNorm = p->Norm();

					if (pNorm == 0) continue;

					double score1 = (*pSimFunc)(pFeature, p);

					simScoreList.push_back(std::make_pair(idx, score1 / pFeatureNorm / pNorm));
				}
			}
		}

		std::sort(simScoreList.begin(), simScoreList.end(), isLarger);

		int simScoreListCount = simScoreList.size();

		int *tagList = new int[m_nSampleTags];
		double score = 0;
		memset(tagList, 0, sizeof(tagList[0]) * m_nSampleTags);

		for (int idx = 0; idx < simScoreListCount && idx < K; idx ++) {

			if (simScoreList[idx].first >= 0 && simScoreList[idx].first < m_nSampleTags) {

				tagList[simScoreList[idx].first] ++;
			}
		}

		if (m_nSampleTags == 2)	{

			if (tagList[1] == 0) {

				 score = tagList[0] * 100;
			}
			else {

				score = tagList[0] / (double)tagList[1];
			}
		}

		delete [] tagList;

		return score * 10;
	}
}
