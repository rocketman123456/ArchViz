#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragColor;
layout(location = 2) out vec3 fragNormal;
layout(location = 3) out vec2 fragTexCoord;
layout(location = 4) out vec3 viewPos;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);

    mat4 model_inv = inverse(ubo.model);
    mat4 view_inv = inverse(ubo.view);

    fragPos = vec3(ubo.model * vec4(inPosition, 1.0));
    fragColor = inColor;
    fragNormal = mat3(transpose(model_inv)) * inNormal;  
    fragTexCoord = inTexCoord;
    viewPos = vec3(view_inv[0][3], view_inv[1][3], view_inv[2][3]);
}
