#include <iostream>
#include <stdio.h>
#include <wchar.h>
#include "TxtFilter.h"
using namespace std;

void printUsage() {
	printf("\nUsage: TxtFilterTrain input_terms_file output_model_file input_sample_dir1 intput_sample_dir2 ...\n");
}

int main(int argc, char *argv[]) {

	if (argc == 1) {
		printUsage();
		return -1;
	}

	char** dirs = new char*[argc - 2];
	for (int idx = 3; idx < argc; idx ++) {
		dirs[idx - 3] = argv[idx];
	}
	dirs[argc - 3] = NULL;

	TxtFilter::TrainModel(argv[1], argv[2], dirs);

	delete [] dirs;

	return 0;
}
