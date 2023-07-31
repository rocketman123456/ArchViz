#pragma once
#include <memory>
#include <string>
#include <utility>

namespace ArchViz
{
    class ILoader
    {
    public:
        virtual ~ILoader() = default;
    };

    // create a resource from CI
    template<typename T, typename CI>
    class Loader : public ILoader
    {
    public:
        virtual ~Loader() = default;

        virtual std::pair<std::shared_ptr<T>, size_t> createResource(const CI& create_info)  = 0;
        virtual std::pair<std::shared_ptr<T>, size_t> createResource(const std::string& uri) = 0;
    };
} // namespace ArchViz
