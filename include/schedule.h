//
// Created by qiming on 25-4-1.
//
// File: schedule.h
// Description: Defines the Schedule class for job shop scheduling problems,
//              including critical path analysis and schedule evaluation
//

#ifndef JOBSHOPSCHEDULING_SCHEDULE_H
#define JOBSHOPSCHEDULING_SCHEDULE_H

#include "graph.h"
#include "instance.h"
#include "operation.h"
#include <algorithm>
#include <random>

/**
 * Operation timing information for neighborhood evaluation in scheduling
 *
 * Implements the forward/backward path length calculation method used in
 * job shop scheduling for identifying critical paths.
 */
struct OperationTimeInfo {
    int operator_id;         // Unique identifier for the operation, only use in export function
    int forward_path_length; // Longest path from start to this operation (R[i])
    int backward_path_length;// Longest path from this operation to finish (Q[i])
    int end_time;            // Earliest possible completion time (R[i] + processing time)

    /**
     * Comparison operator for sorting operations by forward path length
     * @param other The other OperationTimeInfo to compare against
     * @return True if this operation has shorter forward path length
     */
    bool operator<(const OperationTimeInfo &other) const {
        return forward_path_length < other.forward_path_length;
    }
};

/**
 * Main scheduling class representing a complete schedule solution.
 * Handles schedule generation, critical path analysis, and evaluation.
 */
class Schedule {
    friend class TabuSearch;// Allow TabuSearch access to private members

public:
    /**
     * Constructs a new schedule from problem instance data
     * @param instance The problem instance to schedule
     */
    explicit Schedule(const Instance &instance) noexcept
        : operation_list(instance),
          machine_operation(instance),
          makespan(0) {
        // Initialize with random feasible solution
        graph = generate_random_initial_solution(instance);
        // Compute timing information and critical paths
        calculate_time_info();
        // Identify critical blocks for optimization
        // update_critical_blocks();
    }

    // Standard copy/move operations
    Schedule(const Schedule &other) = default;
    Schedule &operator=(const Schedule &other) = default;
    Schedule(Schedule &&other) = default;
    Schedule &operator=(Schedule &&other) = default;

    /**
     * Export schedule data to CSV file
     * @param filename Path to output file
     */
    void export_to_csv(const std::string &filename);

    /**
     * Check if operation is on the critical path
     * @param operation_id ID of operation to check
     * @return True if operation is on critical path
     */
    [[nodiscard]] bool is_critical_operation(int operation_id) const;

    /**
     * Calculate forward and backward path lengths for all operations
     * Updates makespan and operation timing information
     */
    void calculate_time_info();

    /**
     * Identify and update critical blocks in the current schedule
     */
    void update_critical_blocks();

private:
    OperationList operation_list;      // All operations in the schedule
    MachineOperation machine_operation;// Operations grouped by machine
    Graph graph;                       // Disjunctive graph representation

    int makespan;                                 // Total schedule duration
    std::vector<OperationTimeInfo> time_info;     // Timing information per operation
    std::vector<std::vector<int>> critical_blocks;// Groups of operations on critical paths
};

/**
 * Determines if an operation lies on the schedule's critical path
 * @return True if forward_path_length + backward_path_length equals makespan
 */
inline bool Schedule::is_critical_operation(int operation_id) const {
    return (time_info[operation_id].forward_path_length +
                    time_info[operation_id].backward_path_length ==
            makespan);
}

#endif// JOBSHOPSCHEDULING_SCHEDULE_H
