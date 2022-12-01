#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec3 color;
    vec3 normal;
} gs_in[];

out vec3 fColor;
out vec3 normal;
//out vec3 FragPos;

void qsplat(vec4 position)
{    
    fColor = gs_in[0].color; // gs_in[0] since there's only one input vertex
    normal = gs_in[0].normal;
    //gl_Position = position + vec4(normal*0.5,1.0); // 1:bottom-left
    //FragPos = gl_Position;
    gl_Position = position + vec4( 0.2, 0.2, 0.0, 0.0);
    EmitVertex();   
    gl_Position = position + vec4( 0.2, -0.2, 0.0, 0.0); // 2:bottom-right
    EmitVertex();
    gl_Position = position + vec4(-0.2,  0.2, 0.0, 0.0); // 3:top-left
    EmitVertex();
    //gl_Position = position + vec4( 0.2,  0.2, 0.0, 0.0); // 4:top-right
    //EmitVertex();
    EndPrimitive();
}

void main() {    
    qsplat(gl_in[0].gl_Position);
}