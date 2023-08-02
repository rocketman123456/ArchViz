#include "runtime/function/render/rhi/gpu_resource_manager.h"

namespace ArchViz
{
    void GpuResourceManager::initialize()
    {
        // register types, loaders, compilers
    }

    void GpuResourceManager::clear()
    {
        m_resource_max_counts.clear();
        m_resource_max_sizes.clear();
        m_resource_types.clear();
        m_resource_types_inv.clear();
        m_resource_arrays.clear();
        m_resource_id_arrays.clear();
        m_resource_loaders.clear();
        m_resource_handles.clear();
    }
} // namespace ArchViz
