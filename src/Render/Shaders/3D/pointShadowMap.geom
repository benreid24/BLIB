#version 450
#extension GL_GOOGLE_include_directive : require

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

#define LIGHT_CAM_SET_NUMBER 0
#define OBJECTS_SET_NUMBER 1
#include "./uniforms.glsl"

layout(location = 0) out vec4 FragPos;

void main()
{
    for(int face = 0; face < 6; ++face)
    {
        LightCamera camera = lightCameras.cameras[face];
        mat4 viewProj = camera.projection * camera.view;
        gl_Layer = face; // built-in variable that specifies to which face we render.
        for(int i = 0; i < 3; ++i) // for each triangle vertex
        {
            FragPos = gl_in[i].gl_Position;
            gl_Position = viewProj * FragPos;
            EmitVertex();
        }    
        EndPrimitive();
    }
}  
