#version 450
#extension GL_ARB_separate_shader_objects : enable

struct Light {
    vec3 color;
    vec3 direction;
    float ambient;
};

layout(set = 2, binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 inTexCoords;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in Light inLight;
layout(location = 6) in mat4 inView;

layout(location = 0) out vec4 outColor;

void main() {
    vec4 objectTexture = texture(texSampler, inTexCoords);

    //vec3 lightDir = vec3(inView * vec4(inLight.direction, 0.0));
    vec3 lightDir = inLight.direction;
    vec3 norm = normalize(inNormal);

    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * inLight.color;

    // Ambient
    float ambientStrength = inLight.ambient;
    vec3 ambient = ambientStrength * inLight.color;

    vec3 result = (ambient + diffuse);
    outColor = vec4(result, 1.0) * objectTexture;
}