#pragma once
#include <memory>
#include <string>
#include <utility>

namespace ArchViz
{
    class ICompiler
    {
    public:
        virtual ~ICompiler() = default;
    };

    // create a resource from CI
    template<typename From, typename To>
    class Compiler : public ICompiler
    {
    public:
        virtual ~Compiler() = default;

        virtual std::pair<std::shared_ptr<To>, size_t> compileResource(std::shared_ptr<From> from) = 0;
    };
}
