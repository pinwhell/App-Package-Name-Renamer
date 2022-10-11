#pragma once

#include <cstdint>
#include <string>

struct Buffer {
    unsigned char* mBuff;
    uint64_t mSize;

    Buffer();
    Buffer(const std::string& str);
    ~Buffer();
};