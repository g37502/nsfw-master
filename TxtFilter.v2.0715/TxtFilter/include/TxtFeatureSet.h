#pragma once
#include <vector>
#include <stdio.h>
#include "TxtClassifier.h"

namespace TxtFilter {

	class TxtFeature;

	class TxtFeatureSet {
	public:
		TxtFeatureSet();
		virtual ~TxtFeatureSet();

	public:
		int AddFeature(TxtFeature *pFeature);
		TxtFeature* GetFeature(unsigned int idx);
		int Count();
		int SaveToFile(FILE *pFile);
		int LoadFromFile(FILE *pFile);
		double GetSimScore(TxtFeature *pFeature, TxtSimFunc pSimFunc);
		void SetTag(int tag);

	private:
		int m_tag;
		std::vector<TxtFeature*> m_featureList;
	};

}
