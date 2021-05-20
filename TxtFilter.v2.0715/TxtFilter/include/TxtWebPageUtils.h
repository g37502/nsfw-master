#pragma once

namespace TxtFilter {
	namespace WebPageUtils {

	int Initialize();
	int Uninitialize();
	int ExtractBody(const char *strInput, char *strOutput, bool removeLinkText);

	}
}
