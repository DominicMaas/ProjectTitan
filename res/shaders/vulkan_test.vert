#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform SceneUBO {
    mat4 view;
    mat4 proj;
} sceneUBO;

layout(set = 1, binding = 0) uniform ModelUBO {
    mat4 model;
} modelUBO;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoords;

layout(location = 0) out vec2 fragTexCoord;

void main() {
    gl_Position = sceneUBO.proj * sceneUBO.view * modelUBO.model * vec4(inPosition, 1.0);
    fragTexCoord = inTexCoords;
}