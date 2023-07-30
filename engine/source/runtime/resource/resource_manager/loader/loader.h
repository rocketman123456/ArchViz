#pragma once
#include <memory>
#include <string>

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

        virtual std::shared_ptr<T> createResource(const CI& create_info)  = 0;
        virtual std::shared_ptr<T> createResource(const std::string& uri) = 0;
    };
} // namespace ArchViz
