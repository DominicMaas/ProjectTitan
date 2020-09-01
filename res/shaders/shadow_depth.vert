#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 aPos;

layout(set=0, binding = 0) uniform Input
{
    mat4 lightSpaceMatrix;
    mat4 model;
    mat4 projection;
};

void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}