#include "Rand.h"
std::mutex Random::mut;
std::mt19937 Random::rng = std::mt19937(std::random_device()());