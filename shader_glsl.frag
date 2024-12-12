#version 420 core
 
out vec4 fragment_colour;

in vec3 vert_pos_varying; // Vertex position coordinates received from the fragment shader
in vec3 vert_pos_transformed; // Transformed cube vertex position coordinates
in vec3 vertex_normal;
in vec2 texture_coordinates;
in float stress_value_varying;

uniform sampler2D image;
uniform vec3 camera_position;

const float low_stress_threshold = 20000f; 
const float high_stress_threshold = 45000f; // threshold determines color of nodes based on stress

void main()
{
	vec3 view_direction = normalize(camera_position - vert_pos_transformed);
 
	vec3 light_position = vec3(0.0, 5.0, 0.0); // A position used as a light source acts as a point light (Not a directional light)
	vec3 light_direction = normalize(vec3(light_position - vert_pos_transformed));	
	
	vec4 image_colour = texture(image, texture_coordinates);
 
	float ambient_factor = 0.65; // Intensity multiplier.
	vec4 ambient_result = vec4(ambient_factor * image_colour.rgb, 1.0);

	float diffuse_factor = 0.85;
	float diffuse_angle = max(dot(light_direction, vertex_normal), -0.45); // [-1.0 to 0] down to -1 results in darker lighting past 90 degrees.
	vec4 diffuse_result =  vec4(diffuse_factor * diffuse_angle * image_colour.rgb, 1.0);	
		
	vec3 specular_colour = vec3(1.0, 1.0, 1.0);
	vec3 reflect_direction = normalize(reflect(-light_direction, vertex_normal)); // Light direction is negated here.
	float specular_strength = pow(max(dot(view_direction, reflect_direction), 0), 32);
	vec4 specular_result = vec4(specular_colour * specular_strength, 1.0);

	
	vec4 lighting_effect = ambient_result + diffuse_result + specular_result;

	// note these lighting effects are not applied unless fragment_colour is multiplied by lighting_effect
	// it is off by default as it isn't useful when considering the intended use of the program

    if (stress_value_varying <= low_stress_threshold) {
        // Low stress: green
        fragment_colour = vec4(0.0, 1.0, 0.0, 1.0); // * lighting_effect; 
    } else if (stress_value_varying <= high_stress_threshold) {
        // Medium stress: yellow
        fragment_colour = vec4(1.0, 1.0, 0.0, 1.0); // * lighting_effect;
    } else {
        // High stress: red
        fragment_colour = vec4(1.0, 0.0, 0.0, 1.0); // * lighting_effect;
    }

	fragment_colour.a = 1.0; // full opacity.

}