#pragma once
#include "runtime/function/render/rhi/rhi.h"

namespace ArchViz
{
    class VulkanRHI : public RHI
    {
    public:
        VulkanRHI()          = default;
        virtual ~VulkanRHI() = default;

        void initialize(RHIInitInfo initialize_info) override;
        void prepareContext() override;

        void clear() override;

    private:
    };
} // namespace ArchViz
