/// @file cs141random.cpp
/// @brief Implementation of the RNG141 class for group project.
//         Requires C++17.


#include <random>
#include "cs141random.h"

using std::random_device;
using std::seed_seq;
using std::uniform_int_distribution;
using std::uniform_real_distribution;

RNG141::RNG141()
{
    if (!isSeeded) 
    {
        seedRNG();
    }
}

RNG141::RNG141(int seedValue)  
{
    // If a specific seed is provided, force the sequence reset
    seedRNG(seedValue);
}

int RNG141::getRandom(int min, int max) 
{
    // Generating the distribution locally allows dynamic ranges 
    // on every call while safely reusing the static engine.
    uniform_int_distribution<int> dist(min, max);
    return dist(rng);  
}

float RNG141::getRandom(float min, float max) 
{
    // uniform_real_distribution generates a float in the range [min, max)
    uniform_real_distribution<float> dist(min, max);
    return dist(rng);  
}

void RNG141::seedRNG() 
{
    random_device rd;
    
    // Stretch the entropy using 8 32-bit values 
    seed_seq seq{rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd()};
    rng.seed(seq);
    
    isSeeded = true;
}

void RNG141::seedRNG(int seedValue) 
{
    rng.seed(seedValue);
    isSeeded = true;
}