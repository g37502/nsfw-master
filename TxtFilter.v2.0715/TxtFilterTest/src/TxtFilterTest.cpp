//============================================================================
// Name        : TxtFilterTest.cpp
// Author      : 
// Version     :
// Copyright   :
// Description :
//============================================================================

#include <iostream>
#include <fstream>
#include <ios>
#include <string.h>
#include <vector>
#include <stdio.h>
#include <dirent.h>
#include <time.h>

#include "TxtFilter.h"
#include "TxtWebPageUtils.h"

using namespace std;

void printUsage() {
	printf("\nUsage: TxtFilterTest input_terms_file input_model_file input_samples_folder\n\n");
}

int main(int argc, char *argv[]) {

	if (argc != 4) {
		printUsage();
		return -1;
	}

	clock_t start = clock();

	TxtFilter::Initialize(argv[1], argv[2]);

	//std::ifstream file("//home//pebai//workspace//3-20000//653828518.wml");
	//std::ifstream file("//home//pebai//workspace//3-20000//150748790.php");
	//TxtFilter::ProcessUTF8(file);

	// DIR *pFolder = opendir(argv[3]);
	// std::vector<std::string> fileList;
	// int fileCount = 0;


	// if (pFolder) {
	// 	struct dirent *dir;
	// 	while ((dir = readdir(pFolder)) != NULL) {
	// 		if (DT_REG == dir->d_type) { // only read top level files
	// 			fileList.push_back(dir->d_name);
	// 			fileCount ++;
	// 		}
	// 	}
	// }
	// else {
	// 	fprintf(stderr, "TxtFilterTest: wrong folder names.\n");
    //     closedir(pFolder);
    //     TxtFilter::Uninitialize();
	// 	return -3;
	// }

	// closedir(pFolder);

	// printf("Process folder %s ...\n", argv[3]);

	// for (int idx = 0; idx < fileCount; idx ++) {
	// 	char buf[512] = {0};
	// 	strcpy(buf, argv[3]);
	// 	strcat(buf, "/");
	// 	strcat(buf, fileList[idx].c_str());
	// 	std::ifstream file(buf);

	// 	int score = TxtFilter::ProcessUTF8(file);
	// 	printf("%s, %d\n", buf, score);
	// 	if (idx % 100 == 0) {
	// 		fprintf(stderr, "%d\n", idx);
	// 	}
	// }
	std::ifstream file(argv[3]);	
	int score = TxtFilter::ProcessUTF8(file);
    printf(" %d\n",  score);
	//fprintf('%d',score)
	TxtFilter::Uninitialize();

	clock_t end = clock();

	fprintf(stderr, "Processing time cost: %d seconds\n", (end - start) / 1000000);

	return score;
}
