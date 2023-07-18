#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(binding = 2) uniform LightObject {
    vec3 position;
    vec3 color;
} light;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    // ambient
    // float ambientStrength = 0.1;
    // vec3 ambient = ambientStrength * light.color;

    // diffuse 
    // vec3 norm = normalize(Normal);
    // vec3 lightDir = normalize(lightPos - FragPos);
    // float diff = max(dot(norm, lightDir), 0.0);
    // vec3 diffuse = diff * light.color;

    // vec3 result = (ambient + diffuse) * fragColor;
    // outColor = vec4(result, 1.0);

    outColor = vec4(fragColor, 1.0) * texture(texSampler, fragTexCoord);
}
