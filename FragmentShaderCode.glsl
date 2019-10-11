#version 430

in vec2 UV;
uniform sampler2D myTextureSampler0;
uniform sampler2D myTextureSmapler1;
out vec3 color;

void main()
{
	 color = texture( myTextureSampler0, UV ).rgb;
}
