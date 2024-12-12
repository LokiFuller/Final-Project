#version 420 core
 
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;  // basic data gathered from model for calculations
layout (location = 3) in float aStress;   // stress value

out vec3 vert_pos_varying; // vertex position coordinates passed to the fragment shader
out vec3 vert_pos_transformed;
out vec3 vertex_normal;
out vec2 texture_coordinates;
out float stress_value_varying;  // stress value passed to fragment shader

uniform mat4 view;
uniform mat4 projection;
uniform mat4 animate;
 
void main()
{
	vert_pos_varying = aPos; // used as colour values
	vert_pos_transformed = vec3(animate * vec4(aPos, 1.0)); // for lighting effects.
 
	texture_coordinates = aTexCoord;
 
	mat3 normal_matrix = transpose(inverse(mat3(animate)));
	vertex_normal = normal_matrix * aNormal;
	
	if (length(vertex_normal) > 0)
		vertex_normal = normalize(vertex_normal);

    stress_value_varying = aStress; // pass stress value to fragment shader

	gl_Position = projection * view * animate * vec4(aPos, 1.0);
}