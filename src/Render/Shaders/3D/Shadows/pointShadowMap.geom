#version 450
#extension GL_GOOGLE_include_directive : require

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

#define LIGHT_CAM_SET_NUMBER 0
#define OBJECTS_SET_NUMBER 1
#include "3D/Helpers/uniforms.glsl"

layout(location = 0) out vec4 fragPos;

void main()
{
    for(int face = 0; face < 6; ++face)
    {
        mat4 viewProj = lightCameras.viewProj[face];
        gl_Layer = face; // built-in variable that specifies to which face we render.
        for(int i = 0; i < 3; ++i) // for each triangle vertex
        {
            fragPos = gl_in[i].gl_Position;
            gl_Position = viewProj * fragPos;
            EmitVertex();
        }    
        EndPrimitive();
    }
}  
