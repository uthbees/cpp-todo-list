#pragma once

#include <functional>
#include <optional>
#include <string>

using std::function;
using std::vector;
using std::string;
using std::optional;

struct CommandData {
    string name;
    string syntax;
    string description;
    int arg_count;
    // The executor is a lambda that, when called, executes the command with the arguments and
    // returns a lambda to undo it, or nullopt if nothing should be added to the undo stack.
    // Redoing the command after it's been undone can be done by calling the executor again
    // with the original arguments.
    function<optional<function<void()> >(vector<string>)> executor;
};
