#include <stdio.h>
#include "TxtFeatureSet.h"
#include "TxtFeature.h"
#include "TxtFeatureExtraction.h"

namespace TxtFilter {

	TxtFeatureSet::TxtFeatureSet() {
	}

	TxtFeatureSet::~TxtFeatureSet() {

		int length = Count();
		for (int idx = 0; idx < length; idx ++) {
			if (m_featureList[idx]) delete m_featureList[idx];
		}

		m_featureList.clear();
	}

	int TxtFeatureSet::AddFeature(TxtFeature *pFeature) {

		if (NULL == pFeature) {
			fprintf(stderr, "TxtFeatureSet::AddFeature: pFeature is NULL.\n");
			return -1;
		}

		m_featureList.push_back(pFeature);

		return 0;
	}

	int TxtFeatureSet::Count() {

		return m_featureList.size();

	}

	TxtFeature* TxtFeatureSet::GetFeature(unsigned int idx) {

		if (idx < m_featureList.size()) {
			return m_featureList[idx];
		}

		return NULL;
	}

	double TxtFeatureSet::GetSimScore(TxtFeature *pFeature, TxtSimFunc pSimFunc) {

		if (NULL == pFeature || NULL == pSimFunc) {
			fprintf(stderr, "TxtFeatureSet::GetSimScore: pFeature and pSimFunc cannot be NULL.\n");
			return -1;
		}

		int length = Count();
		double result = 0;
		double baseScore1 = pFeature->Norm();

		if (length == 0 || baseScore1 == 0) return 0;

		for (int idx = 0; idx < length; idx ++) {
			TxtFeatureDataType score1 = (*pSimFunc)(pFeature, m_featureList[idx]);
			if (score1 == 0) continue;
			double baseScore2 = m_featureList[idx]->Norm();
			if (baseScore2 == 0) continue;
			result += (double) score1 / baseScore1 / baseScore2;
		}

		result /= (double) length;

		return result;
	}

	void TxtFeatureSet::SetTag(int tag) {

		m_tag = tag;
	}

	int TxtFeatureSet::LoadFromFile(FILE *pFile) {

		if (NULL == pFile) {
			fprintf(stderr, "TxtFeatureSet::LoadFromFile pFile cannot be NULL.\n");
			return -1;
		}

		m_featureList.clear();

		int count = 0;
		if (fscanf(pFile, "Count:%d\n", &count) < 0) {
			fprintf(stderr, "TxtFeatureSet::LoadFromFile file corrupted.\n");
			return -3;
		}

		if (count <= 0) {
			fprintf(stderr, "TxtFeatureSet::LoadFromFile file corrupted.\n");
			return -2;
		}

		for (int idx = 0; idx < count; idx ++) {
			TxtFeature *pFeature;
			if (0 == TxtFeature::LoadFromFile(pFile, &pFeature)) {
				m_featureList.push_back(pFeature);
			}
			else {
				fprintf(stderr, "TxtFeatureSet::LoadFromFile content wrong.\n");
				return -3;
			}
		}

		return 0;
	}

	int TxtFeatureSet::SaveToFile(FILE *pFile) {

		if (NULL == pFile) {
			fprintf(stderr, "TxtFeatureSet::SaveToFile pFile cannot be NULL.\n");
			return -1;
		}

		int length = Count();
		fprintf(pFile, "Count:%d\n", length);
		for (int idx = 0; idx < length; idx ++) {
			if (0 != m_featureList[idx]->SaveToFile(pFile)) {
				fprintf(stderr, "TxtFeatureSet::SaveToFile cannot save to file.\n");
				return -1;
			}
		}

		return 0;
	}
}
