//
// Created by qiming on 25-3-31.
//
// File: graph.h
// Description: Definition of disjunctive graph structure and related functions for job shop scheduling
//

#ifndef JOBSHOPSCHEDULING_GRAPH_H
#define JOBSHOPSCHEDULING_GRAPH_H

#include "operation.h"
#include <deque>
#include <fstream>
#include <vector>

/**
 * Disjunctive graph representation for job shop scheduling.
 * Total number of nodes = job_num * operation_num + 2 (including virtual start and end nodes)
 */
struct Graph {
    int job_num;      // Number of jobs
    int operation_num;// Number of operations per job
    int node_num;     // Total nodes count (job_num * operation_num + 2)

    /**
     * Forward edges in the disjunctive graph.
     * For all nodes except virtual nodes:
     * - edges[u] = v indicates an edge u -> v
     * - edges[u] = -1 indicates no outgoing edge from u
     *
     * Virtual nodes' outgoing edges are handled separately via
     * first_job_operation and first_machine_operation.
     */
    std::vector<int> job_successor;    // Successor in job sequence
    std::vector<int> machine_successor;// Successor in machine sequence

    std::vector<int> job_predecessor;    // Predecessor in job sequence
    std::vector<int> machine_predecessor;// Predecessor in machine sequence

    /**
     * Special edge handling for virtual nodes' outgoing edges:
     */
    std::vector<int> first_job_operation;    // First operation ID for each job
    std::vector<int> last_job_operation;     // Last operation ID for each job
    std::vector<int> first_machine_operation;// First operation ID for each machine
    std::vector<int> last_machine_operation; // Last operation ID for each machine

    /**
     * Performs topological sort on the graph
     * @param reverse If true, performs reverse topological sort
     * @return Deque containing node IDs in topologically sorted order
     */
    [[nodiscard]] std::deque<int> topological_sort(bool reverse = false) const;
};

/**
 * Generates a random initial solution graph from problem instance
 * @param instance The scheduling problem instance
 * @return Graph structure representing a feasible solution
 */
Graph generate_random_initial_solution(const Instance &instance);

/**
 * Tests the correctness of topological sort implementation
 * @return true if test passes, false otherwise
 */
[[maybe_unused]] bool test_topological_sort();

#endif// JOBSHOPSCHEDULING_GRAPH_H
