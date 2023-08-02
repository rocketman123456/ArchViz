#pragma once
#include "runtime/resource/resource_manager/loader/loader.h"

// #include <future>

namespace ArchViz
{
    template<typename T, typename CI>
    class GpuLoader : ILoader
    {
    public:
        virtual ~GpuLoader() = default;

        // sync upload
        virtual std::pair<std::shared_ptr<T>, size_t> createResource(const CI& create_info)  = 0;
        virtual std::pair<std::shared_ptr<T>, size_t> createResource(const std::string& uri) = 0;

        // virtual std::future<std::pair<std::shared_ptr<T>, size_t>> createResourceAsync(const AsyncCI& create_info) = 0;
        // virtual std::future<std::pair<std::shared_ptr<T>, size_t>> createResourceAsync(const std::string& uri)     = 0;
    };
} // namespace ArchViz
