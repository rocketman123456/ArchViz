#include "runtime/function/render/rhi/spirv_parser.h"

namespace ArchViz
{
    namespace SPIRV
    {
        constexpr uint32_t k_bindless_texture_binding = 10;

        struct Member
        {
            uint32_t id_index;
            uint32_t offset;

            // StringView name;
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
            // StringView      name;
            // Array<Member>   members;
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

        void parse_binary(const uint32_t* data, size_t data_size, const std::vector<uint8_t>& name_buffer, ParseResult* parse_result) {}
    } // namespace SPIRV
} // namespace ArchViz
