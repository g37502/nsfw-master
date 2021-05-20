#pragma once
#include <stdio.h>
#include <vector>
#include <utility>

namespace TxtFilter
{
	typedef float TxtFeatureDataType;
	typedef std::vector<std::pair<int, TxtFeatureDataType> > TxtFeatureArrayType;
	class TxtFeature;
	typedef TxtFeatureDataType (*TxtSimFunc)(TxtFeature *, TxtFeature *);

	class TxtFeature
	{
	public:
		TxtFeature();
		virtual ~TxtFeature();

	public:
		int GetDim();
		TxtFeatureArrayType & GetData();
		int GetValue(int idx);
		int SetValue(int idx, TxtFeatureDataType value);
		int IncValue(int idx, TxtFeatureDataType value);
		double Norm();
		int SaveToFile(FILE *pFile);
		static int LoadFromFile(FILE *pFile, TxtFeature **ppFeatureOut);

	public:
		TxtFeatureArrayType m_data;
		int m_dim;
		double m_norm;
	};
}
