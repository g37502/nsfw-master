#include <sys/types.h>
#include <dirent.h>
#include <vector>
#include <string.h>
#include <sys/stat.h>
#include <istream>

#include "TxtFilter.h"
#include "TxtSampleSet.h"
#include "TxtFeatureSet.h"
#include "TxtFeatureExtraction.h"
#include "TxtClassifier.h"
#include "TxtWebPageUtils.h"
#include "StrUtils.h"

namespace TxtFilter {

#ifndef min
#define min(x, y) (x > y ? y : x)
#endif

#ifndef max
#define max(x, y) (x > y ? x : y)
#endif

	TxtFeatureExtraction *g_pExtractor = NULL;
	TxtSampleSet *g_pSampleSet = NULL;
	TxtClassifier *g_pClassifier = NULL;

	int Initialize(const char* termFilename, const char* modelFilename) {

		if (NULL == termFilename || NULL == modelFilename) {

			fprintf(stderr, "Initialize: termFilename or modelFilename cannot be NULL.\n");
			return -1;
		}

		if (g_pExtractor) delete g_pExtractor;
		g_pExtractor = new TxtFeatureExtraction();

		if (0 != g_pExtractor->InitializeFromKeywords(termFilename)) {

			fprintf(stderr, "Initialize: cannot initialize feature extractor.\n");
			Uninitialize();
			return -1;
		}

		if (g_pSampleSet) delete g_pSampleSet;
		g_pSampleSet = new TxtSampleSet(g_pExtractor, 2); // 2-class problem

		FILE *pModelFile = fopen(modelFilename, "r");
		if (!pModelFile) {

			fprintf(stderr, "Initialize: model file cannot open.\n");
			return -1;
		}

		if (0 != g_pSampleSet->LoadFromFile(pModelFile)) {

			fprintf(stderr, "Initialize: cannot initialize sample set.\n");
			Uninitialize();
			fclose(pModelFile);
			return -1;
		}
		fclose(pModelFile);

		if (g_pClassifier) delete g_pClassifier;
		g_pClassifier = new TxtClassifier(g_pSampleSet);

		WebPageUtils::Initialize();

		return 0;
	}

	int Process(unsigned char * inputStr, int nSizeStr) {

		if (inputStr == NULL || nSizeStr <= 0) {

			fprintf(stderr, "TxtFilter::Process: input inputStr or nSizeStr cannot be NULL.\n");
			return -1;
		}

		if (g_pSampleSet == NULL) {

			fprintf(stderr, "TxtFilter::Process: not initialized.\n");
			return -2;
		}

		unsigned char *cleanStr = new unsigned char[nSizeStr + 1];
		unsigned char *strToProcess = NULL;
		memset(cleanStr, 0, nSizeStr + 1);

		if (0 == WebPageUtils::ExtractBody((char *)inputStr, (char*)cleanStr, true)) {

			strToProcess = cleanStr;
		}
		else {

			strToProcess = inputStr;
		}

		TxtFeature *pFeatureX = NULL;

		if (0 != g_pExtractor->ProcessUTF8Sample((unsigned char *)strToProcess, nSizeStr + 1, &pFeatureX, false)) {

			fprintf(stderr, "TxtFilter::ProcessStream: cannot extract feature from stream.\n");

			delete [] cleanStr;
			return -2;
		}

		double score = g_pSampleSet->GetClassifyScore(pFeatureX, TxtDot, g_pSampleSet->GetFeatureSet(0)->Count() / 2);

		delete pFeatureX;
		delete [] cleanStr;


		return min(max((int)(score + 0.5), 0), 100);
	}

	int ProcessUTF8(std::istream &inputStream) {

		if (!g_pExtractor || !g_pSampleSet || !g_pClassifier) {
			fprintf(stderr, "TxtFilter::ProcessStream: not initialized.\n");
			return -1;
		}

		int streamSize = inputStream.tellg();
		inputStream.seekg(0, std::ios::end);
		streamSize = static_cast<int>(inputStream.tellg()) - streamSize;
		inputStream.seekg(0, std::ios::beg);

		char *buf = new char[streamSize + 1];
		inputStream.read(buf, streamSize);
		buf[streamSize] = '\0';

		wchar_t *wcleanStr = new wchar_t[streamSize * 10 + 5];
		memset(wcleanStr, 0, streamSize * 10 + 5);

		StrUtils::UTF82W(buf, wcleanStr, streamSize * 10 + 4);//clean the warning

		if (StrUtils::ChineseCharNum(wcleanStr) <= 140) { // skip the small samples
			delete [] buf;
			delete [] wcleanStr;
			return 0;
		}

		int resProcess = Process((unsigned char*)buf, streamSize + 1);

		delete [] buf;
		delete [] wcleanStr;

		return resProcess;
	}

	int ProcessUTF8Buffer(unsigned char* lpszContent,int nLength) {

		if (!g_pExtractor || !g_pSampleSet || !g_pClassifier) {
			fprintf(stderr, "TxtFilter::ProcessUTF8Buffer: not initialized.\n");
			return -1;
		}
		if(!lpszContent)
		{
			fprintf(stderr, "TxtFilter::ProcessUTF8Buffer: lpszContent invalid.\n");
			return -1;
		}
		if(nLength <= 0)
		{
			fprintf(stderr, "TxtFilter::ProcessUTF8Buffer: nLength invalid.\n");
			return -1;
		}
		
		char *buf = new char[nLength + 1];
		if(NULL == buf)
		{
			fprintf(stderr, "TxtFilter::ProcessUTF8Buffer: malloc error.\n");
			return -1;
		}
		memcpy(buf, lpszContent, nLength);
		buf[nLength] = '\0';
		
		wchar_t *wcleanStr = new wchar_t[nLength * 10 + 5];
		if(NULL == wcleanStr)
		{
			fprintf(stderr, "TxtFilter::ProcessUTF8Buffer: malloc error.\n");
			delete [] buf;
			return -1;
		}
		memset(wcleanStr, 0, nLength * 10 + 5);

		StrUtils::UTF82W((const char*)buf, wcleanStr, nLength * 10 + 4);

		if (StrUtils::ChineseCharNum(wcleanStr) <= 140) { // skip the small samples
			delete [] buf;
			delete [] wcleanStr;
			return 0;
		}

		int resProcess = Process((unsigned char*)buf, nLength + 1);

		delete [] buf;
		delete [] wcleanStr;

		return resProcess;
	}

	int ProcessWstr(wchar_t* inStr, int nSizeStr) {

		if (inStr == NULL || nSizeStr <= 0) {

			fprintf(stderr, "TxtFilter::ProcessWstr: input inStr or nSizeStr cannot be NULL.\n");
			return -1;
		}

		if (StrUtils::ChineseCharNum(inStr) <= 140) { // skip the small samples

			return 0;
		}

		unsigned char * buf = new unsigned char[nSizeStr + 1];
		memset(buf, 0, nSizeStr + 1);

		StrUtils::W2UTF8(inStr, nSizeStr, (char*)buf, nSizeStr);

		int resProcess = Process(buf, nSizeStr + 1);

		delete [] buf;

		return resProcess;
	}

	int Uninitialize() {

		if (g_pExtractor) delete g_pExtractor;

		if (g_pSampleSet) delete g_pSampleSet;

		if (g_pClassifier) delete g_pClassifier;

		WebPageUtils::Uninitialize();

		return 0;
	}

	int TrainModel(const char* termFilename, const char* modelFilename, char *sampleFolders[]) {

		if (NULL == termFilename || NULL == modelFilename || NULL == sampleFolders) {
			fprintf(stderr, "TxtFilter::TrainModel: input params cannot be NULL\n");
			return -1;
		}

		TxtFeatureExtraction *pExtractor = new TxtFeatureExtraction();

		if (0 != pExtractor->InitializeFromKeywords(termFilename)) {
			fprintf(stderr, "TxtFilter::TrainModel: terms file wrong.\n");
			delete pExtractor;
			return -2;
		}

		TxtSampleSet *pSampleSet = new TxtSampleSet(pExtractor);

		int idxFolder = 0;

		WebPageUtils::Initialize();

		while (sampleFolders[idxFolder]) {

			DIR *pFolder = opendir(sampleFolders[idxFolder]);
			std::vector<std::string> fileList;
			int fileCount = 0;

			if (pFolder) {
				struct dirent *dir;
				while ((dir = readdir(pFolder)) != NULL) {
					if (DT_REG == dir->d_type) { // only read top level files
						fileList.push_back(dir->d_name);
						fileCount ++;
					}
				}
			}
			else {
				fprintf(stderr, "TxtFilter::TrainModel: wrong folder names.\n");
				return -3;
			}

			closedir(pFolder);

			printf("Process folder %s ...\n", sampleFolders[idxFolder]);

			for (int idx = 0; idx < fileCount; idx ++) {
				char buf[512] = {0};
				strcpy(buf, sampleFolders[idxFolder]);
				strcat(buf, "/");
				strcat(buf, fileList[idx].c_str());
				FILE *pFile = fopen(buf, "r");
				long long fileSize = 0;
				struct stat fileStat;

				if (stat(buf, &fileStat) >= 0) {

					fileSize = fileStat.st_size;
				}


				if (pFile && fileSize > 0) {

					printf("Processing file %s ...\n", buf);

					char *buf = new char[fileSize + 1];

					if (fread(buf, fileSize, 1, pFile) <= 0) {
						fprintf(stderr, "TxtFilter::TrainModel: cannot read file.\n");
						return -4;
					}

					buf[fileSize] = '\0';

					char *strToProcess = NULL;
					char *cleanStr = new char[fileSize + 1];
					memset(cleanStr, 0, fileSize + 1);

					if (0 == WebPageUtils::ExtractBody(buf, cleanStr, true)) {

						strToProcess = cleanStr;
					}
					else {

						strToProcess = buf;
					}

					pSampleSet->AddSample((unsigned char*)strToProcess, fileSize, idxFolder);

					delete [] cleanStr;
					delete [] buf;

					fclose(pFile);
				}
			}

			idxFolder ++;
		}

		// divide DF
		for (int idx1 = 0; idx1 < pSampleSet->GetNumTags(); idx1 ++) {

			for (int idx2 = 0; idx2 < pSampleSet->GetFeatureSet(idx1)->Count(); idx2 ++) {

				TxtFeature *pFeature = pSampleSet->GetFeatureSet(idx1)->GetFeature(idx2);

				if (pFeature) {

					TxtFeatureArrayType pData = pFeature->GetData();
					int nData = pData.size();

					if (nData) {

						for (int idx3 = 0; idx3 < nData; idx3 ++) {

							if (pData[idx3].second > 0) {

								int df = pExtractor->GetDF(pData[idx3].first);
								if (df <= 0) {
									fprintf(stderr, "Unexpected error!.\n");
									return -5;
								}

								pFeature->SetValue(pData[idx3].first, pData[idx3].second /= (double)df);
							}
						}
					}
				}
			}
		}

		FILE *modelFile = fopen(modelFilename, "w");

		pSampleSet->SaveToFile(modelFile);

		fclose(modelFile);

		WebPageUtils::Uninitialize();

		delete pSampleSet;
		delete pExtractor;

		return 0;
	}
}
