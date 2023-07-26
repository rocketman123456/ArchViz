#pragma once

namespace ArchViz
{
    class ICommand
    {
    public:
        virtual ~ICommand() = default;

        virtual void doCmd() = 0;
        virtual void redoCmd() = 0;
    };
}
