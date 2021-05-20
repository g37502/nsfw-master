#include <string.h>
#include <math.h>
#include <algorithm>

#include "TxtFeature.h"

namespace TxtFilter {

	inline bool isIdxSmaller(std::pair<int, TxtFeatureDataType> p1, std::pair<int, TxtFeatureDataType> p2) {

		return p1.first < p2.first;
	}

	TxtFeature::TxtFeature() {

		m_norm = 0;
		m_dim = 900;
	}

	TxtFeature::~TxtFeature() {

	}

	int TxtFeature::GetDim() {

		return m_dim;
	}

	TxtFeatureArrayType & TxtFeature::GetData() {

		return m_data;
	}

	int TxtFeature::GetValue(int idx) {

		if (idx < 0 || idx >= m_dim) return 0;

		int count = m_data.size();

		for (int i = 0; i < count; i ++) {

			if (m_data[i].first == idx) {

				return m_data[i].second;
			}
		}

		return 0;
	}

	int TxtFeature::SetValue(int idx, TxtFeatureDataType value) {

		if (idx < 0 || idx >= m_dim) return 0;

		int count = m_data.size();
		bool done = false;

		for (int i = 0; i < count; i ++) {

			if (m_data[i].first == idx) {

				m_data[i].second = value;

				done = true;

				break;
			}
		}

		if (!done) m_data.push_back(std::make_pair(idx, value));

		std::sort(m_data.begin(), m_data.end(), isIdxSmaller);

		return 0;
	}

	int TxtFeature::IncValue(int idx, TxtFeatureDataType value) {

		if (idx < 0 || idx >= m_dim) return 0;

		int count = m_data.size();
		bool done = false;

		for (int i = 0; i < count; i ++) {

			if (m_data[i].first == idx) {

				m_data[i].second += value;

				done = true;

				break;
			}
		}

		if (!done) m_data.push_back(std::make_pair(idx, value));

		std::sort(m_data.begin(), m_data.end(), isIdxSmaller);

		return 0;
	}

	double TxtFeature::Norm() {

		if (m_norm == 0) {

			TxtFeatureDataType norm = 0;

			int count = m_data.size();

			for (int idx = 0; idx < count; idx ++) {

				norm += m_data[idx].second * m_data[idx].second;
			}

			m_norm = sqrt(norm);
		}

		return m_norm;
	}

	int TxtFeature::SaveToFile(FILE *pFile) {

		if (NULL == pFile) {
			fprintf(stderr, "TxtFeature::SaveToFile pFile cannot be NULL.\n");
			return -1;
		}

		if (fprintf(pFile, "%d ", m_data.size()) > 0) {

			int count = m_data.size();
			for (int idx = 0; idx < count; idx ++) {
				if (fprintf(pFile, "%d,%f ", m_data[idx].first, m_data[idx].second) > 0) {
					continue;
				}
				else {
					goto exitFileWrong;
				}
			}
			fprintf(pFile, "\n");
		}
		else {
			goto exitFileWrong;
		}
		return 0;

	exitFileWrong:
		fprintf(stderr, "TxtFeature::SaveToFile file wrong.\n");
		return -1;
	}

	int TxtFeature::LoadFromFile(FILE *pFile, TxtFeature **ppFeatureOut) {

		if (NULL == pFile) {
			fprintf(stderr, "TxtFeature::LoadFromFile pFile cannot be NULL.\n");
			return -1;
		}

		if (NULL == ppFeatureOut) {
			fprintf(stderr, "TxtFeature::LoadFromFile ppFeatureOut cannot be NULL.\n");
			return -1;
		}

		int dim = 0;

		if (fscanf(pFile, "%d ", &dim) > 0) {
			*ppFeatureOut = new TxtFeature();
			for (int idx = 0; idx < dim; idx ++) {
				int dataIdx = 0;
				float dataValue = 0;

				if (fscanf(pFile, "%d,%f ", &dataIdx, &dataValue) > 0) {

					(*ppFeatureOut)->GetData().push_back(std::make_pair(dataIdx, dataValue));

					continue;
				}
				else {
					goto exitFileWrong;
				}
			}
			if (fscanf(pFile, "\n") < 0) goto exitFileWrong;
		}
		else {
			goto exitFileWrong;
		}

		return 0;

	exitFileWrong:
		if (*ppFeatureOut) delete *ppFeatureOut;
		*ppFeatureOut = NULL;
		fprintf(stderr, "TxtFeature::LoadFromFile file wrong.\n");
		return -1;
	}
}
