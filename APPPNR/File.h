#pragma once

#include <string>
#include "Buffer.h"

struct File {
	std::string filePathStr;
	Buffer* pBuff;

	File();
	~File();

	bool ValidOffset(uint64_t offset) const;
	bool ApplyChanges();

	bool OverrideWithString(const std::string& toOverride);
};
