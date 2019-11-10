#version 430

in vec2 UV;
in vec3 normal;
in vec3 fragPos;
uniform vec3 light_pos;
uniform vec3 light_color;
uniform vec3 light_amb;
uniform sampler2D myTextureSampler0;
uniform sampler2D myTextureSmapler1;
out vec3 color;

void main()
{
	float k_ambient = 0.7;
	vec3 ambient = k_ambient * light_amb;

	vec3 tmp_color = texture(myTextureSampler0, UV).rgb;

	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(light_pos - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * light_color;

	color = (ambient + diffuse) * tmp_color;
}
