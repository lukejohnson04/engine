
#version 430 core
out vec4 FragColor;

in vec2 TexCoord;

uniform vec4 eyeColor;
uniform vec4 skinColor;
uniform sampler2D _texture;

void main()
{
    FragColor = texture(_texture, TexCoord);
    if (FragColor == vec4(40.0/255.0,130.0/255.0,0.0,1.0)) {
        FragColor = eyeColor;
    } else if ((FragColor == vec4(1.0,0.0,0.0,1.0) || FragColor == vec4(1.0,0.0,1.0,1.0) || FragColor == vec4(0.0,1.0,0.0,1.0) || FragColor == vec4(0.0,0.0,1.0,1.0) || FragColor == vec4(150.0/255.0,0.0,1.0,1.0) || FragColor == vec4(1.0,1.0,200.0/255.0,1.0) || FragColor == vec4(1.0,1.0,0.0,1.0))) {
        FragColor = skinColor;
    }
}
