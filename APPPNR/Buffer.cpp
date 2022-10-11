#include "Buffer.h"

Buffer::Buffer()
    : mBuff(nullptr)
    , mSize(0)
{}

Buffer::Buffer(const std::string & str)
{
    mBuff = new unsigned char[str.size()];
    mSize = str.size();

    memcpy(mBuff, str.data(), mSize);
}

Buffer::~Buffer()
{
    if (mBuff)
        delete[] mBuff;
}
