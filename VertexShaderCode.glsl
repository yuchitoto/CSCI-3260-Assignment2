#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec2 vertexUV;
in layout(location = 2) vec3 obj_normal;

out vec2 UV;
out vec3 FragPos;
out vec3 normal;

uniform mat4 modelTransformMatrix;
uniform mat4 modelScalingMatrix;
uniform mat4 modelRotationMatrix;
uniform mat4 projectionMatrix;

void main()
{

	vec4 v = vec4(position, 1.0);
	vec4 new_position = modelTransformMatrix * modelScalingMatrix * modelRotationMatrix * v;
	FragPos = vec3(new_position);
	vec4 projectedPosition = projectionMatrix * new_position;
	gl_Position = projectedPosition;

	UV = vertexUV;
	normal = mat3(transpose(inverse(modelTransformMatrix * modelScalingMatrix * modelRotationMatrix))) * obj_normal;
}
