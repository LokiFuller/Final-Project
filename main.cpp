#include <glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>
#include <iostream>
#include <fstream>

#include "load_model_meshes.h"
#include "shader_configure.h"

#include "fea.h"
#include "fea_stress.h"

// rotation variables
double lastX = 0.0, lastY = 0.0;
float yaw = 0.0f, pitch = 0.0f;
bool rotating = false;

// translation variables
float translateX = 0.0f, translateY = 0.0f;
bool translating = false;

// zoom variables
float zoomLevel = 3.0f;

// animations
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS)
		{
			rotating = true;
			glfwGetCursorPos(window, &lastX, &lastY);
		}
		else if (action == GLFW_RELEASE)
		{
			rotating = false;
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
	{
		if (action == GLFW_PRESS)
		{
			translating = true;
			glfwGetCursorPos(window, &lastX, &lastY);
		}
		else if (action == GLFW_RELEASE)
		{
			translating = false;
		}
	}
}
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (rotating)
	{
		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
		lastX = xpos;
		lastY = ypos;

		float sensitivity = 0.1f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		if (pitch > 89.0f) pitch = 89.0f;
		if (pitch < -89.0f) pitch = -89.0f;
	}
	else if (translating)
	{
		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;

		float translationSensitivity = 0.005f;
		translateX += xoffset * translationSensitivity;
		translateY += yoffset * translationSensitivity;
	}
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	float zoomSensitivity = 0.5f;  // Adjust this value to control zoom speed
	zoomLevel -= yoffset * zoomSensitivity;

	// Set boundaries for zoom level to prevent over-zooming
	if (zoomLevel < 1.0f) zoomLevel = 1.0f;      // Minimum zoom
	if (zoomLevel > 20.0f) zoomLevel = 20.0f;    // Maximum zoom
}

// gives access to vert_positions
std::vector<glm::vec3> vert_positions;

// swapping between geometric models (right arrow key)
int currentModelIndex = 0;
std::vector<Model> models;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Only respond to key press events
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
	{
		// Increment model index, wrapping around to 0 when reaching the end
		currentModelIndex = (currentModelIndex + 1) % models.size();
		std::cout << "Switched to model " << currentModelIndex << std::endl;
	}
}

int main()
{
	// initialize GLFW
	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_SAMPLES, 4);

	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	// window based on monitor size
	int monitor_width = mode->width;
	int monitor_height = mode->height;
	int window_width = (int)(monitor_width * 0.75f); 
	int window_height = (int)(monitor_height * 0.75f); // window will be 50% the monitor's size

	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Assimp Model Loading - Blender Object Files", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); // set the window to be used and then centre that window on the monitor
	glfwSetWindowPos(window, (monitor_width - window_width) / 2, (monitor_height - window_height) / 2);

	glfwSwapInterval(1); // Set VSync rate 1:1 with monitor's refresh rate.

	// initialize glew
	if (glewInit() != GLEW_OK) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glEnable(GL_DEPTH_TEST); // Enabling depth testing allows rear faces of 3D objects to be hidden behind front faces.
	glEnable(GL_MULTISAMPLE); // Anti-aliasing
	glEnable(GL_BLEND); // GL_BLEND for OpenGL transparency
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// compile shaders from text files

	const char* vert_shader = "shader_glsl.vert";
	const char* frag_shader = "shader_glsl.frag";

	Shader main_shader(vert_shader, frag_shader);
	main_shader.use();

	unsigned int view_matrix_loc = glGetUniformLocation(main_shader.ID, "view");
	unsigned int projection_matrix_loc = glGetUniformLocation(main_shader.ID, "projection");
	unsigned int camera_position_loc = glGetUniformLocation(main_shader.ID, "camera_position");

	// set camera position
	glm::vec3 camera_position(0.0f, 0.0f, -10.0f);
	glm::vec3 camera_target(0.0f, 0.0f, 0.0f);
	glm::vec3 camera_up(0.0f, 1.0f, 0.0f);

	glUniform3f(camera_position_loc, camera_position.x, camera_position.y, camera_position.z);

	glm::mat4 view = glm::lookAt(camera_position, camera_target, camera_up);
	glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, glm::value_ptr(view)); // Uniform: Transfer view matrix to vertex shader.

	glm::mat4 projection = glm::perspective(glm::radians(55.0f), (float)window_width / (float)window_height, 0.1f, 100.0f);
	glUniformMatrix4fv(projection_matrix_loc, 1, GL_FALSE, glm::value_ptr(projection));

	// enter the main loop
	
	models.push_back(Model("Glider2.gltf"));    // First Model
	models.push_back(Model("ship.gltf"));       // Second Model
	models.push_back(Model("plane.glb"));       // Third Model
	
	unsigned int animate_loc = glGetUniformLocation(main_shader.ID, "animate");

	for (const auto& mesh : models[0].mesh_list) {
		for (const auto& position : mesh.vert_positions) {
			vert_positions.push_back(position);
		}
	}

	std::cout << "Number of vertices in vert_positions: " << vert_positions.size() << std::endl;

	glActiveTexture(GL_TEXTURE0);
	unsigned int image_sampler_loc = glGetUniformLocation(main_shader.ID, "image");
	glUniform1i(image_sampler_loc, 0);

	initializeFEA(vert_positions);
	printFEAInfo();

	std::vector<float> nodal_forces(nodes.size(), 0.0f);
	calculateNodalForces(nodal_forces, elements, nodes);

	std::vector<float> stresses(nodes.size(), 0.0f);
	calculateStress(nodal_forces, nodes, elements, stresses);

	glfwSetKeyCallback(window, key_callback);

	glm::mat4 spinning_mat(1.0f);

	std::vector<float> stress_values;

	for (size_t i = 0; i < nodes.size(); ++i) {
		stress_values.push_back(stresses[i]);
	}

	while (!glfwWindowShouldClose(window)) // main loop
	{
		spinning_mat = glm::mat4(1.0f); // reset transformation matrix
		spinning_mat = glm::translate(spinning_mat, glm::vec3(translateX, translateY, 0.0f));
		spinning_mat = glm::rotate(spinning_mat, glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
		spinning_mat = glm::rotate(spinning_mat, glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(animate_loc, 1, GL_FALSE, glm::value_ptr(spinning_mat));

		camera_position.z = zoomLevel;
		glm::mat4 view = glm::lookAt(camera_position, camera_target, camera_up);
		glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, glm::value_ptr(view));

		glfwSetScrollCallback(window, scroll_callback);
		glfwSetMouseButtonCallback(window, mouse_button_callback);
		glfwSetCursorPosCallback(window, cursor_position_callback);

		// clear screen and draw model meshes

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Model& currentModel = models[currentModelIndex];

		for (unsigned int i = 0; i < currentModel.num_meshes; ++i)
		{
			// fill stress_values with the appropriate data for the current mesh
			std::vector<float> stress_values;
			for (size_t j = 0; j < currentModel.mesh_list[i].vert_positions.size(); ++j)
			{
				stress_values.push_back(stresses[j]);  // fill with the stress for each vertex
			}

			// set buffer data, including stress values
			currentModel.set_buffer_data(i, stress_values);

			// bind texture and draw mesh
			glBindTexture(GL_TEXTURE_2D, currentModel.mesh_list[i].tex_handle);
			glBindVertexArray(currentModel.mesh_list[i].VAO);
			glDrawElements(GL_TRIANGLES, (GLsizei)currentModel.mesh_list[i].vert_indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// exit application
	glDeleteProgram(main_shader.ID);

	glfwTerminate();

	exit(EXIT_SUCCESS);
}