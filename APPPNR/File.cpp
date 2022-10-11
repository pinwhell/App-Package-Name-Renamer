#include "File.h"
#include "Buffer.h"
#include "FileHelper.h"

#define IN_RANGE(min, x, max) ((x) >= (min) && (x) <= (max))

File::File()
	: pBuff(nullptr)
	, filePathStr("")
{}

File::~File()
{
	if (pBuff) delete pBuff;
}

bool File::ValidOffset(uint64_t offset) const
{
	return pBuff && IN_RANGE(0, offset, pBuff->mSize);
}

bool File::ApplyChanges()
{
	return FileHelper::WriteFile(filePathStr, *pBuff);
}

bool File::OverrideWithString(const std::string& toOverride)
{
	delete pBuff;

	pBuff = new Buffer(toOverride);

	return ApplyChanges();
}
