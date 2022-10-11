#pragma once

#include <cstdint>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string>

class RandomHelper{
    public:
    static inline void InitRandomization()
    {
        srand(time(NULL));
    }

    static inline uint32_t getRandomKey()
    {
        return rand();
    }

    static inline uint32_t getRandomInBtw(int from, int to)
    {
        int d = abs(to - from);
        return (from + (getRandomKey() % d));
    }

    static inline std::string getRandomString(size_t len)
    {
        std::string abcdary = std::string("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ012345678");
        size_t abcdaryLen = abcdary.size();
        std::string result = "";

        for(size_t i = 0; i < len; i++)
            result.push_back(abcdary[getRandomInBtw(0, abcdaryLen - 1)]);

        return result;
    }

    static inline char getRandomVucal()
    {
        std::string vocals = std::string("aeiou");

        return vocals[getRandomInBtw(0, vocals.size() - 1)];
    }

    static inline char getRandomBcdry()
    {
        std::string bcdry = std::string("bcdfghjklmnpqrtvwxyz");

        return bcdry[getRandomInBtw(0, bcdry.size() - 1)];
    }

    static inline std::string getRandomSenseString(size_t len)
    {
        enum class CharType {
            VUCAL,
            BCDARY
        };

        CharType cType = (CharType)(getRandomInBtw(0, 100) > 50);
        std::string result = "";

        for (size_t i = 0; i < len; i++)
        {
            switch (cType)
            {
            case CharType::VUCAL:
                result += getRandomVucal();
                break;

            case CharType::BCDARY:
                result += getRandomBcdry();
                break;
            }

            cType = (CharType)(!(int)cType);
        }

        return result;
    }
};