#include <stdio.h>
#include <algorithm>
#include "TxtFeature.h"
#include "TxtFeatureSet.h"
#include "TxtSampleSet.h"
#include "TxtClassifier.h"

namespace TxtFilter {

	inline TxtFeatureDataType TxtDot(TxtFeature *pFeature1, TxtFeature *pFeature2) {
		if (NULL == pFeature1 || NULL == pFeature2) {
			fprintf(stderr, "TxtDot: pFeature1 and pFeature2 cannot be NULL.\n");
			return -1;
		}

		if (pFeature1->GetDim() != pFeature2->GetDim()) {
			fprintf(stderr, "TxtDot: pFeature1 and pFeature2 have different dimensions.\n");
			return -1;
		}

		TxtFeatureDataType score = 0;
		TxtFeatureArrayType data1 = pFeature1->GetData();
		TxtFeatureArrayType data2 = pFeature2->GetData();

		int count1 = data1.size();
		int count2 = data2.size();
		int idx1 = 0;
		int idx2 = 0;

		while (idx1 < count1 && idx2 < count2) {

			if (data1[idx1].first < data2[idx2].first) {

				idx1 ++;
			}
			else if (data1[idx1].first > data2[idx2].first) {

				idx2 ++;
			}
			else {

				score += data1[idx1].second * data2[idx2].second;
				idx1 ++;
				idx2 ++;
			}
		}

		return score;
	}

	TxtClassifier::TxtClassifier(TxtSampleSet* pSampleSet) :
	m_pSampleSet(pSampleSet) {
		// TODO Auto-generated constructor stub

	}

	TxtClassifier::~TxtClassifier() {
		// TODO Auto-generated destructor stub
	}

	int TxtClassifier::Process(TxtFeature *pFeatureX, TxtClassResult **ppClassResult, int *pNumClasses) {
		if (NULL == pFeatureX || NULL == ppClassResult || NULL == pNumClasses) {
			fprintf(stderr, "TxtClassifier::Process: pFeatureX and ppClassResult cannot be NULL.\n");
			return -1;
		}

		*pNumClasses = m_pSampleSet->GetNumTags();
		if (*pNumClasses <= 0 || *pNumClasses > MAX_SAMPLETAG) {
			fprintf(stderr, "TxtClassifier::Process: wrong classes number.\n");
			return -1;
		}

		(*ppClassResult) = new TxtClassResult[*pNumClasses];
		for (int idx = 0; idx < *pNumClasses; idx ++) {
			(*ppClassResult)[idx].score = 0;
			(*ppClassResult)[idx].classTag = idx;
		}

		if (pFeatureX->Norm() == 0) return 0;

		for (int idx = 0; idx < *pNumClasses; idx ++) {
			TxtFeatureSet *pFeatureSet = m_pSampleSet->GetFeatureSet(idx);
			(*ppClassResult)[idx].score = pFeatureSet->GetSimScore(pFeatureX, TxtDot);
		}
		return 0;
	}

}
