#pragma once
#include <istream>
#include <string.h>
#include "TxtFeature.h"
#include "TxtFeatureExtraction.h"

namespace TxtFilter
{
/*
 * Initialize the TxtFilter, MUST be called before calling ProcessUTF8 or ProcessWstr.
 * DO NOT need to be called before calling TrainModel.
 *
 * Param: termFilename provides keywords for trained "modelFilename"
 * Param: modelFilename contains the path the name of the trained model file.
 * Return: 0 if successed. Otherwise returns an error code (negative value)
 */
int Initialize(const char* termFilename, const char* modelFilename);

/*
 * Process UTF-8 encoding text stream
 *
 * Param: inStream is in UTF-8 encoding
 * Return: int value indicates the score, range from 0 to 100.
 *                 0 indicates the inStream is clean.
 */
int ProcessUTF8(std::istream &inStream);

/*
 * try to Process all encoding text buffer.
 *
 * Param: lpszContent content text
 * Param: nLength sizeof lpszContent.
 * Return: int value indicates the score, range from 0 to 100.
 *                 0 indicates the inStream is clean.
 */
int ProcessUTF8Buffer(unsigned char* lpszContent,int nLength);

/*
 * Process Unicode text
 *
 * Param: inStr is a string with wide char
 * Param: nSizeStr is the size of inStr in bytes
 * Return: int value indicates the score, range from 0 to 100.
 *                 0 indicates the inStr is clean.
 */
int ProcessWstr(wchar_t* inStr, int nSizeStr);

/*
 * Uninitialized() is for clean up the memory. Call this when program doesn't use TxtFilter anymore.
 */
int Uninitialize();

/*
 * Train a model for processing.
 *
 * Param: termFIlename provides the keywords for training
 * Param: modelFIlename indicates the path and name you want the model to be saved.
 * Param: SampleFolders provides the sample folders
 * Return: 0 if successed. Otherwise return an error code (negative value).
 */
int TrainModel(const char* termFilename, const char* modelFilename, char *sampleFolders[]);

}
