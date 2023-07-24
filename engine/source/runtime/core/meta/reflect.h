/*
MIT License

Copyright (c) 2018 Jeff Preshing

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

namespace ArchViz
{
    namespace Reflect
    {

        //--------------------------------------------------------
        // Base class of all type descriptors
        //--------------------------------------------------------

        struct TypeDescriptor
        {
            const char* name;
            size_t      size;

            TypeDescriptor(const char* name, size_t size) : name {name}, size {size} {}
            virtual ~TypeDescriptor() {}
            virtual std::string getFullName() const { return name; }
            virtual void        dump(const void* obj, int indentLevel = 0) const = 0;
        };

        //--------------------------------------------------------
        // Finding type descriptors
        //--------------------------------------------------------

        // Declare the function template that handles primitive types such as int, std::string, etc.:
        template<typename T>
        TypeDescriptor* getPrimitiveDescriptor();

        // A helper class to find TypeDescriptors in different ways:
        struct DefaultResolver
        {
            template<typename T>
            static char func(decltype(&T::Reflection));
            template<typename T>
            static int func(...);
            template<typename T>
            struct IsReflected
            {
                enum
                {
                    value = (sizeof(func<T>(nullptr)) == sizeof(char))
                };
            };

            // This version is called if T has a static member named "Reflection":
            template<typename T, typename std::enable_if<IsReflected<T>::value, int>::type = 0>
            static TypeDescriptor* get()
            {
                return &T::Reflection;
            }

            // This version is called otherwise:
            template<typename T, typename std::enable_if<!IsReflected<T>::value, int>::type = 0>
            static TypeDescriptor* get()
            {
                return getPrimitiveDescriptor<T>();
            }
        };

        // This is the primary class template for finding all TypeDescriptors:
        template<typename T>
        struct TypeResolver
        {
            static TypeDescriptor* get() { return DefaultResolver::get<T>(); }
        };

        //--------------------------------------------------------
        // Type descriptors for user-defined structs/classes
        //--------------------------------------------------------

        struct TypeDescriptor_Struct : TypeDescriptor
        {
            struct Member
            {
                const char*     name;
                size_t          offset;
                TypeDescriptor* type;
            };

            std::vector<Member> members;

            TypeDescriptor_Struct(void (*init)(TypeDescriptor_Struct*)) : TypeDescriptor {nullptr, 0} { init(this); }
            TypeDescriptor_Struct(const char* name, size_t size, const std::initializer_list<Member>& init) : TypeDescriptor {nullptr, 0}, members {init} {}
            virtual void dump(const void* obj, int indentLevel) const override
            {
                std::cout << name << " {" << std::endl;
                for (const Member& member : members)
                {
                    std::cout << std::string(4 * (indentLevel + 1), ' ') << member.name << " = ";
                    member.type->dump((char*)obj + member.offset, indentLevel + 1);
                    std::cout << std::endl;
                }
                std::cout << std::string(4 * indentLevel, ' ') << "}";
            }
        };

#define REFLECT() \
    friend struct Reflect::DefaultResolver; \
    static Reflect::TypeDescriptor_Struct Reflection; \
    static void                           initReflection(Reflect::TypeDescriptor_Struct*);

#define REFLECT_STRUCT_BEGIN(type) \
    Reflect::TypeDescriptor_Struct type::Reflection {type::initReflection}; \
    void                           type::initReflection(Reflect::TypeDescriptor_Struct* typeDesc) \
    { \
        using T           = type; \
        typeDesc->name    = #type; \
        typeDesc->size    = sizeof(T); \
        typeDesc->members = {

#define REFLECT_STRUCT_MEMBER(name) {#name, offsetof(T, name), Reflect::TypeResolver<decltype(T::name)>::get()},

#define REFLECT_STRUCT_END() \
    } \
    ; \
    }

        //--------------------------------------------------------
        // Type descriptors for std::vector
        //--------------------------------------------------------

        struct TypeDescriptor_StdVector : TypeDescriptor
        {
            TypeDescriptor* itemType;
            size_t (*getSize)(const void*);
            const void* (*getItem)(const void*, size_t);

            template<typename ItemType>
            TypeDescriptor_StdVector(ItemType*) : TypeDescriptor {"std::vector<>", sizeof(std::vector<ItemType>)}, itemType {TypeResolver<ItemType>::get()}
            {
                getSize = [](const void* vecPtr) -> size_t {
                    const auto& vec = *(const std::vector<ItemType>*)vecPtr;
                    return vec.size();
                };
                getItem = [](const void* vecPtr, size_t index) -> const void* {
                    const auto& vec = *(const std::vector<ItemType>*)vecPtr;
                    return &vec[index];
                };
            }
            virtual std::string getFullName() const override { return std::string("std::vector<") + itemType->getFullName() + ">"; }
            virtual void        dump(const void* obj, int indentLevel) const override
            {
                size_t numItems = getSize(obj);
                std::cout << getFullName();
                if (numItems == 0)
                {
                    std::cout << "{}";
                }
                else
                {
                    std::cout << "{" << std::endl;
                    for (size_t index = 0; index < numItems; index++)
                    {
                        std::cout << std::string(4 * (indentLevel + 1), ' ') << "[" << index << "] ";
                        itemType->dump(getItem(obj, index), indentLevel + 1);
                        std::cout << std::endl;
                    }
                    std::cout << std::string(4 * indentLevel, ' ') << "}";
                }
            }
        };

        // Partially specialize TypeResolver<> for std::vectors:
        template<typename T>
        class TypeResolver<std::vector<T>>
        {
        public:
            static TypeDescriptor* get()
            {
                static TypeDescriptor_StdVector typeDesc {(T*)nullptr};
                return &typeDesc;
            }
        };

        //--------------------------------------------------------
        // A type descriptor for int
        //--------------------------------------------------------

        struct TypeDescriptor_Int : TypeDescriptor
        {
            TypeDescriptor_Int() : TypeDescriptor {"int", sizeof(int)} {}
            virtual void dump(const void* obj, int /* unused */) const override { std::cout << "int{" << *(const int*)obj << "}"; }
        };

        template<>
        TypeDescriptor* getPrimitiveDescriptor<int>()
        {
            static TypeDescriptor_Int typeDesc;
            return &typeDesc;
        }

        //--------------------------------------------------------
        // A type descriptor for double
        //--------------------------------------------------------

        struct TypeDescriptor_Double : TypeDescriptor
        {
            TypeDescriptor_Double() : TypeDescriptor {"double", sizeof(double)} {}
            virtual void dump(const void* obj, int /* unused */) const override { std::cout << "double{" << *(const double*)obj << "}"; }
        };

        template<>
        TypeDescriptor* getPrimitiveDescriptor<double>()
        {
            static TypeDescriptor_Double typeDesc;
            return &typeDesc;
        }

        //--------------------------------------------------------
        // A type descriptor for float
        //--------------------------------------------------------

        struct TypeDescriptor_Float : TypeDescriptor
        {
            TypeDescriptor_Float() : TypeDescriptor {"float", sizeof(float)} {}
            virtual void dump(const void* obj, int /* unused */) const override { std::cout << "float{" << *(const double*)obj << "}"; }
        };

        template<>
        TypeDescriptor* getPrimitiveDescriptor<float>()
        {
            static TypeDescriptor_Float typeDesc;
            return &typeDesc;
        }

        //--------------------------------------------------------
        // A type descriptor for std::string
        //--------------------------------------------------------

        struct TypeDescriptor_StdString : TypeDescriptor
        {
            TypeDescriptor_StdString() : TypeDescriptor {"std::string", sizeof(std::string)} {}
            virtual void dump(const void* obj, int /* unused */) const override { std::cout << "std::string{\"" << *(const std::string*)obj << "\"}"; }
        };

        template<>
        TypeDescriptor* getPrimitiveDescriptor<std::string>()
        {
            static TypeDescriptor_StdString typeDesc;
            return &typeDesc;
        }

    } // namespace Reflect
} // namespace ArchViz
