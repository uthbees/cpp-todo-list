#pragma once

#include <functional>
#include <map>
#include <optional>
#include <stack>
#include <string>

#include "CommandData.h"
#include "UndoRedoCommand.h"

using std::string;
using std::vector;
using std::stack;
using std::map;
using std::function;
using std::pair;
using std::optional;

class CommandManager {
public:
    CommandManager();

    void register_command(const CommandData &command);

    void undo_command();

    void redo_command();

    void print_help() const;

    void run();

    void stop();

private:
    bool running;
    map<string, CommandData> registered_commands;
    vector<string> command_insertion_order;

    stack<UndoRedoCommand> undo_stack;
    stack<UndoRedoCommand> redo_stack;

    optional<pair<CommandData, vector<string> > > parse_command(string &raw_command) const;

    static void print_insufficient_arguments_message(CommandData const &command_data);

    static void trim_in_place(string &str);
};
