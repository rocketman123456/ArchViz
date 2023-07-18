#include "runtime/function/render/rhi/spirv_parser.h"

#include "runtime/core/base/macro.h"
#include "runtime/core/math/math.h"

#include <algorithm>
#include <string>
#include <vector>

namespace ArchViz
{
    namespace SPIRV
    {
        static constexpr uint32_t k_bindless_texture_binding = 10;

        struct Member
        {
            uint32_t    id_index;
            uint32_t    offset;
            std::string name;
        };

        struct Id
        {
            SpvOp    op;
            uint32_t set;
            uint32_t binding;

            // For integers and floats
            uint8_t width;
            uint8_t sign;

            // For arrays, vectors and matrices
            uint32_t type_index;
            uint32_t count;

            // For variables
            SpvStorageClass storage_class;

            // For constants
            uint32_t value;

            // For structs
            std::string         name;
            std::vector<Member> members;
        };

        VkShaderStageFlags parse_execution_model(SpvExecutionModel model)
        {
            switch (model)
            {
                case (SpvExecutionModelVertex): {
                    return VK_SHADER_STAGE_VERTEX_BIT;
                }
                case (SpvExecutionModelGeometry): {
                    return VK_SHADER_STAGE_GEOMETRY_BIT;
                }
                case (SpvExecutionModelFragment): {
                    return VK_SHADER_STAGE_FRAGMENT_BIT;
                }
                case (SpvExecutionModelKernel): {
                    return VK_SHADER_STAGE_COMPUTE_BIT;
                }
            }

            return 0;
        }

        void parse_binary(const std::vector<uint32_t>& data, std::string& name_buffer, ParseResult& parse_result)
        {
            uint32_t spv_word_count = safe_cast<uint32_t>(data.size());

            uint32_t magic_number = data[0];
            ASSERT(magic_number == 0x07230203);

            uint32_t id_bound = data[3];

            std::vector<Id> ids;
            ids.resize(id_bound);

            memset(ids.data(), 0, id_bound * sizeof(Id));

            VkShaderStageFlags stage;

            size_t word_index = 5;
            while (word_index < spv_word_count)
            {
                SpvOp    op         = (SpvOp)(data[word_index] & 0xFF);
                uint16_t word_count = (uint16_t)(data[word_index] >> 16);

                switch (op)
                {

                    case (SpvOpEntryPoint): {
                        ASSERT(word_count >= 4);

                        SpvExecutionModel model = (SpvExecutionModel)data[word_index + 1];

                        stage = parse_execution_model(model);
                        ASSERT(stage != 0);

                        break;
                    }

                    case (SpvOpDecorate): {
                        ASSERT(word_count >= 3);

                        uint32_t id_index = data[word_index + 1];
                        ASSERT(id_index < id_bound);

                        Id& id = ids[id_index];

                        SpvDecoration decoration = (SpvDecoration)data[word_index + 2];
                        switch (decoration)
                        {
                            case (SpvDecorationBinding): {
                                id.binding = data[word_index + 3];
                                break;
                            }

                            case (SpvDecorationDescriptorSet): {
                                id.set = data[word_index + 3];
                                break;
                            }
                        }

                        break;
                    }

                    case (SpvOpMemberDecorate): {
                        ASSERT(word_count >= 4);

                        uint32_t id_index = data[word_index + 1];
                        ASSERT(id_index < id_bound);

                        Id& id = ids[id_index];

                        uint32_t member_index = data[word_index + 2];

                        if (id.members.size() == 0)
                        {
                            id.members.resize(64);
                        }

                        Member& member = id.members[member_index];

                        SpvDecoration decoration = (SpvDecoration)data[word_index + 3];
                        switch (decoration)
                        {
                            case (SpvDecorationOffset): {
                                member.offset = data[word_index + 4];
                                break;
                            }
                        }

                        break;
                    }

                    case (SpvOpName): {
                        ASSERT(word_count >= 3);

                        uint32_t id_index = data[word_index + 1];
                        ASSERT(id_index < id_bound);

                        Id& id = ids[id_index];

                        char* name      = (char*)(data.data() + (word_index + 2));
                        auto  name_view = name_buffer + name;
                        id.name         = name_view;

                        break;
                    }

                    case (SpvOpMemberName): {
                        ASSERT(word_count >= 4);

                        uint32_t id_index = data[word_index + 1];
                        ASSERT(id_index < id_bound);

                        Id& id = ids[id_index];

                        uint32_t member_index = data[word_index + 2];

                        if (id.members.size() == 0)
                        {
                            id.members.resize(64);
                        }

                        Member& member = id.members[member_index];

                        char* name      = (char*)(data.data() + (word_index + 3));
                        auto  name_view = name_buffer + name;
                        member.name     = name_view;

                        break;
                    }

                    case (SpvOpTypeInt): {
                        ASSERT(word_count == 4);

                        uint32_t id_index = data[word_index + 1];
                        ASSERT(id_index < id_bound);

                        Id& id   = ids[id_index];
                        id.op    = op;
                        id.width = (uint8_t)data[word_index + 2];
                        id.sign  = (uint8_t)data[word_index + 3];

                        break;
                    }

                    case (SpvOpTypeFloat): {
                        ASSERT(word_count == 3);

                        uint32_t id_index = data[word_index + 1];
                        ASSERT(id_index < id_bound);

                        Id& id   = ids[id_index];
                        id.op    = op;
                        id.width = (uint8_t)data[word_index + 2];

                        break;
                    }

                    case (SpvOpTypeVector): {
                        ASSERT(word_count == 4);

                        uint32_t id_index = data[word_index + 1];
                        ASSERT(id_index < id_bound);

                        Id& id        = ids[id_index];
                        id.op         = op;
                        id.type_index = data[word_index + 2];
                        id.count      = data[word_index + 3];

                        break;
                    }

                    case (SpvOpTypeMatrix): {
                        ASSERT(word_count == 4);

                        uint32_t id_index = data[word_index + 1];
                        ASSERT(id_index < id_bound);

                        Id& id        = ids[id_index];
                        id.op         = op;
                        id.type_index = data[word_index + 2];
                        id.count      = data[word_index + 3];

                        break;
                    }

                    case (SpvOpTypeImage): {
                        // NOTE(marco): not sure we need this information just yet
                        ASSERT(word_count >= 9);

                        break;
                    }

                    case (SpvOpTypeSampler): {
                        ASSERT(word_count == 2);

                        uint32_t id_index = data[word_index + 1];
                        ASSERT(id_index < id_bound);

                        Id& id = ids[id_index];
                        id.op  = op;

                        break;
                    }

                    case (SpvOpTypeSampledImage): {
                        ASSERT(word_count == 3);

                        uint32_t id_index = data[word_index + 1];
                        ASSERT(id_index < id_bound);

                        Id& id = ids[id_index];
                        id.op  = op;

                        break;
                    }

                    case (SpvOpTypeArray): {
                        ASSERT(word_count == 4);

                        uint32_t id_index = data[word_index + 1];
                        ASSERT(id_index < id_bound);

                        Id& id        = ids[id_index];
                        id.op         = op;
                        id.type_index = data[word_index + 2];
                        id.count      = data[word_index + 3];

                        break;
                    }

                    case (SpvOpTypeRuntimeArray): {
                        ASSERT(word_count == 3);

                        uint32_t id_index = data[word_index + 1];
                        ASSERT(id_index < id_bound);

                        Id& id        = ids[id_index];
                        id.op         = op;
                        id.type_index = data[word_index + 2];

                        break;
                    }

                    case (SpvOpTypeStruct): {
                        ASSERT(word_count >= 2);

                        uint32_t id_index = data[word_index + 1];
                        ASSERT(id_index < id_bound);

                        Id& id = ids[id_index];
                        id.op  = op;

                        if (word_count > 2)
                        {
                            for (uint16_t member_index = 0; member_index < word_count - 2; ++member_index)
                            {
                                id.members[member_index].id_index = data[word_index + member_index + 2];
                            }
                        }

                        break;
                    }

                    case (SpvOpTypePointer): {
                        ASSERT(word_count == 4);

                        uint32_t id_index = data[word_index + 1];
                        ASSERT(id_index < id_bound);

                        Id& id        = ids[id_index];
                        id.op         = op;
                        id.type_index = data[word_index + 3];

                        break;
                    }

                    case (SpvOpConstant): {
                        ASSERT(word_count >= 4);

                        uint32_t id_index = data[word_index + 1];
                        ASSERT(id_index < id_bound);

                        Id& id        = ids[id_index];
                        id.op         = op;
                        id.type_index = data[word_index + 2];
                        id.value      = data[word_index + 3]; // NOTE(marco): we assume all constants to have maximum 32bit width

                        break;
                    }

                    case (SpvOpVariable): {
                        ASSERT(word_count >= 4);

                        uint32_t id_index = data[word_index + 2];
                        ASSERT(id_index < id_bound);

                        Id& id           = ids[id_index];
                        id.op            = op;
                        id.type_index    = data[word_index + 1];
                        id.storage_class = (SpvStorageClass)data[word_index + 3];

                        break;
                    }
                }

                word_index += word_count;
            }

            LOG_DEBUG("");
            LOG_DEBUG("SPRIV parse result: ");

            for (uint32_t id_index = 0; id_index < ids.size(); ++id_index)
            {
                Id& id = ids[id_index];

                if (id.op == SpvOpVariable)
                {
                    switch (id.storage_class)
                    {
                        case (SpvStorageClassUniform):
                        case (SpvStorageClassUniformConstant): {
                            if (id.set == 1 && (id.binding == k_bindless_texture_binding || id.binding == (k_bindless_texture_binding + 1)))
                            {
                                // NOTE(marco): these are managed by the GPU device
                                continue;
                            }

                            // NOTE(marco): get actual type
                            Id& uniform_type = ids[ids[id.type_index].type_index];

                            DescriptorSetLayoutCreation& setLayout = parse_result.sets[id.set];
                            setLayout.setSetIndex(id.set);

                            DescriptorSetLayoutCreation::Binding binding {};
                            binding.start = id.binding;
                            binding.count = 1;

                            switch (uniform_type.op)
                            {
                                case (SpvOpTypeStruct): {
                                    binding.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                                    binding.name = uniform_type.name.c_str();
                                    break;
                                }

                                case (SpvOpTypeSampledImage): {
                                    binding.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                                    binding.name = id.name.c_str();
                                    break;
                                }
                            }

                            setLayout.addBindingAtIndex(binding, id.binding);

                            parse_result.set_count = std::max(parse_result.set_count, (id.set + 1));

                            LOG_DEBUG("\t binding: name: {}, start: {}, count: {}", binding.name, binding.start, binding.count);

                            break;
                        }
                    }
                }
                id.members.clear();
            }
            ids.clear();

            LOG_DEBUG("SPRIV parse finish");
        }

    } // namespace SPIRV
} // namespace ArchViz
