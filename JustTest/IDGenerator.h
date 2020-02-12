#pragma once
#include <random>

struct IDGenerator {
    std::random_device r;
    std::default_random_engine generator;
    std::uniform_int_distribution<unsigned long long> distribution;

    IDGenerator() {
        generator = std::default_random_engine(r());
        distribution = std::uniform_int_distribution<unsigned long long>(0, ULLONG_MAX);
    }

    unsigned long long getID() {
        return distribution(generator);
    }
    
}id_generator;