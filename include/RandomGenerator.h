//
// Created by qiming on 25-4-7.
//
// File: random_generator.h
// Description: Thread-safe singleton implementation of a random number generator
//              supporting multiple distribution types for simulation purposes
//

#ifndef RANDOM_GENERATOR_H
#define RANDOM_GENERATOR_H

#include <mutex>
#include <random>

/**
 * Thread-safe random number generator implementing the singleton pattern.
 * Supports multiple distribution types:
 * - Uniform integer
 * - Uniform real
 * - Boolean
 * - Normal distribution
 */
class RandomGenerator {
public:
    /**
     * Get the singleton instance (thread-safe initialization)
     * @return Reference to the single RandomGenerator instance
     */
    static RandomGenerator &instance() {
        static RandomGenerator instance;
        return instance;
    }

    /**
     * Set the random number generator seed
     * @param seed New seed value
     */
    void setSeed(unsigned seed) {
        std::lock_guard<std::mutex> lock(mtx);
        gen.seed(seed);
    }

    // Delete copy constructor and assignment operator
    RandomGenerator(const RandomGenerator &) = delete;
    RandomGenerator &operator=(const RandomGenerator &) = delete;

    /**
     * Generate random integer in [min, max] (inclusive)
     * @param min Minimum value
     * @param max Maximum value
     * @return Random integer
     */
    int getInt(int min, int max) {
        std::lock_guard<std::mutex> lock(mtx);
        std::uniform_int_distribution<int> dist(min, max);
        return dist(gen);
    }

    /**
     * Generate random integer in [0, max) (exclusive)
     * @param max Upper bound (exclusive)
     * @return Random integer
     */
    int getInt(int max) {
        return getInt(0, max - 1);
    }

    /**
     * Generate random double in [min, max) (exclusive)
     * @param min Minimum value
     * @param max Maximum value (exclusive)
     * @return Random double
     */
    double getDouble(double min, double max) {
        std::lock_guard<std::mutex> lock(mtx);
        std::uniform_real_distribution<double> dist(min, max);
        return dist(gen);
    }

    /**
     * Generate random double in [0, max) (exclusive)
     * @param max Upper bound (exclusive)
     * @return Random double
     */
    [[maybe_unused]]
    double getDouble(double max) {
        return getDouble(0, max);
    }

    /**
     * Generate random boolean
     * @return True or false
     */
    [[maybe_unused]]
    bool getBool() {
        return getInt(0, 1) == 1;
    }

    /**
     * Generate random number from normal distribution
     * @param mean Distribution mean
     * @param stddev Standard deviation
     * @return Normally distributed random number
     */
    [[maybe_unused]]
    double getNormal(double mean = 0.0, double stddev = 1.0) {
        std::lock_guard<std::mutex> lock(mtx);
        std::normal_distribution<double> dist(mean, stddev);
        return dist(gen);
    }

private:
    // Private constructor using random_device for initial seed
    RandomGenerator() : gen(std::random_device{}()) {}

    std::mt19937 gen;// Mersenne Twister engine (32-bit)
    std::mutex mtx;  // Mutex for thread safety
};

#endif// RANDOM_GENERATOR_H
