#shader vertex
#version 330 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec4 Colour;
layout(location = 2) in vec2 TextureCoordinate;
layout(location = 3) in float TextureSlot;

out vec4 QuadColour;
out vec2 QuadTextureCoordinate;
out float QuadTextureSlot;

uniform mat4 CameraProjection;

void main() {
    QuadColour = Colour;
    QuadTextureCoordinate = TextureCoordinate;
    QuadTextureSlot = TextureSlot;

    gl_Position = CameraProjection * vec4(Position, 1.0);
    
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 Color;

in vec4 QuadColour;
in vec2 QuadTextureCoordinate;
in float QuadTextureSlot;

uniform sampler2D Samplers[8];

void main() {
    int TextureSlot = int(QuadTextureSlot);
    Color = QuadColour * texture(Samplers[TextureSlot], QuadTextureCoordinate); //TexColour * u_Colour;
};