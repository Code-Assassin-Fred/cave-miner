/// @file examples.cpp
/// @brief Demonstration of the RNG141 utility class.
/// @brief In the console, compile with "make examples" 
///        In the console, then execute "make run_examples"

#include <iostream>
#include <iomanip> // For formatting float output
#include "cs141random.h"

using std::cout;
using std::endl;

int main()
{
    // Instantiate the random generator. 
    // The first time this is created, it automatically seeds the shared static engine.
    RNG141 rng;
    
    cout << "--- CS 141 Random Number Generator Demo ---" << endl << endl;

    // 1. Integer Generation
    cout << "Generating 5 random integers (e.g. 6-sided die, 1 to 6):" << endl;
    for (int i = 0; i < 5; i++)
    {
        // Calls the int version of getRandom
        int roll = rng.getRandom(1, 6);
        cout << "Roll " << (i + 1) << ": " << roll << endl;
    }
    
    cout << endl;

    // 2. Floating-Point Generation
    cout << "Generating 3 random floats (e.g. Raylib X-coordinates, 0.0 to 960.0):" << endl;
    cout << std::fixed << std::setprecision(2); // Format output to 2 decimal places
    
    for (int i = 0; i < 3; i++)
    {
        // Adding 'f' specifies these as floats, triggering the overloaded float version
        float xCoord = rng.getRandom(0.0f, 960.0f); 
        cout << "X Pos " << (i + 1) << ": " << xCoord << endl;
    }

    cout << endl;

    // 3. Demonstrating Shared Engine State
    // Even if a second obect is created locally, it draws from the 
    // exact same continuously advancing static engine sequence.
    RNG141 localRng; 
    
    cout << "Generating a random enemy speed using a secondary RNG object:" << endl;
    float speed = localRng.getRandom(1.5f, 4.5f);
    cout << "Enemy Speed: " << speed << endl;

    return 0;
}