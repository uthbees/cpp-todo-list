#pragma once

#include <functional>

using std::function;

struct UndoRedoCommand {
    function<void()> undo;

    function<void()> redo;
};
