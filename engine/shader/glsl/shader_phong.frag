#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(binding = 2) uniform LightObject {
    vec3 position;
    vec3 color;
} light;

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragColor;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec2 fragTexCoord;
layout(location = 4) in vec3 viewPos;

layout(location = 0) out vec4 outColor;

void main() {
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * light.color;

    // diffuse 
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.color;

    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * light.color;

    vec3 result = (ambient + diffuse + specular) * fragColor;

    outColor = vec4(result, 1.0) * texture(texSampler, fragTexCoord);
}
