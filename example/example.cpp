//
// Created by qiming on 25-4-11.
//

#include "instance.h"   // Header file for loading job shop instance data
#include "tabu_search.h"// Header file for Tabu Search algorithm implementation
#include <chrono>       // Header for high-resolution clock and timing utilities

using namespace std::chrono;// For easier access to chrono components

int main() {
    // Maximum running time in seconds
    int limit_time = 100;

    // Known best makespan for the instance (used as a stopping condition)
    int best_answer = 930;

    // Maximum number of iterations allowed for Tabu Search
    unsigned long long max_iteration = 100000000;

    // Seed for the random number generator to ensure reproducibility
    int rand_seed = 2001;

    // Path to the job shop instance file (FT10)
    const std::string instance_path = R"(../../instance/ft/ft10.txt)";

    // Load the instance data from file
    Instance instance = load_instance(instance_path);

    // Set the random number generator seed
    RandomGenerator::instance().setSeed(rand_seed);

    // Start measuring execution time
    auto start = std::chrono::high_resolution_clock::now();

    // Initialize the Tabu Search solver with the instance
    auto tabuSearch = TabuSearch(instance);

    // Compute the stopping time point by adding the limit to the current time
    steady_clock::time_point endTime = steady_clock::now() + seconds(limit_time);

    // Run the Tabu Search algorithm
    // It will stop if either the time limit is reached or the best known solution is found
    tabuSearch.search(max_iteration,
                      [&]() {
                          return duration_cast<milliseconds>(endTime - steady_clock::now()).count() <= 0 || tabuSearch.makespan() == best_answer;
                      });

    // Measure end time after search completes
    auto end = std::chrono::high_resolution_clock::now();

    // Calculate total execution duration in milliseconds
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // Log execution time to standard error (usually shown on console)
    std::clog << "Function took " << duration.count() << " milliseconds to execute." << std::endl;

    // Log the number of iterations performed
    std::clog << "Iteration: " << tabuSearch.get_iter() << std::endl;

    // Log the best makespan found
    std::clog << "Best makespan: " << tabuSearch.makespan() << std::endl;

    // Export the result to a CSV file
    tabuSearch.export_result("../../output/result.csv");

    // Exit the program
    return 0;
}
