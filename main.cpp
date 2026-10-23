#include <iostream>

#include "commands/CommandManager.h"
#include "tasks/TaskList.h"

using std::cin;
using std::cout;
using std::endl;
using std::getline;
using std::string;
using std::to_string;
using std::nullopt;
using std::make_optional;

// Note: If I was going to keep working on this, I would want to decouple TaskList from the UI, but this is
// just a class project that I've already spent too much time on, so I'm leaving it as is.

int main() {
    cout << "========== Todo list ==========" << endl;
    cout << "Type \"help\" for help." << endl;

    auto task_list = TaskList();
    task_list.display();

    auto command_manager = CommandManager();
    command_manager.register_command(CommandData{
        .name = "add",
        .syntax = "add <task name>",
        .description = "Add a task with the specified name.",
        .arg_count = 1,
        .executor = [&task_list](const vector<string> &args) -> optional<function<void()> > {
            const auto task_id = task_list.handle_add_task(args[0]);
            if (task_id == nullopt) return nullopt;

            return make_optional([&task_list, task_id] {
                task_list.handle_delete_task(to_string(task_id.value()), true);
            });
        }
    });
    command_manager.register_command(CommandData{
        .name = "done",
        .syntax = "done <task id>",
        .description = "Check/uncheck a task's done status.",
        .arg_count = 1,
        .executor = [&task_list](const vector<string> &args) -> optional<function<void()> > {
            const auto result = task_list.handle_check_task(args[0]);
            if (result == false) return nullopt;

            return make_optional([&task_list, args] {
                task_list.handle_check_task(args[0]);
            });
        }
    });
    command_manager.register_command(CommandData{
        .name = "rename",
        .syntax = "rename <task id> <new task name>",
        .description = "Rename a task.",
        .arg_count = 2,
        .executor = [&task_list](const vector<string> &args) -> optional<function<void()> > {
            const auto task_ptr = task_list.get_task_ptr_by_id(args[0]);
            // hacky workaround because TaskList is coupled to the UI
            if (task_ptr == nullptr) {
                cout << "Error: No task found with id \"" << args[0] << "\"." << endl;
                return nullopt;
            }

            const auto old_name = task_ptr->get_name();
            const auto result = task_list.handle_rename_task(args[0], args[1]);
            if (result == false) return nullopt;

            return make_optional([&task_list, args, old_name] {
                task_list.handle_rename_task(args[0], old_name);
            });
        }
    });
    command_manager.register_command(CommandData{
        .name = "delete",
        .syntax = "delete <task id>",
        .description = "Delete a task.",
        .arg_count = 1,
        .executor = [&task_list](const vector<string> &args) -> optional<function<void()> > {
            const auto task_ptr = task_list.get_task_ptr_by_id(args[0]);

            // hacky workaround because TaskList is coupled to the UI
            if (task_ptr == nullptr) {
                cout << "Error: No task found with id \"" << args[0] << "\"." << endl;
                return nullopt;
            }

            const auto task_id = task_ptr->get_id();
            const auto task_name = task_ptr->get_name();
            const auto task_done = task_ptr->get_done();
            const auto result = task_list.handle_delete_task(args[0]);
            if (result == false) return nullopt;

            return make_optional([&task_list, task_id, task_name, task_done] {
                task_list.handle_add_task(task_name, task_done, make_optional(task_id));
            });
        }
    });
    command_manager.register_command(CommandData{
        .name = "undo",
        .syntax = "undo",
        .description = "Undo the most recent action.",
        .arg_count = 0,
        .executor = [&command_manager](const vector<string> &) -> optional<function<void()> > {
            command_manager.undo_command();
            return nullopt;
        }
    });
    command_manager.register_command(CommandData{
        .name = "redo",
        .syntax = "redo",
        .description = "Redo the most recently undone action.",
        .arg_count = 0,
        .executor = [&command_manager](const vector<string> &) -> optional<function<void()> > {
            command_manager.redo_command();
            // redo_command handles updating the undo stack internally.
            return nullopt;
        }
    });
    command_manager.register_command(CommandData{
        .name = "list",
        .syntax = "list",
        .description = "Print the full todo list.",
        .arg_count = 0,
        .executor = [&task_list](const vector<string> &) -> optional<function<void()> > {
            task_list.display();
            return nullopt;
        }
    });
    command_manager.register_command(CommandData{
        .name = "help",
        .syntax = "help",
        .description = "Displays this help.",
        .arg_count = 0,
        .executor = [&command_manager](const vector<string> &) -> optional<function<void()> > {
            command_manager.print_help();
            return nullopt;
        }
    });
    command_manager.register_command(CommandData{
        .name = "exit",
        .syntax = "exit",
        .description = "Exits the program.",
        .arg_count = 0,
        .executor = [&command_manager](const vector<string> &) -> optional<function<void()> > {
            command_manager.stop();
            return nullopt;
        }
    });

    command_manager.run();
    return 0;
}
