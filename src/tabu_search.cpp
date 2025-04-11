//
// Created by qiming on 25-4-4.
//

#include "tabu_search.h"

#ifdef FIND_WITH_PRIORITY_QUEUE
#include <queue>

/**
 * Helper structure for priority queue comparison
 * Contains an action and its associated makespan value
 */
struct CompareAction {
    const Action *_action;
    int _makespan;
    CompareAction(const Action *action, int makespan) : _action(action), _makespan(makespan) {}
};

/**
 * Comparator for priority queue that sorts actions by makespan
 * Uses random tie-breaking for equal makespan values
 */
struct CompareActionComparator {
    bool operator()(const CompareAction &a, const CompareAction &b) const {
        // In priority queue, return true means a has lower priority than b
        if (a._makespan != b._makespan) {
            // Lower makespan has higher priority
            return a._makespan > b._makespan;
        } else {
            // Random tie-breaker for equal makespan
            return RandomGenerator::instance().getInt(2) == 0;
        }
    }
};

/**
 * Finds the best non-tabu move using neighborhood action N7 with approximate makespan evaluation
 * Uses a priority queue to efficiently track best candidates
 * @return The best non-tabu action found, or a random action if all are tabu
 */
Action TabuSearch::find_move() const {
// Macro to safely add legal actions to list
#define LEGAL_PUSH(list, action) \
    if (is_legal_action(action)) { list.push_back(action); }

    std::vector<Action> all_actions;
    std::priority_queue<CompareAction, std::vector<CompareAction>, CompareActionComparator> best_actions;

    // Generate all potential swap actions between critical operations
    for (int i = 0; i < current_schedule.graph.operation_num; i++) {
        auto start = current_schedule.graph.first_machine_operation[i];
        for (auto pre = start; pre != current_schedule.graph.last_machine_operation[i]; pre = NEXT_MACHINE_OP(pre)) {
            if (!current_schedule.is_critical_operation(pre)) continue;

            for (auto next = NEXT_MACHINE_OP(pre); next != -1; next = NEXT_MACHINE_OP(next)) {
                if (!current_schedule.is_critical_operation(next)) continue;

                int js_next = current_schedule.graph.job_successor[next];
                int jp_pre = current_schedule.graph.job_predecessor[pre];

                // Generate swap actions based on critical path conditions
                if (current_schedule.is_critical_operation(js_next)) {
                    Action action1(ActionDirection::BACK, pre, next);
                    LEGAL_PUSH(all_actions, action1)
                    Action action2(ActionDirection::FORWARD, next, pre);
                    LEGAL_PUSH(all_actions, action2)
                } else if (current_schedule.is_critical_operation(jp_pre)) {
                    Action action1(ActionDirection::FORWARD, next, pre);
                    LEGAL_PUSH(all_actions, action1)
                    Action action2(ActionDirection::BACK, pre, next);
                    LEGAL_PUSH(all_actions, action2)
                }
            }
        }
    }

    // Evaluate all actions and populate priority queue
    for (const auto &action: all_actions) {
        int val = ApproximateNeighborhoodActionEvaluation(action);
        best_actions.emplace(&action, val);
    }

    // Find the best non-tabu action
    while (!best_actions.empty()) {
        const Action &action = *best_actions.top()._action;
        const int val = best_actions.top()._makespan;
        if (is_tabu(action, val)) {
            best_actions.pop();
        } else {
            return action;
        }
    }

    // Fallback: return random action if all are tabu
    return all_actions[RandomGenerator::instance().getInt(static_cast<int>(all_actions.size()))];
}
#else
/**
 * Finds the best non-tabu move using neighborhood action N7 with approximate makespan evaluation
 * Uses exhaustive search through all candidates
 * @return The best non-tabu action found, or a random action if all are tabu
 */
Action TabuSearch::find_move() const {
#define LEGAL_PUSH(list, action) \
    if (is_legal_action(action)) { list.push_back(action); }

    int min_makespan = INT_MAX;
    std::vector<Action> all_actions;
    std::vector<Action> best_actions;

    // Generate all potential swap actions between critical operations
    for (int i = 0; i < current_schedule.graph.operation_num; i++) {
        auto start = current_schedule.graph.first_machine_operation[i];
        for (auto pre = start; pre != current_schedule.graph.last_machine_operation[i]; pre = NEXT_MACHINE_OP(pre)) {
            if (!current_schedule.is_critical_operation(pre)) continue;

            for (auto next = NEXT_MACHINE_OP(pre); next != -1; next = NEXT_MACHINE_OP(next)) {
                if (!current_schedule.is_critical_operation(next)) continue;

                int js_next = current_schedule.graph.job_successor[next];
                int jp_pre = current_schedule.graph.job_predecessor[pre];

                // Generate swap actions based on critical path conditions
                if (current_schedule.is_critical_operation(js_next)) {
                    Action action1(ActionDirection::BACK, pre, next);
                    LEGAL_PUSH(all_actions, action1)
                    Action action2(ActionDirection::FORWARD, next, pre);
                    LEGAL_PUSH(all_actions, action2)
                } else if (current_schedule.is_critical_operation(jp_pre)) {
                    Action action1(ActionDirection::FORWARD, next, pre);
                    LEGAL_PUSH(all_actions, action1)
                    Action action2(ActionDirection::BACK, pre, next);
                    LEGAL_PUSH(all_actions, action2)
                }
            }
        }
    }

    // Evaluate all actions and track best non-tabu ones
    for (const auto &action: all_actions) {
        int val = approximate_evaluation(action);
        if (val == INT_MAX) continue;

        if (val < min_makespan) {
            if (!is_tabu(action, val)) {
                min_makespan = val;
                best_actions.clear();
                best_actions.push_back(action);
            }
        } else if (val == min_makespan) {
            if (!is_tabu(action, val)) {
                best_actions.push_back(action);
            }
        }
    }

    // Return random best action or fallback to random action if no non-tabu options
    if (best_actions.empty()) {
        return all_actions[RandomGenerator::instance().getInt(static_cast<int>(all_actions.size()))];
    } else {
        int index = RandomGenerator::instance().getInt(static_cast<int>(best_actions.size()));
        return best_actions[index];
    }
}
#endif


/**
 * Approximates makespan evaluation for a neighborhood action
 * Uses either precise evaluation (if PRECISE_EVALUATE is defined) or fast approximation
 * @param action The neighborhood action to evaluate
 * @return Estimated makespan after applying the action, or INT_MAX if action is invalid
 */
int TabuSearch::approximate_evaluation(const Action &action) const {
#ifdef PRECISE_EVALUATE
    //------------------------------------------------------------------------
    // Precise evaluation by creating modified schedule copy
    Schedule tmp = current_schedule;
    auto &graph = tmp.graph;


    // Update disjunctive graph edges based on move direction
    if (action.direction == ActionDirection::FORWARD) {
        // Forward move: swap operation 'which' before 'where'
        int ms_u = graph.machine_successor[action.which];
        int mp_u = graph.machine_predecessor[action.which];
        int mp_v = graph.machine_predecessor[action.where];

        // Update machine edges
        graph.machine_successor[action.which] = action.where;
        graph.machine_predecessor[action.where] = action.which;

        if (mp_v != -1) {
            graph.machine_successor[mp_v] = action.which;
        } else {
            // Update first operation on machine if needed
            auto it = std::ranges::find(graph.first_machine_operation, action.where);
            if (it != graph.first_machine_operation.end()) {
                *it = action.which;
            }
        }

        graph.machine_predecessor[action.which] = mp_v;

        if (ms_u != -1) {
            graph.machine_predecessor[ms_u] = mp_u;
        } else {
            // Update last operation on machine if needed
            auto it = std::ranges::find(graph.last_machine_operation, action.which);
            if (it != graph.last_machine_operation.end()) {
                *it = mp_u;
            }
        }

        graph.machine_successor[mp_u] = ms_u;
    } else {
        // Backward move: swap operation 'which' after 'where'
        int ms_u = graph.machine_successor[action.which];
        int ms_v = graph.machine_successor[action.where];
        int mp_u = graph.machine_predecessor[action.which];

        // Update machine edges
        graph.machine_successor[action.where] = action.which;
        graph.machine_predecessor[action.which] = action.where;

        if (mp_u != -1) {
            graph.machine_successor[mp_u] = ms_u;
        } else {
            // Update first operation on machine if needed
            auto it = std::ranges::find(graph.first_machine_operation, action.which);
            if (it != graph.first_machine_operation.end()) {
                *it = ms_u;
            }
        }

        graph.machine_predecessor[ms_u] = mp_u;

        if (ms_v != -1) {
            graph.machine_predecessor[ms_v] = action.which;
        } else {
            // Update last operation on machine if needed
            auto it = std::ranges::find(graph.last_machine_operation, action.where);
            if (it != graph.last_machine_operation.end()) {
                *it = action.which;
            }
        }

        graph.machine_successor[action.which] = ms_v;
    }

    // Recalculate schedule times and critical blocks
    tmp.calculate_time_info();

    return tmp.makespan;


    //----------------------------------------------------------------------------------------------------------------------------------------
#endif
    // Fast approximation without full schedule recalculation

    // Move operation 'which' before operation 'where'
    if (action.direction == ActionDirection::FORWARD) {

        // Build new operation sequence
        std::vector<int> new_op_sequence;
        new_op_sequence.push_back(action.which);
        for (int op = action.where; op != action.which; op = current_schedule.graph.machine_successor[op]) {
            new_op_sequence.push_back(op);
        }
        std::vector<int> new_R_value(new_op_sequence.size());
        std::vector<int> new_Q_value(new_op_sequence.size());
        // Calculate new R values (forward pass)

        if (current_schedule.graph.machine_predecessor[action.where] == -1) {
            // 'where' was first on machine
            int jp_u = current_schedule.graph.job_predecessor[action.which];
            new_R_value.front() = current_schedule.time_info[jp_u].end_time;
        } else {
            int jp_u = current_schedule.graph.job_predecessor[action.which];
            int mp_u = current_schedule.graph.machine_predecessor[action.where];
            new_R_value.front() = std::max(current_schedule.time_info[jp_u].end_time, current_schedule.time_info[mp_u].end_time);
        }
        for (int i = 1; i < new_op_sequence.size(); ++i) {
            int op = new_op_sequence[i];
            int jp_op = current_schedule.graph.job_predecessor[op];
            new_R_value[i] = std::max(current_schedule.time_info[jp_op].forward_path_length + current_schedule.operation_list[jp_op].time, new_R_value[i - 1] + current_schedule.operation_list[new_op_sequence[i - 1]].time);
        }
        // Calculate new Q values (backward pass)
        if (current_schedule.graph.machine_successor[action.which] == -1) {
            int op = new_op_sequence.back();
            int js_op = current_schedule.graph.job_successor[op];
            new_Q_value.back() = current_schedule.time_info[js_op].backward_path_length + current_schedule.operation_list[op].time;
        } else {
            int op = new_op_sequence.back();
            int js_op = current_schedule.graph.job_successor[op];
            new_Q_value.back() = std::max(current_schedule.time_info[js_op].backward_path_length, current_schedule.time_info[current_schedule.graph.machine_successor[action.which]].backward_path_length) + current_schedule.operation_list[op].time;
        }
        for (int i = static_cast<int>(new_op_sequence.size()) - 2; i >= 0; --i) {
            int op = new_op_sequence[i];
            int js_op = current_schedule.graph.job_successor[op];
            new_Q_value[i] = std::max(current_schedule.time_info[js_op].backward_path_length, new_Q_value[i + 1]) + current_schedule.operation_list[op].time;
        }
        int res = INT_MIN;
        for (size_t i = 0; i < new_R_value.size(); ++i) {
            res = std::max(res, new_R_value[i] + new_Q_value[i]);
        }
        return res;
    } else {
        // Move operation 'which' after operation 'where'

        // Build new operation sequence (reverse order)
        std::vector<int> new_op_sequence;
        new_op_sequence.push_back(action.which);
        for (int op = action.where; op != action.which; op = current_schedule.graph.machine_predecessor[op]) {
            new_op_sequence.push_back(op);
        }
        std::vector<int> new_R_value(new_op_sequence.size());
        std::vector<int> new_Q_value(new_op_sequence.size());
        // Calculate new Q values (backward pass first)
        // 'where' was last on machine
        if (current_schedule.graph.machine_successor[action.where] == -1) {
            int js_u = current_schedule.graph.job_successor[action.which];
            new_Q_value.front() = current_schedule.time_info[js_u].backward_path_length + current_schedule.operation_list[action.which].time;
        } else {
            int js_u = current_schedule.graph.job_successor[action.which];
            int ms_v = current_schedule.graph.machine_successor[action.where];
            new_Q_value.front() = std::max(current_schedule.time_info[js_u].backward_path_length, current_schedule.time_info[ms_v].backward_path_length) + current_schedule.operation_list[action.which].time;
        }
        for (int i = 1; i < new_op_sequence.size(); ++i) {
            int op = new_op_sequence[i];
            int js_op = current_schedule.graph.job_successor[op];
            new_Q_value[i] = std::max(current_schedule.time_info[js_op].backward_path_length, new_Q_value[i - 1]) + current_schedule.operation_list[op].time;
        }

        // Calculate new R values (forward pass)
        if (current_schedule.graph.machine_predecessor[action.which] == -1) {
            int op = new_op_sequence.back();
            int jp_op = current_schedule.graph.job_predecessor[op];
            new_R_value.back() = current_schedule.time_info[jp_op].forward_path_length + current_schedule.operation_list[jp_op].time;
        } else {
            int op = new_op_sequence.back();
            int jp_op = current_schedule.graph.job_predecessor[op];
            int mp_u = current_schedule.graph.machine_predecessor[action.which];
            new_R_value.back() = std::max(current_schedule.time_info[jp_op].forward_path_length + current_schedule.operation_list[jp_op].time, current_schedule.time_info[mp_u].forward_path_length + current_schedule.operation_list[mp_u].time);
        }
        for (int i = static_cast<int>(new_op_sequence.size()) - 2; i >= 0; --i) {
            int op = new_op_sequence[i];
            int jp_op = current_schedule.graph.job_predecessor[op];
            new_R_value[i] = std::max(current_schedule.time_info[jp_op].forward_path_length + current_schedule.operation_list[jp_op].time, new_R_value[i + 1] + current_schedule.operation_list[new_op_sequence[i + 1]].time);
        }
        int res = INT_MIN;
        for (size_t i = 0; i < new_R_value.size(); ++i) {
            res = std::max(res, new_R_value[i] + new_Q_value[i]);
        }

        return res;
    }
}
/**
 * Executes the specified move action and updates the tabu list
 * @param action The move action to execute
 */
void TabuSearch::make_move(const Action &action) {
    auto &graph = current_schedule.graph;

    // Create tabu sequence for the move
    std::vector<int> tabu_sequence;
    int pos = 0;
    int machine_id = current_schedule.operation_list[action.which].machine_id;

    if (action.direction == ActionDirection::FORWARD) {
        // Forward move: collect operations from 'where' to new position after move
        for (int op = action.where; op != NEXT_MACHINE_OP(action.which); op = NEXT_MACHINE_OP(op)) {
            tabu_sequence.push_back(op);
        }
        // Find original position of 'where' operation
        for (int op = graph.first_machine_operation[machine_id]; op != action.where; op = NEXT_MACHINE_OP(op)) {
            pos++;
        }
    } else {
        // Backward move: collect operations from 'which' to new position after move
        for (int op = action.which; op != NEXT_MACHINE_OP(action.where); op = NEXT_MACHINE_OP(op)) {
            tabu_sequence.push_back(op);
        }
        // Find original position of 'which' operation
        for (int op = graph.first_machine_operation[machine_id]; op != action.which; op = NEXT_MACHINE_OP(op)) {
            pos++;
        }
    }

    // Calculate dynamic tabu tenure
    int L = 10 + current_schedule.graph.job_num / current_schedule.graph.operation_num;
    unsigned long long tabu_time = iteration + RandomGenerator::instance().getInt(L, static_cast<int>(1.5 * L));
    tabu_list.add_sequence(machine_id, pos, tabu_sequence, tabu_time);

    // Update disjunctive graph edges based on move direction
    if (action.direction == ActionDirection::FORWARD) {
        // Forward move: swap operation 'which' before 'where'
        int ms_u = graph.machine_successor[action.which];
        int mp_u = graph.machine_predecessor[action.which];
        int mp_v = graph.machine_predecessor[action.where];

        // Update machine edges
        graph.machine_successor[action.which] = action.where;
        graph.machine_predecessor[action.where] = action.which;

        if (mp_v != -1) {
            graph.machine_successor[mp_v] = action.which;
        } else {
            // Update first operation on machine if needed
            auto it = std::ranges::find(graph.first_machine_operation, action.where);
            if (it != graph.first_machine_operation.end()) {
                *it = action.which;
            }
        }

        graph.machine_predecessor[action.which] = mp_v;

        if (ms_u != -1) {
            graph.machine_predecessor[ms_u] = mp_u;
        } else {
            // Update last operation on machine if needed
            auto it = std::ranges::find(graph.last_machine_operation, action.which);
            if (it != graph.last_machine_operation.end()) {
                *it = mp_u;
            }
        }

        graph.machine_successor[mp_u] = ms_u;
    } else {
        // Backward move: swap operation 'which' after 'where'
        int ms_u = graph.machine_successor[action.which];
        int ms_v = graph.machine_successor[action.where];
        int mp_u = graph.machine_predecessor[action.which];

        // Update machine edges
        graph.machine_successor[action.where] = action.which;
        graph.machine_predecessor[action.which] = action.where;

        if (mp_u != -1) {
            graph.machine_successor[mp_u] = ms_u;
        } else {
            // Update first operation on machine if needed
            auto it = std::ranges::find(graph.first_machine_operation, action.which);
            if (it != graph.first_machine_operation.end()) {
                *it = ms_u;
            }
        }

        graph.machine_predecessor[ms_u] = mp_u;

        if (ms_v != -1) {
            graph.machine_predecessor[ms_v] = action.which;
        } else {
            // Update last operation on machine if needed
            auto it = std::ranges::find(graph.last_machine_operation, action.where);
            if (it != graph.last_machine_operation.end()) {
                *it = action.which;
            }
        }

        graph.machine_successor[action.which] = ms_v;
    }

    // Recalculate schedule times
    current_schedule.calculate_time_info();
}

/**
 * Checks if an action is currently tabu
 * @param action The action to check
 * @param makespan The makespan resulting from this action
 * @return True if the action is tabu and doesn't meet aspiration criteria
 */
bool TabuSearch::is_tabu(const Action &action, int makespan) const {
    // Aspiration criteria: allow tabu moves that improve best solution
    if (makespan < best_schedule.makespan) {
        return false;
    }

    int machine_id = current_schedule.operation_list[action.which].machine_id;
    std::vector<int> new_op_sequence;
    int pos = 0;

    if (action.direction == ActionDirection::FORWARD) {
        // Forward move sequence
        new_op_sequence.push_back(action.which);
        for (int op = action.where; op != action.which; op = NEXT_MACHINE_OP(op)) {
            new_op_sequence.push_back(op);
        }
        // Find original position of 'where' operation
        for (int op = current_schedule.graph.first_machine_operation[machine_id]; op != action.where; op = NEXT_MACHINE_OP(op)) {
            pos++;
        }
    } else {
        // Backward move sequence
        for (int op = NEXT_MACHINE_OP(action.which); op != NEXT_MACHINE_OP(action.where); op = NEXT_MACHINE_OP(op)) {
            new_op_sequence.push_back(op);
        }
        new_op_sequence.push_back(action.which);
        // Find original position of 'which' operation
        for (int op = current_schedule.graph.first_machine_operation[machine_id]; op != action.which; op = NEXT_MACHINE_OP(op)) {
            pos++;
        }
    }

    return tabu_list.is_tabu(machine_id, pos, new_op_sequence, iteration);
}

/**
 * Checks if an action is legal (doesn't violate precedence constraints)
 * @param action The action to check
 * @return True if the action is legal
 */
bool TabuSearch::is_legal_action(const Action &action) const {
    if (action.direction == ActionDirection::BACK) {
        // Check backward move legality
        return current_schedule.time_info[action.where].backward_path_length >=
               current_schedule.time_info[current_schedule.graph.job_successor[action.which]].backward_path_length;
    } else {
        // Check forward move legality
        return current_schedule.time_info[action.where].end_time >=
               current_schedule.time_info[current_schedule.graph.job_predecessor[action.which]].forward_path_length;
    }
}

/**
 * Executes the tabu search algorithm
 * @param max_iteration Maximum number of iterations to run
 * @param condition Early termination condition function
 */
void TabuSearch::search(unsigned long long int max_iteration, const std::function<bool()> &condition) {
    iteration = 0;
    int no_improved = 0;

    while (iteration < max_iteration) {
        const auto action = find_move();
        make_move(action);

        // Update the best solution if improved
        if (current_schedule.makespan < best_schedule.makespan) {
            best_schedule = current_schedule;
            no_improved = 0;
        } else {
            no_improved++;
        }

#ifdef PRINT_INFO
        // Periodic progress reporting
        if (iteration % 100000 == 0) {
            std::cout << "iteration: " << iteration
                      << ", makespan: " << current_schedule.makespan
                      << ", best_makespan: " << best_schedule.makespan
                      << std::endl;
        }
#endif

        iteration++;
        if (condition()) {
            break;
        }
    }

    // Final results output
    //    std::clog << "iteration: " << iteration
    //              << ", best_makespan: " << best_schedule.makespan
    //              << std::endl;
    // best_schedule.export_to_csv("../../output/result.csv");
}