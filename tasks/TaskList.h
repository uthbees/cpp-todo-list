#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Task.h"

using std::string;
using std::vector;
using std::optional;
using std::nullopt;

class TaskList {
public:
    TaskList();

    Task *get_task_ptr_by_id(const string &raw_task_id);

    void display() const;

    optional<int> handle_add_task(
        const string &task_name,
        bool start_done = false,
        optional<int> forced_task_id = nullopt
    );

    bool handle_check_task(const string &raw_task_id);

    bool handle_rename_task(const string &raw_task_id, const string &new_task_name);

    bool handle_delete_task(const string &raw_task_id, bool reclaim_task_id = false);

private:
    int next_task_id;

    vector<Task> tasks;

    [[nodiscard]] optional<int> parse_task_id(const string &raw_task_id) const;
};
