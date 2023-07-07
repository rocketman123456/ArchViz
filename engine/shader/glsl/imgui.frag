#version 450 core

layout(location = 0) out vec4 outColor;

layout(set=0, binding=0) uniform sampler2D fontSampler;

layout (location = 0) in vec2 inUV;
layout (location = 1) in vec4 inColor;

void main() {
    outColor = inColor * texture(fontSampler, inUV);
}
