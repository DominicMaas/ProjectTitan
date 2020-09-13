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

layout( location = 0 ) out vec3 vert_texcoord;

void main()
{
    mat4 modelViewMatrix = modelUBO.model * sceneUBO.view;
    vec3 position = mat3(modelViewMatrix) * inPosition.xyz;
    gl_Position = (sceneUBO.proj * vec4( position, 0.0 )).xyzz;
    vert_texcoord = inPosition.xyz;
}
