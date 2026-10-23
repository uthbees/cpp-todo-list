#include "CommandManager.h"

#include <iostream>
#include <map>
#include <optional>
#include <string>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::map;
using std::pair;
using std::optional;
using std::make_optional;
using std::nullopt;

CommandManager::CommandManager() : running(false) {
}

/// Registers a command. Note that the help menu is printed in order of command registration.
void CommandManager::register_command(const CommandData &command) {
    this->registered_commands[command.name] = command;
    this->command_insertion_order.push_back(command.name);
}

void CommandManager::undo_command() {
    if (this->undo_stack.empty()) {
        cout << "Nothing to undo." << endl;
        return;
    }

    const auto command = this->undo_stack.top();
    this->undo_stack.pop();

    command.undo();
    this->redo_stack.push(command);
}

void CommandManager::redo_command() {
    if (this->redo_stack.empty()) {
        cout << "Nothing to redo." << endl;
        return;
    }

    const auto command = this->redo_stack.top();
    this->redo_stack.pop();

    command.redo();
    this->undo_stack.push(command);
}

void CommandManager::print_help() const {
    cout << "Commands:" << endl;
    for (auto const &command_name: this->command_insertion_order) {
        auto const &command_data = this->registered_commands.at(command_name);
        cout << "  " << command_data.syntax << ": " << command_data.description << endl;
    }
}

/// Listen for commands until stop() is called.
void CommandManager::run() {
    this->running = true;

    while (this->running) {
        cout << endl;
        cout << "> ";
        string command;
        getline(cin, command);

        auto parsed_command = this->parse_command(command);
        if (parsed_command.has_value()) {
            const auto &command_data = parsed_command->first;
            const auto &args = parsed_command->second;
            const auto undo = command_data.executor(args);
            if (undo.has_value()) {
                this->undo_stack.push(UndoRedoCommand{
                    .undo = undo.value(),
                    .redo = [command_data, args] {
                        auto _ = command_data.executor(args);
                    }
                });
                this->redo_stack = stack<UndoRedoCommand>();
            }
        }
    }
}

/// Stop listening for commands. The program will continue past run() once the current command is completed.
void CommandManager::stop() {
    this->running = false;
}

/// Parse a raw command from input. Returns (command data, arguments) if the raw command was valid,
/// or nullopt if it was not. Handles error messages internally.
optional<pair<CommandData, vector<string> > > CommandManager::parse_command(string &raw_command) const {
    trim_in_place(raw_command);

    const size_t first_space_idx = raw_command.find(' ');
    const string command_name = raw_command.substr(0, first_space_idx);
    if (!this->registered_commands.contains(command_name)) {
        cout << "\"" << command_name << "\" is not a valid command." << endl;
        this->print_help();
        return nullopt;
    }

    const CommandData &command_data = this->registered_commands.at(command_name);

    vector<string> args;
    if (command_data.arg_count == 0) {
        args = {};
    } else {
        if (first_space_idx == string::npos) {
            print_insufficient_arguments_message(command_data);
            return nullopt;
        }
        string remaining_args = raw_command.substr(first_space_idx + 1);

        for (int arg_num = 1; arg_num <= command_data.arg_count; arg_num++) {
            string next_arg;
            if (arg_num == command_data.arg_count) {
                next_arg = remaining_args;
            } else {
                const size_t space_idx = remaining_args.find(' ');
                if (space_idx == string::npos) {
                    print_insufficient_arguments_message(command_data);
                    return nullopt;
                }
                next_arg = remaining_args.substr(0, space_idx);
                remaining_args = remaining_args.substr(space_idx + 1);
            }

            if (next_arg.empty()) {
                print_insufficient_arguments_message(command_data);
                return nullopt;
            }

            args.push_back(next_arg);
        }
    }

    return std::make_optional(std::make_pair(command_data, args));
}

void CommandManager::print_insufficient_arguments_message(CommandData const &command_data) {
    cout << "Insufficient arguments. Syntax: " << command_data.syntax << endl;
}

// Modified from: https://stackoverflow.com/a/217605
void CommandManager::trim_in_place(string &str) {
    // ReSharper disable once CppUseRangeAlgorithm
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](const unsigned char c) {
        return !std::isspace(c);
    }));

    str.erase(std::find_if(str.rbegin(), str.rend(), [](const unsigned char c) {
        return !std::isspace(c);
    }).base(), str.end());
}
