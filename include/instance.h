//
// Created by qiming on 25-3-31.
//
// File: instance.h
// Description: Definition of Instance structure and related I/O operations for job shop scheduling
//

#ifndef JOBSHOPSCHEDULING_INSTANCE_H
#define JOBSHOPSCHEDULING_INSTANCE_H

#include <iostream>
#include <string>
#include <vector>

/**
 * Data structure representing a job shop scheduling problem instance
 *
 * Contains:
 * - Number of jobs
 * - Number of operations per job
 * - Processing data for all operations
 */
struct Instance {
    int job_num;      // Total number of jobs in the instance
    int operation_num;// Number of operations per job
    std::vector<std::vector<std::pair<int, int>>> data;
    // 2D vector where:
    // - First index: job ID
    // - Second index: operation sequence
    // - pair<int, int>: (machine_id, processing_time)
};

/**
 * Input stream operator for Instance
 * @param is Input stream
 * @param instance Instance object to populate
 * @return Reference to the input stream
 */
std::istream &operator>>(std::istream &is, Instance &instance);

/**
 * Output stream operator for Instance
 * @param os Output stream
 * @param instance Instance object to output
 * @return Reference to the output stream
 */
std::ostream &operator<<(std::ostream &os, const Instance &instance);

/**
 * Loads problem instance from file
 * @param path Filesystem path to the instance data file
 * @return Instance object populated with the data
 */
Instance load_instance(const std::string &path);

#endif// JOBSHOPSCHEDULING_INSTANCE_H
