#include "Utils.h"
#include "FilesEngine.h"
#include <regex>
#include <sstream>


bool Utils::ExtractParamByArgName(const std::string& filePath, const std::string& argName, std::string& outParamVal)
{
	FilesEngine fe;
	File* pOutFile = nullptr;
	bool bResult = false;

	if (fe.LoadFile(filePath, &pOutFile))
	{
		const char* pFileEntry = (const char*)(pOutFile->pBuff->mBuff);
		const char* pArgNameBeg = strstr(pFileEntry, argName.c_str());

		if (pArgNameBeg)
		{
			pArgNameBeg += argName.size();
			enum class StringMark {
				NONE = -1,
				SINGLE_MARK,
				DOUBLE_MARK
			};

			StringMark strMark = StringMark::NONE;

			outParamVal = "";

			for (const char* c = pArgNameBeg; *c != '\0'; c++)
			{
				switch (*c)
				{
				case ' ':
					if (strMark == StringMark::NONE)
						continue;
					else outParamVal += ' ';
					break;

				case '\'':
				case '\"':
				{
					if (strMark == StringMark::NONE)
					{
						switch (*c)
						{
						case '\'':
							strMark = StringMark::SINGLE_MARK;
							break;
						case '\"':
							strMark = StringMark::DOUBLE_MARK;
							break;
						}

						continue;
					}
					else {
						switch (strMark)
						{
						case StringMark::SINGLE_MARK:
							if (*c == '\'')
								goto DONE;
							break;

						case StringMark::DOUBLE_MARK:
							if (*c == '\"')
								goto DONE;
							break;
						}
					}
					// appgradle '"Hello """'
				}

				default:
					outParamVal += *c;
				}
			}

		DONE:
			bResult = true;
		}
	}

	return bResult;
}

void Utils::ReplaceAllOcurrences(std::string& str, const std::string& toFind, const std::string& replaceWith)
{
	size_t index = std::string::npos;

	while ((index = str.find(toFind)) != std::string::npos) {    //for each location where Hello is found
		str.replace(index, toFind.length(), replaceWith); //remove and replace from that position
	}
}

void Utils::explode(std::string const& s, char delim, std::vector<std::string>& outExplode)
{
	outExplode = std::vector<std::string>();
	std::istringstream iss(s);

	for (std::string token; std::getline(iss, token, delim); )
		outExplode.push_back(token);
}

std::vector<std::string> Utils::explode(std::string const& s, char delim)
{
	std::vector<std::string> result = std::vector<std::string>();

	explode(s, delim, result);

	return result;
}

std::string Utils::implode(const std::vector<std::string>& s, char delim)
{
	std::string result = "";

	for (int i = 0; i < s.size(); i++)
	{
		result += s[i];

		if ((i + 1) != s.size())
			result += delim;
	}

	return result;
}
