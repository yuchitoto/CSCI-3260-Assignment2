#version 430

struct DirLight {
	vec3 dir;
	vec3 color;
};

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct PointLight {
	vec3 pos;
	vec3 color;
	float con;
	float lin;
	float quad;
};

struct SpotLight {
	struct PointLight light;
	vec3 dir;
	float cutoff;
};

in vec2 UV;
in vec3 normal;
in vec3 FragPos;

uniform vec3 viewPos;
uniform vec3 light_amb;
uniform DirLight directional_light;
uniform Material material;
uniform PointLight ptlt[6];
uniform SpotLight sptlt[3];

uniform sampler2D myTextureSampler0;
uniform sampler2D myTextureSmapler1;
out vec3 color;

vec3 calcDirLight(Material m, DirLight light, vec3 norm, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.dir);
	float diff = max(dot(norm, lightDir), 0.0);

	vec3 rDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, rDir), 0.0), m.shininess);

	vec3 diffuse = light.color * (diff * m.diffuse);
	vec3 specular = light.color * (spec * m.specular);

	return (diffuse + specular);
}

vec3 calcPointLight(Material m, PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.pos - fragPos);

	float diff = max(dot(norm, lightDir), 0.0);

	/*vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), m.shininess);*/
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfwayDir), 0.0), m.shininess);

	float dist = length(light.pos - fragPos);
	float attenuation = 1.0 / (light.con + light.lin * dist + light.quad * dist * dist);

	vec3 diffuse = light.color * (diff * m.diffuse) * attenuation;
	vec3 specular = light.color * (spec * m.specular) * attenuation;

	return (diffuse + specular);
}

vec3 calcSpotLight(Material m, SpotLight light, vec3 norm, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.light.pos - fragPos);
	float theta = dot(lightDir, normalize(-light.dir));
	vec3 retlt;
	if(theta > light.cutoff)
	{
		return calcPointLight(m, light.light, norm, fragPos, viewDir);
	}
	else
		return vec3(0.0);
}

void main()
{

	vec3 ambient = light_amb * material.ambient;
	vec3 tmp_color = texture(myTextureSampler0, UV).rgb;

	vec3 norm = normalize(normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	vec3 dLight = calcDirLight(material, directional_light, norm, viewDir);
	vec3 light = ambient + dLight;

	for(int k=0; k<6; k++)
		light += calcPointLight(material, ptlt[k], norm, FragPos, viewDir);

	for(int k=0; k<3; k++)
		light += calcSpotLight(material, sptlt[k], norm, FragPos, viewDir);

	color = light * tmp_color;
}
