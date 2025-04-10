//
// Created by qiming on 25-4-4.
//
// File: tabu_search.h
// Description: Implementation of Tabu Search metaheuristic for job shop scheduling
//

#ifndef JOBSHOPSCHEDULING_TABUSEARCH_H
#define JOBSHOPSCHEDULING_TABUSEARCH_H

#include "RandomGenerator.h"
#include "schedule.h"
#include <functional>
#include <unordered_map>
#include <vector>

/**
 * Represents a tabu list item containing:
 * - Operation ID
 * - Machine position (0-based index)
 */
struct TabuItem {
    int op_id;      // Operation identifier
    int machine_pos;// Position on machine (0-based)

    bool operator==(const TabuItem &other) const {
        return op_id == other.op_id && machine_pos == other.machine_pos;
    }
};

/**
 * Hash function specialization for TabuItem
 */
template<>
struct std::hash<TabuItem> {
    std::size_t operator()(const TabuItem &item) const noexcept {
        std::size_t h1 = std::hash<int>{}(item.op_id);
        std::size_t h2 = std::hash<int>{}(item.machine_pos);
        return h1 ^ (h2 << 1);
    }
};

/**
 * Tabu list implementation that prevents cycling by recording forbidden moves
 */
struct TabuList {
    std::vector<std::unordered_map<TabuItem, unsigned long long>> tabu_list;

    /**
     * Constructor
     * @param machine_num Number of machines in the problem
     */
    explicit TabuList(int machine_num = 0) {
        tabu_list.resize(machine_num);
    }

    /**
     * Add an item to the tabu list
     * @param item Tabu item to add
     * @param value The iteration until which this move is tabu
     * @param machine_id Machine identifier
     */
    void add_item(const TabuItem &item, unsigned long long value, int machine_id = 0) {
        tabu_list[machine_id][item] = value;
    }

    /**
     * Check if an item exists in the tabu list
     * @return True if the item is currently tabu
     */
    [[nodiscard]] bool has_item(const TabuItem &item, int machine_id = 0) const {
        if (machine_id >= 0 && machine_id < tabu_list.size()) {
            return tabu_list[machine_id].contains(item);
        }
        return false;
    }

    /**
     * Get the expiration iteration for a tabu item
     * @return The iteration when tabu status expires (0 if not tabu)
     */
    [[nodiscard]] unsigned long long get_item_value(const TabuItem &item, int machine_id = 0) const {
        if (has_item(item, machine_id)) {
            return tabu_list[machine_id].at(item);
        }
        return 0;
    }

    /**
     * Add an entire sequence of operations to the tabu list
     * @param machine_id Machine identifier
     * @param start_pos Starting position on machine
     * @param sequence Vector of operation IDs
     * @param tabu_time Iteration until which these moves are tabu
     */
    void add_sequence(int machine_id, int start_pos, const std::vector<int> &sequence,
                      unsigned long long tabu_time) {
        for (const int op: sequence) {
            tabu_list[machine_id][{op, start_pos++}] = tabu_time;
        }
    }

    /**
     * Check if a sequence is currently tabu
     * @return True if any operation in the sequence is still tabu
     */
    [[nodiscard]] bool is_tabu(int machine_id, int start_pos,
                               const std::vector<int> &sequence,
                               unsigned long long iteration) const {
        for (const int op: sequence) {
            if (get_item_value({op, start_pos++}, machine_id) < iteration) {
                return false;
            }
        }
        return true;
    }
};

/**
 * Enumeration of possible move directions
 */
enum class ActionDirection {
    FORWARD,// Move operation earlier in sequence
    BACK    // Move operation later in sequence
};

/**
 * Represents a neighbor solution move
 */
struct Action {
    ActionDirection direction;// Move direction
    int which;                // Operation to move
    int where;                // Target position

    Action() noexcept : direction(ActionDirection::FORWARD), which(-1), where(-1) {}
    Action(ActionDirection direction, int u, int v) : direction(direction), which(u), where(v) {}
};

// Macro for accessing next operation on same machine
#define NEXT_MACHINE_OP(op) current_schedule.graph.machine_successor[op]

/**
 * Tabu Search algorithm implementation for job shop scheduling
 */
class TabuSearch {
public:
    explicit TabuSearch(const Instance &instance) : current_schedule(instance),
                                                    best_schedule(current_schedule),
                                                    tabu_list(instance.operation_num) {}

    /**
     * Get current best makespan
     * @return Best makespan found so far
     */
    [[nodiscard]] int makespan() const { return best_schedule.makespan; }

    /**
     * Execute the tabu search algorithm
     * @param max_iteration Maximum iterations to run
     * @param condition Optional termination condition function
     */
    void search(unsigned long long max_iteration, const std::function<bool()> &condition = [] { return false; });

protected:
    /**
     * Find the best available move in neighborhood
     * @return Selected move action
     */
    [[nodiscard]] Action find_move() const;

    /**
     * Fast approximate evaluation of move quality
     * @return Estimated makespan after move
     */
    [[nodiscard]] int approximate_evaluation(const Action &action) const;

    /**
     * Execute a move and update schedules
     */
    void make_move(const Action &action);

    /**
     * Check if a move is currently tabu
     * @param makespan Current solution quality
     * @return True if move is tabu
     */
    [[nodiscard]] bool is_tabu(const Action &action, int makespan) const;

    /**
     * Check if a move is legal
     * @return True if move is valid
     */
    [[nodiscard]] inline bool is_legal_action(const Action &action) const;

private:
    unsigned long long iteration{};// Current iteration counter
    Schedule current_schedule;     // Current working solution
    Schedule best_schedule;        // Best solution found
    TabuList tabu_list;            // Tabu move tracking
};

#endif// JOBSHOPSCHEDULING_TABUSEARCH_H
