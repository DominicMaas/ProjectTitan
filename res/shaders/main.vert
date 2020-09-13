#version 450
#extension GL_ARB_separate_shader_objects : enable

struct Light {
    vec3 color;
    vec3 direction;
    float ambient;
};

layout(set = 0, binding = 0) uniform SceneUBO {
    mat4 view;
    mat4 proj;
    Light light;
} sceneUBO;

layout(set = 1, binding = 0) uniform ModelUBO {
    mat4 model;
} modelUBO;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoords;

layout(location = 0) out vec2 outTexCoords;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out Light outLight;
layout(location = 6) out mat4 outView;

void main() {
    gl_Position = sceneUBO.proj * sceneUBO.view * modelUBO.model * vec4(inPosition, 1.0);

    outTexCoords = inTexCoords;
    outLight = sceneUBO.light;
    outNormal = inNormal;
    outView = sceneUBO.view;
}