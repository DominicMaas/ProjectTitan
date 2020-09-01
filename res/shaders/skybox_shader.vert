#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 aPos;

layout(set=0, binding = 0) uniform Input
{
    mat4 projection;
    mat4 view;
};

layout(location = 1) out vec3 TexCoords;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}  