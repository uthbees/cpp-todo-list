#pragma once
#include <string>

using std::string;

class Task {
public:
    explicit Task(string name, int id);

    [[nodiscard]] int get_id() const;

    [[nodiscard]] string get_name() const;

    [[nodiscard]] bool get_done() const;

    void set_name(const string &name);

    void toggle_done();

    void display() const;

private:
    int id;
    string name;
    bool done;
};
