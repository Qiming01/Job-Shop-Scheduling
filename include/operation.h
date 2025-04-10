//
// Created by qiming on 25-3-31.
//
// File: operation.h
// Description: Definitions of operation-related structures for job shop scheduling
//

#ifndef JOBSHOPSCHEDULING_OPERATION_H
#define JOBSHOPSCHEDULING_OPERATION_H

#include "instance.h"
#include <vector>

/**
 * Single operation in job shop scheduling
 */
struct Operation {
    int job_id;       // ID of the job this operation belongs to
    int operation_cnt;// Sequence number of this operation within its job (1-based)
    int machine_id;   // Machine ID required to process this operation
    int time;         // Processing time required for this operation

    /**
     * Constructor for Operation
     * @param job_id Job ID
     * @param operation_cnt Operation sequence number
     * @param machine_id Required machine ID
     * @param time Processing time
     */
    Operation(int job_id, int operation_cnt, int machine_id, int time)
        : job_id(job_id), operation_cnt(operation_cnt), machine_id(machine_id), time(time) {}
};

/**
 * Collection of operation IDs grouped by machine
 */
struct MachineOperation {
    std::vector<std::vector<int>> operation_ids;
    // Matrix where:
    // - First index: machine ID
    // - Second index: operation sequence on that machine
    // - Value: operation ID

    /**
     * Constructor that initializes operations by machine from instance data
     * @param instance Problem instance containing job/machine data
     */
    explicit MachineOperation(const Instance &instance) {
        operation_ids.resize(instance.operation_num);// Size to number of machines
        for (int i = 0; i < instance.job_num; ++i) {
            for (int j = 0; j < instance.operation_num; ++j) {
                operation_ids[instance.data[i][j].first].emplace_back(
                        i * instance.operation_num + j + 1);
            }
        }
    }
};

/**
 * Complete list of all operations with index access
 */
struct OperationList {
    [[maybe_unused]] int job_num;     // Total number of jobs
    int operation_num;                // Operations per job
    std::vector<Operation> operations;// Storage for all operations (1-based index)

    /**
     * Constructor that builds operation list from instance data
     * Includes dummy start (0) and end (n+1) operations
     * @param instance Problem instance data
     */
    explicit OperationList(const Instance &instance)
        : job_num(instance.job_num),
          operation_num(instance.operation_num) {
        // Dummy start operation (index 0)
        operations.emplace_back(0, 0, 0, 0);

        // Add all real operations
        for (int i = 0; i < instance.job_num; ++i) {
            for (int j = 0; j < instance.operation_num; ++j) {
                operations.emplace_back(
                        i + 1,                      // job_id (1-based)
                        j + 1,                      // operation_cnt (1-based)
                        instance.data[i][j].first,  // machine_id
                        instance.data[i][j].second);// processing time
            }
        }

        // Dummy end operation (last index)
        operations.emplace_back(0, 0, 0, 0);
    }

    /**
     * Index operator (non-const version)
     * @param id Operation ID
     * @return Reference to the requested Operation
     */
    Operation &operator[](int id) {
        return operations[id];
    }

    /**
     * Index operator (const version)
     * @param id Operation ID
     * @return Const reference to the requested Operation
     */
    const Operation &operator[](int id) const {
        return operations[id];
    }

    /**
     * Get operation reference by job and sequence position
     * @param job_id ID of the job (1-based)
     * @param operation_cnt Sequence number within job (1-based)
     * @return Reference to the requested Operation
     */
    [[maybe_unused]] Operation &get_operation_ref(int job_id, int operation_cnt) {
        return operations[(job_id - 1) * operation_num + operation_cnt];
    }

    /**
     * Get operation reference by operation ID
     * @param operation_id Unique operation identifier (1-based)
     * @return Reference to the requested Operation
     */
    [[maybe_unused]] Operation &get_operation_ref(int operation_id) {
        return operations[operation_id];
    }
};

#endif// JOBSHOPSCHEDULING_OPERATION_H
