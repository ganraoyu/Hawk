#pragma once
#include <chrono>

class Timer
{
public:
    Timer() : start(std::chrono::high_resolution_clock::now()) {}

    long long elapsedMicroseconds()
    {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    }

    long long elapsedNanoseconds()
    {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    }

private:
    std::chrono::high_resolution_clock::time_point start;
};
