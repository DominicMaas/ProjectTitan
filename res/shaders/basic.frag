#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) out vec4 color;

void main()
{
    color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}