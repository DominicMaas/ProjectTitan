#version 450
#extension GL_ARB_separate_shader_objects : enable

struct Light {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

layout(set = 2, binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 inTexCoords;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inFragPos;
layout(location = 3) in vec3 inCamPos;
layout(location = 4) in Light inLight;

layout(location = 0) out vec4 outColor;

void main() {
    // Ambient
    vec3 ambient = inLight.ambient * texture(texSampler, inTexCoords).rgb;

    // Diffuse
    vec3 norm = normalize(inNormal);
    vec3 lightDir = normalize(-inLight.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = inLight.diffuse * diff * texture(texSampler, inTexCoords).rgb;

    // Specular
    //float specularStrength = 0.5;
    //vec3 viewDir = normalize(inCamPos - inFragPos);
    //vec3 reflectDir = reflect(-lightDir, norm);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    //vec3 specular = specularStrength * (spec * inLight.specular);

    vec3 result = ambient + diffuse;
    outColor = vec4(result, 1.0);
}