#include "Task.h"

#include <iostream>
#include <utility>

using std::cout;
using std::endl;
using std::to_string;

Task::Task(string name, const int id) : id(id), name(std::move(name)), done(false) {
}

int Task::get_id() const {
    return this->id;
}

string Task::get_name() const {
    return this->name;
}

bool Task::get_done() const {
    return this->done;
}

void Task::set_name(const string &new_name) {
    this->name = new_name;
}

void Task::toggle_done() {
    this->done = !this->done;
}

void Task::display() const {
    const string done_marker = this->done ? "[x]" : "[ ]";
    cout << to_string(this->id) + ". " + done_marker + " " + this->name << endl;
}
