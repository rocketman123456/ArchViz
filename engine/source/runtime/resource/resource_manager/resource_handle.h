#pragma once
#include "runtime/core/base/hash.h"

#include <atomic>
#include <cstdint>
#include <limits>

namespace ArchViz
{
    using ResourceIndex = uint32_t;

    struct ModelHandle
    {
        ResourceIndex index;
    }; // struct ModelHandle

    struct MeshHandle
    {
        ResourceIndex index;
    }; // struct MeshHandle

    struct MaterialHandle
    {
        ResourceIndex index;
    }; // struct MaterialHandle

    struct AudioHandle
    {
        ResourceIndex index;
    };

    struct BufferHandle
    {
        ResourceIndex index;
    }; // struct BufferHandle

    struct TextureHandle
    {
        ResourceIndex index;
    }; // struct TextureHandle

    //// ---------------------------------------------------------------------------
    //// ---------------------------------------------------------------------------
    //// ---------------------------------------------------------------------------
    //// ---------------------------------------------------------------------------
    //// ---------------------------------------------------------------------------

    constexpr ResourceIndex k_invalid_index = 0xffffffff;

    // Invalid handles
    // CPU side
    static ModelHandle    k_invalid_model {k_invalid_index};
    static MeshHandle     k_invalid_mesh {k_invalid_index};
    static MaterialHandle k_invalid_material {k_invalid_index};
    static AudioHandle    k_invalid_audio {k_invalid_index};
    static BufferHandle   k_invalid_buffer {k_invalid_index};
    static TextureHandle  k_invalid_texture {k_invalid_index};

    template<typename T>
    inline bool is_valid_handle(const T& handle)
    {
        return handle.index != k_invalid_index;
    }

    using ResourceTypeId = uint16_t;
    using MagicId        = uint16_t;
    using ResourceId     = uint64_t;

    constexpr MagicId        k_magic_id                 = 0x1234;
    constexpr ResourceTypeId k_invalid_resource_type_id = std::numeric_limits<uint16_t>::max();
    constexpr ResourceId     k_invalid_resource_id      = std::numeric_limits<uint64_t>::max();

    class ResourceTypeIdAllocator
    {
    public:
        static ResourceTypeId alloc();

    private:
        static std::atomic<ResourceTypeId> m_next_id;
    };

    class ResourceIdAllocator
    {
    public:
        static ResourceId alloc();

    private:
        static std::atomic<ResourceId> m_next_id;
    };

    struct ResourceHandle
    {
        MagicId        magic {k_magic_id};
        ResourceTypeId type;
        ResourceId     index;

        bool isValid(const ResourceHandle& handle) { return handle.index != k_invalid_index && handle.type != k_invalid_resource_type_id && handle.magic == k_magic_id; }

        bool operator==(const ResourceHandle& other) { return magic == other.magic && type == other.type && index == other.index; }
    };

    static ResourceHandle k_invalid_res_handle {0, k_invalid_resource_type_id, k_invalid_index};

    constexpr uint32_t k_max_resource_count = 1024;
    constexpr size_t   k_max_resource_size  = 1024 * 1024 * 40; // 40 Mb
} // namespace ArchViz

namespace std
{
    template<>
    struct hash<ArchViz::ResourceHandle>
    {
        size_t operator()(const ArchViz::ResourceHandle& handle) const
        {
            size_t handle_hash = 0;
            ArchViz::hash_combine(handle_hash, handle.magic, handle.type, handle.index);
            return handle_hash;
        }
    };

    template<>
    struct equal_to<ArchViz::ResourceHandle>
    {
        bool operator()(const ArchViz::ResourceHandle& lhs, const ArchViz::ResourceHandle& rhs) const
        {
            const std::hash<ArchViz::ResourceHandle> _hash_;
            return _hash_(lhs) == _hash_(rhs);
        }
    };
} // namespace std
