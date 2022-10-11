#pragma once

#include <vector>
#include <string>

namespace Utils {
	bool ExtractParamByArgName(const std::string& filePath, const std::string& argName, std::string& outParamVal);
	void ReplaceAllOcurrences(std::string& str, const std::string& toFind, const std::string& replaceWith);
	void explode(std::string const& s, char delim, std::vector<std::string>& outExplode);
	std::vector<std::string> explode(std::string const& s, char delim);
	std::string implode(const std::vector<std::string>& s, char delim);
}

