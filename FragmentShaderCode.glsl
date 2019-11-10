#version 430

in vec2 UV;
in vec3 normal;
in vec3 fragPos;

uniform vec3 light_pos;
uniform vec3 light_color;
uniform vec3 light_amb;
uniform vec3 k_ambient;
uniform vec3 k_diffuse;
uniform float shininess;
uniform vec3 k_specular;
uniform vec3 viewPos;

uniform sampler2D myTextureSampler0;
uniform sampler2D myTextureSmapler1;
out vec3 color;

void main()
{
	vec3 ambient = light_amb * k_ambient;

	vec3 tmp_color = texture(myTextureSampler0, UV).rgb;

	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(light_pos - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);

	vec3 diffuse = light_color * (diff * k_diffuse);

	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	vec3 specular = light_color * (spec * k_specular);

	color = (ambient + diffuse + specular) * tmp_color;
}
