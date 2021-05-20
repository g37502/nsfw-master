#pragma once
#include "TxtFeature.h"

namespace TxtFilter {

	TxtFeatureDataType TxtDot(TxtFeature *pFeature1, TxtFeature *pFeature2);

	struct TxtClassResult {
		TxtClassResult() { }
		TxtClassResult(double aScore, int aClassTag) {
			score = aScore;
			classTag = aClassTag;
		}

		double score;
		int classTag;
	};

	class TxtSampleSet;

	class TxtClassifier {
	public:
		TxtClassifier(TxtSampleSet* pSampleSet);
		virtual ~TxtClassifier();

	public:
		int Process(TxtFeature *pFeatureX, TxtClassResult **ppClassResult, int *pNumClasses);

	private:
		TxtSampleSet* m_pSampleSet;
	};

}
