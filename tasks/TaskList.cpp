#include "TaskList.h"

#include <algorithm>
#include <iostream>
#include <optional>

using std::cout;
using std::endl;
using std::string;
using std::optional;
using std::make_optional;
using std::nullopt;

TaskList::TaskList() : next_task_id(1) {
}

Task *TaskList::get_task_ptr_by_id(const string &raw_task_id) {
    const auto parsed_task_id = this->parse_task_id(raw_task_id);
    if (!parsed_task_id.has_value()) return nullptr;
    const int task_id = parsed_task_id.value();

    for (auto &task: this->tasks) {
        if (task.get_id() == task_id) return &task;
    }

    return nullptr;
}

void TaskList::display() const {
    if (this->tasks.empty()) {
        cout << "No tasks." << endl;
    } else {
        cout << "Tasks:" << endl;

        bool any_tasks_done = false;
        bool any_tasks_not_done = false;
        for (const auto &task: this->tasks) {
            if (task.get_done()) {
                any_tasks_done = true;
            } else {
                any_tasks_not_done = true;
                task.display();
            }
        }
        if (any_tasks_done && any_tasks_not_done) cout << endl;

        for (const auto &task: this->tasks) {
            if (task.get_done()) {
                task.display();
            }
        }
    }
}

optional<int> TaskList::handle_add_task(const string &task_name, bool start_done, optional<int> forced_task_id) {
    if (task_name.empty()) {
        cout << "Please specify a task name." << endl;
        return nullopt;
    }

    // forced_task_id should only ever be passed with an unused task id, but we handle it gracefully
    // if it's passed with a used task id by ignoring the parameter.
    int task_id;
    bool used_forced_task_id = false;
    if (forced_task_id == nullopt) {
        task_id = this->next_task_id++;
    } else {
        const auto forced_task_id_val = forced_task_id.value();
        for (const auto &task: this->tasks) {
            if (task.get_id() == forced_task_id_val) {
                task_id = this->next_task_id++;
                break;
            }
        }
        task_id = forced_task_id_val;
        used_forced_task_id = true;
    }

    auto task = Task(task_name, task_id);
    if (start_done) {
        task.toggle_done();
    }
    this->tasks.push_back(task);
    // Re-sort the tasks if we added one with a forced id, because it shouldn't necessarily go at the end.
    if (used_forced_task_id) {
        std::ranges::sort(this->tasks, [](const Task &task1, const Task &task2) {
            return task1.get_id() < task2.get_id();
        });
    }

    cout << "Added task \"" << task_name << "\"." << endl;
    cout << endl;
    this->display();
    return make_optional(task_id);
}

bool TaskList::handle_check_task(const string &raw_task_id) {
    const auto task_ptr = this->get_task_ptr_by_id(raw_task_id);
    if (task_ptr == nullptr) {
        cout << "Error: No task found with id \"" << raw_task_id << "\"." << endl;
        return false;
    }

    task_ptr->toggle_done();
    const string marked_status = task_ptr->get_done() ? "done" : "not done";
    cout << "Marked \"" << task_ptr->get_name() << "\" as " << marked_status << "." << endl;
    this->display();
    return true;
}

bool TaskList::handle_rename_task(const string &raw_task_id, const string &new_task_name) {
    const auto task_ptr = this->get_task_ptr_by_id(raw_task_id);
    if (task_ptr == nullptr) {
        cout << "Error: No task found with id \"" << raw_task_id << "\"." << endl;
        return false;
    }

    if (new_task_name.empty()) {
        cout << "Please specify a task name." << endl;
        return false;
    }

    const string old_name = task_ptr->get_name();
    task_ptr->set_name(new_task_name);
    cout << "Renamed task " << raw_task_id << " from \"" << old_name << "\" to \"" << new_task_name << "\"." << endl;
    this->display();
    return true;
}

bool TaskList::handle_delete_task(const string &raw_task_id, const bool reclaim_task_id) {
    const auto task_ptr = this->get_task_ptr_by_id(raw_task_id);
    if (task_ptr == nullptr) {
        cout << "Error: No task found with id \"" << raw_task_id << "\"." << endl;
        return false;
    }
    const auto task_id = task_ptr->get_id();

    // reclaim_task_id should only ever be passed when raw_task_id corresponds to the most recently added task,
    // but we handle it gracefully if it's not by ignoring the parameter.
    if (reclaim_task_id && task_id == this->next_task_id - 1) {
        this->next_task_id--;
    }

    const auto task_name = task_ptr->get_name();

    bool deleted_task = false;
    for (int i = 0; i < this->tasks.size(); i++) {
        if (this->tasks[i].get_id() == task_id) {
            this->tasks.erase(this->tasks.begin() + i);
            deleted_task = true;
            break;
        }
    }

    if (!deleted_task) {
        cout << "Internal error: Failed to delete task." << endl;
        return false;
    }

    cout << "Deleted task \"" << task_name << "\"." << endl;
    this->display();
    return true;
}

/// Returns the parsed and validated task id, or nullopt if parsing or validation failed.
/// If not nullopt, the return value is safe to use as a task id.
optional<int> TaskList::parse_task_id(const string &raw_task_id) const {
    int task_id;
    try {
        task_id = std::stoi(raw_task_id);
    } catch (...) {
        return nullopt;
    }

    if (task_id < 1 || task_id > this->tasks.size()) {
        return nullopt;
    }

    return task_id;
}
