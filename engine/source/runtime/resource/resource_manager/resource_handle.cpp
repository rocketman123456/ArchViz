#include "runtime/resource/resource_manager/resource_handle.h"

#include "runtime/core/base/macro.h"

namespace ArchViz
{
    std::atomic<ResourceTypeId> ResourceTypeIdAllocator::m_next_id {0};
    std::atomic<ResourceId>     ResourceIdAllocator::m_next_id {0};

    ResourceTypeId ResourceTypeIdAllocator::alloc()
    {
        std::atomic<ResourceTypeId> new_object_ret = m_next_id.load();
        m_next_id++;
        if (m_next_id >= k_invalid_resource_type_id)
        {
            LOG_FATAL("gobject id overflow");
        }
        return new_object_ret;
    }

    ResourceId ResourceIdAllocator::alloc()
    {
        std::atomic<ResourceId> new_object_ret = m_next_id.load();
        m_next_id++;
        if (m_next_id >= k_invalid_resource_id)
        {
            LOG_FATAL("gobject id overflow");
        }
        return new_object_ret;
    }
} // namespace ArchViz