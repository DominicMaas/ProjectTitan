#version 450
#extension GL_ARB_separate_shader_objects : enable

struct Light {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

layout(set = 0, binding = 0) uniform SceneUBO {
    mat4 view;
    mat4 proj;
    Light light;
    vec3 camPos;
} sceneUBO;

layout(set = 1, binding = 0) uniform ModelUBO {
    mat4 model;
} modelUBO;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoords;

layout(location = 0) out vec2 outTexCoords;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outFragPos;
layout(location = 3) out vec3 outCamPos;
layout(location = 4) out Light outLight;

void main() {
    outTexCoords = inTexCoords;
    outLight = sceneUBO.light;
    outNormal = mat3(transpose(inverse(modelUBO.model))) * inNormal;
    outFragPos = vec3(modelUBO.model * vec4(inPosition, 1.0));
    outCamPos = sceneUBO.camPos;

    gl_Position = sceneUBO.proj * sceneUBO.view * vec4(outFragPos, 1.0);
}