#pragma once
#include "runtime/function/command/command.h"

#include <list>
#include <memory>
#include <vector>

namespace ArchViz
{
    class CommandSystem
    {
    public:
        void recordCommand(std::shared_ptr<ICommand> cmd);
        void clearCommandHistory();
        void doCmd();
        void redoCmd();

    private:
        std::list<std::shared_ptr<ICommand>> m_history {};

        int64_t m_current_cmd {0};
    };
} // namespace ArchViz
