#shader vertex
#version 330 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec4 Colour;
layout(location = 2) in vec2 TexCoord;
layout(location = 3) in float TexIndex;

out vec4 vColour;
out vec2 vTexCoord;
out float vTexIndex;

uniform mat4 u_Proj;

void main() {
    vColour = Colour;
    vTexCoord = TexCoord;
    vTexIndex = TexIndex;

    gl_Position = u_Proj * vec4(Position.x, Position.y, Position.z, 1.0);
    
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 Color;

in vec4 vColour;
in vec2 vTexCoord;
in float vTexIndex;

uniform sampler2D Samplers[8];

void main() {
    int index = int(vTexIndex);
    Color = vColour * texture(Samplers[index], vTexCoord); //TexColour * u_Colour;
};