#pragma once
#include "runtime/core/event/event.h"

namespace ArchViz
{
    class IListener
    {
    public:
        virtual bool canHandle(const IEvent& event) = 0;
        virtual void handleEvent(const IEvent& event);
    };
} // namespace ArchViz
