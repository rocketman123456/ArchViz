#pragma once
#include "runtime/core/meta/reflection/reflection.h"

namespace ArchViz
{
    // follow mtl file format https://www.fileformat.info/format/material/
    // std::string ambient_texture;            // map_Ka. For ambient or ambient occlusion.
    // std::string diffuse_texture;            // map_Kd
    // std::string specular_texture;           // map_Ks
    // std::string specular_highlight_texture; // map_Ns
    // std::string bump_texture;               // map_bump, map_Bump, bump
    // std::string displacement_texture;       // disp
    // std::string alpha_texture;              // map_d
    // std::string reflection_texture;         // refl

    enum class AlphaMode
    {
        /// Alpha value is ignored
        Opaque,
        /// Either full opaque or fully transparent
        Mask,
        /// Output is combined with the background
        Blend
    };

    REFLECTION_TYPE(MaterialRes)
    CLASS(MaterialRes, Fields)
    {
        REFLECTION_BODY(MaterialRes);

    public:
        std::string m_base_colour_texture_file;
        std::string m_metallic_roughness_texture_file;
        std::string m_normal_texture_file;
        std::string m_occlusion_texture_file;
        std::string m_emissive_texture_file;
    };
} // namespace ArchViz