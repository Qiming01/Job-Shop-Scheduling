# Tabu Search for Job Shop Scheduling Problem

**Job Shop Scheduling Problem (JSSP)** is a classic combinatorial optimization problem in operations research and computer science. It involves scheduling a set of jobs, where each job consists of a sequence of tasks that must be processed in a specific order on different machines. Each task requires a specific machine for a certain amount of time, and no machine can handle more than one task at a time.

The main goal is to determine a schedule — assigning start times to all tasks — such that the overall completion time (called the **makespan**) is minimized, while satisfying all constraints:

- **Precedence constraints**: Tasks within the same job must be processed in the given order.
- **Resource constraints**: A machine can process only one task at a time, and a task cannot be interrupted once started.

JSSP is known to be **NP-hard**, meaning it is computationally challenging to solve optimally for large instances. It has wide applications in manufacturing, production planning, and service systems.

------

## Benchmark

| Instance | n×m   | 最优解      | Best  | Avg    | Worst | Avg. Iterations | Avg. Time (ms) |
|----------|-------|----------|-------|--------|-------|-----------------|----------------|
| ft10     | 10×10 | 930      | 930*  | 930.0  | 930   | 2317871.4       | 14465.8        |
| la19     | 10×10 | 842      | 842*  | 842.0  | 842   | 147897.4        | 866.8          |
| la21     | 15×10 | 1046     | 1046* | 1046.0 | 1046  | 701835.2        | 5165.6         |
| la24     | 15×10 | 935      | 935*  | 935.0  | 935   | 1528499.4       | 12359.2        |
| la25     | 20×10 | 977      | 977*  | 977.0  | 977   | 911077.2        | 7555.0         |
| la27     | 20×10 | 1235     | 1235* | 1235.0 | 1235  | 209377.8        | 1712.2         |
| la29     | 20×10 | 1152     | 1157  | 1158.0 | 1160  | 31159394.6      | 299999.0       |
| la36     | 15×15 | 1268     | 1268* | 1268.0 | 1268  | 1726843.2       | 16291.6        |
| la37     | 15×15 | 1397     | 1397* | 1399.2 | 1403  | 33006278.8      | 297808.0       |
| la38     | 15×15 | 1196     | 1196* | 1198.4 | 1201  | 26639615.2      | 251164.2       |
| la39     | 15×15 | 1233     | 1233* | 1233.0 | 1233  | 6389358.2       | 62667.8        |
| la40     | 15×15 | 1222     | 1222* | 1223.6 | 1224  | 28899708.6      | 255733.2       |
| abz7     | 20×15 | 656      | 658   | 659.6  | 660   | 25687161        | 299999.4       |
| abz8     | 20×15 | 665(648) | 669   | 669.0  | 669   | 22552921        | 299999.0       |
| abz9     | 20×15 | 678      | 679   | 681.2  | 684   | 21216061        | 275962.2       |

## Usage

### how to build

```shell
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### Example

```cpp
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
```

After calling the `TabuSearch::export_result(path)` function, the scheduling results will be written to a CSV file at the specified path. You can then use the `draw.py` script located in the `output` directory to generate a Gantt chart.

![FT10-gantt_chart](output/FT10-gantt_chart.png)
