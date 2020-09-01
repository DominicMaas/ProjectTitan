#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec4 position;

layout(set=0, binding = 0) uniform Input
{
    mat4 model;
    mat4 view;
    mat4 projection;
};

void main()
{
    gl_Position = projection * view * model * position;
}