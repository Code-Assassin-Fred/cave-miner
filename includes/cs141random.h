/// @file cs141random.h
/// @author Adam T Koehler, PhD
/// @date October 12, 2026
/// @brief A generalized random number generator utility for the group project.
//         Requires C++17.

#pragma once

#include <random>

class RNG141
{
    private:
        // Shared Class-Level RNG State 
        inline static std::mt19937 rng;
        inline static bool isSeeded = false;

        // Private seed functions
        static void seedRNG();
        static void seedRNG(int);

    public:
        // Constructors
        RNG141();
        explicit RNG141(int seedValue);

        // Core Generation (Overloaded for ints and floats)
        int getRandom(int min, int max);
        float getRandom(float min, float max);

        // Prevention of copying the generator interface
        RNG141(const RNG141&) = delete;
        RNG141& operator=(const RNG141&) = delete;
};