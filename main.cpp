#include "RandomGenerator.h"
#include "instance.h"
#include "tabu_search.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>


using namespace std;
using namespace std::chrono;

const int LIMIT_TIME = 300;// 秒
const unsigned long long MAX_ITERATION = 100000000;
const vector<int> SEEDS = {2001, 1107, 142857, 202474297, 202474298};


string get_subfolder(const string &instance_name) {
    if (instance_name.rfind("ft", 0) == 0) return "ft";
    if (instance_name.rfind("la", 0) == 0) return "la";
    if (instance_name.rfind("abz", 0) == 0) return "abz";
    return "unknown";// fallback
}

int main() {
    map<string, int> instances = {
            {"ft10", 930},
            {"la19", 842},
            {"la21", 1046},
            {"la24", 935},
            {"la25", 977},
            {"la27", 1235},
            {"la29", 1152},
            {"la36", 1268},
            {"la37", 1397},
            {"la38", 1196},
            {"la39", 1233},
            {"la40", 1222},
            {"abz7", 656},
            {"abz8", 665},
            {"abz9", 679}};

    ofstream outfile("../../output/results.md");
    outfile << "| Instance | Seed | Makespan | Iterations | Time (ms) |\n";
    outfile << "|----------|------|----------|------------|-----------|\n";

    for (const auto &[name, best_answer]: instances) {
        string folder = get_subfolder(name);
        string path = "../../instance/" + folder + "/" + name + ".txt";
        cout << "Testing instance: " << name << " (path: " << path << ")\n";

        for (int seed: SEEDS) {
            Instance instance = load_instance(path);
            RandomGenerator::instance().setSeed(seed);

            auto start = high_resolution_clock::now();
            auto tabuSearch = TabuSearch(instance);
            steady_clock::time_point endTime = steady_clock::now() + seconds(LIMIT_TIME);
            tabuSearch.search(MAX_ITERATION, [&]() {
                return duration_cast<milliseconds>(endTime - steady_clock::now()).count() <= 0 || tabuSearch.makespan() == best_answer;
            });
            auto end = high_resolution_clock::now();
            auto duration = duration_cast<milliseconds>(end - start);

            int makespan = tabuSearch.makespan();
            unsigned long long iter = tabuSearch.get_iter();
            long long time_ms = duration.count();

            cout << "  Seed " << seed << " -> Makespan: " << makespan
                 << ", Iter: " << iter << ", Time: " << time_ms << "ms\n";

            outfile << "| " << name << " | " << seed << " | " << makespan
                    << " | " << iter << " | " << time_ms << " |\n";
        }

        cout << "---------------------------------------------\n";
    }

    outfile.close();
    cout << "Results written to results.md\n";
    return 0;
}
