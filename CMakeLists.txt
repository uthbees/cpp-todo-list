cmake_minimum_required(VERSION 4.3)
project(cpp-todo-list)

set(CMAKE_CXX_STANDARD 20)

add_executable(cpp-todo-list main.cpp
        tasks/TaskList.cpp
        tasks/TaskList.h
        tasks/Task.cpp
        tasks/Task.h
        commands/CommandManager.cpp
        commands/CommandManager.h
        commands/CommandData.h
        commands/UndoRedoCommand.h
)
