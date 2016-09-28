#include <stdio.h>
#include "glUtils.h"
#include "LoadShaders.h"
#include "vec3.h"
#include "mat4.h"
#include "LoadObj.h"
#include <vector>
#include <algorithm>

int g_gl_width = 640;
int g_gl_height = 480;

std::string cows = "triangulatedCowDos.obj";
std::string people = "Body_Mesh_Rigged.obj";
std::string file = cows;

GLFWwindow* g_window = NULL;

int main() {
	
	start_gl();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	float plane[] = {
		-1.0, -1.0, -1.0,
		1.0, -1.0, -1.0,
		1.0, -1.0, 1.0,
		-1.0, -1.0, -1.0,
		1.0, -1.0, 1.0,
		-1.0, -1.0, 1.0
	};

	float plane_n[] = {
		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0
	};

	int pointCount = 6;

	GLuint plane_vao = 0;
	glGenVertexArrays(1, &plane_vao);
	glBindVertexArray(plane_vao);
	
	GLuint plane_vbo = 0;
	glGenBuffers(1, &plane_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, plane_vbo);
	glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), plane, GL_STATIC_DRAW);	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	GLuint plane_n_vbo = 0;
	glGenBuffers(1, &plane_n_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, plane_n_vbo);
	glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), plane_n, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	const char* vertex_shader = load_shaders_run(".\\vert.glsl");
	const char* fragment_shader = load_shaders_run(".\\frag.glsl");

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, NULL);
	glCompileShader(vs);
	check_shader(vs);

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, NULL);
	glCompileShader(fs);
	check_shader(fs);

	GLuint shader_programme = glCreateProgram();
	glAttachShader(shader_programme, fs);
	glAttachShader(shader_programme, vs);
	glLinkProgram(shader_programme);
	glUseProgram(shader_programme);
	glBindVertexArray(plane_vao);

	mat4 camMat = makeCameraMatrix(vec3(1, 1, 1), vec3(0, -1, 0), vec3(0, 1, 0));

	mat4 persMat4 = perspective_projection(67, (float)g_gl_width / g_gl_height, 0.1, 100);

	GLuint persMat = glGetUniformLocation(shader_programme, "persMat");

	glUniformMatrix4fv(persMat, 1, GL_TRUE, (persMat4 * camMat).mat);

	GLuint color = glGetUniformLocation(shader_programme, "in_color");

	GLuint model_mat = glGetUniformLocation(shader_programme, "model_mat");

	std::vector<vec3> points = load_obj_vecs(VERTEX, file);
	std::vector<vec3> normals = load_obj_vecs(NORMAL, file);

	GLuint entity_vao = 0;
	glGenVertexArrays(1, &entity_vao);
	glBindVertexArray(entity_vao);

	GLuint entity_vbo = 0;
	glGenBuffers(1, &entity_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, entity_vbo);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(vec3), points.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	GLuint entity_n_vbo = 0;
	glGenBuffers(1, &entity_n_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, entity_n_vbo);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), normals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	float angle = 0.0;

	float x = 0.0;
	float y = -0.82;
	float z = 0.0;

	vec3 entity_positions[] = {
		vec3(x, y, z),
		vec3(x + 0.5, y, z + 0.5),
		vec3(x - 0.5, y, z + 0.5)
	};

	float entity_rotations[] = {
		270.0,
		30.0,
		60.0
	};

	int entity = 1;

	mat4 translation_mat1 = getTranslation(vec3(x, y, z));

	while (!glfwWindowShouldClose(g_window)) {
		static double previous_seconds = glfwGetTime();
		double current_seconds = glfwGetTime();
		double elapsed_seconds = current_seconds - previous_seconds;
		previous_seconds = current_seconds;

		_update_fps_counter(g_window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, g_gl_width, g_gl_height);

		glUseProgram(shader_programme);

		if (angle >= 360)
			angle = 0.0;

		angle += 0.5;

		glUniformMatrix4fv(model_mat, 1, GL_TRUE, getRotY(angle).mat);

		glUniform3f(color, 0.75, 0.75, 0.75);
		glBindVertexArray(plane_vao);
		glDrawArrays(GL_TRIANGLES, 0, pointCount);

		glUniform3f(color, 0.75, 0.0, 0.75);
		glUniformMatrix4fv(model_mat, 1, GL_TRUE, (getRotY(angle) * getTranslation(entity_positions[0]) * getRotY(entity_rotations[0]) * scale_mat(0.025)).mat);
		glBindVertexArray(entity_vao);
		glDrawArrays(GL_TRIANGLES, 0, points.size());

		glUniform3f(color, 0.75, 0.9, 0.75);
		glUniformMatrix4fv(model_mat, 1, GL_TRUE, (getRotY(angle) * getTranslation(entity_positions[1]) * getRotY(entity_rotations[1]) * scale_mat(0.05)).mat);
		glBindVertexArray(entity_vao);
		glDrawArrays(GL_TRIANGLES, 0, points.size());

		glUniform3f(color, 0.5, 0.0, 0.75);
		glUniformMatrix4fv(model_mat, 1, GL_TRUE, (getRotY(angle) * getTranslation(entity_positions[2]) * getRotY(entity_rotations[2]) * scale_mat(0.05)).mat);
		glBindVertexArray(entity_vao);
		glDrawArrays(GL_TRIANGLES, 0, points.size());

		glfwPollEvents();

		if (GLFW_PRESS == glfwGetKey(g_window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(g_window, 1);
		}

		if (glfwGetKey(g_window, GLFW_KEY_D)) {
			entity_positions[entity - 1].x = std::min(entity_positions[entity - 1].x + 0.01, 1.0);
		}
		if (glfwGetKey(g_window, GLFW_KEY_A)) {
			entity_positions[entity - 1].x = std::max(entity_positions[entity - 1].x - 0.01, -1.0);
		}
		if (glfwGetKey(g_window, GLFW_KEY_W)) {
			entity_positions[entity - 1].z = std::min(entity_positions[entity - 1].z + 0.01, 1.0);
		}
		if (glfwGetKey(g_window, GLFW_KEY_S)) {
			entity_positions[entity - 1].z = std::max(entity_positions[entity - 1].z - 0.01, -1.0);
		}
		if (glfwGetKey(g_window, GLFW_KEY_UP)) {
			entity_positions[entity - 1].y = std::min(entity_positions[entity - 1].y + 0.01, 1.0);
		}
		if (glfwGetKey(g_window, GLFW_KEY_DOWN)) {
			entity_positions[entity - 1].y = std::max(entity_positions[entity - 1].y - 0.01, -1.0);
		}
		if (glfwGetKey(g_window, GLFW_KEY_LEFT)) {
			entity_rotations[entity - 1] += 2.0;
			if (entity_rotations[entity - 1] >= 360.0)
				entity_rotations[entity - 1] = 0.0;
		}
		if (glfwGetKey(g_window, GLFW_KEY_RIGHT)) {
			entity_rotations[entity - 1] -= 2.0;
			if (entity_rotations[entity - 1] <= 0.0)
				entity_rotations[entity - 1] = 360.0;
		}
		if (glfwGetKey(g_window, GLFW_KEY_1)) {
			entity = 1;
		}
		if (glfwGetKey(g_window, GLFW_KEY_2)) {
			entity = 2;
		}
		if (glfwGetKey(g_window, GLFW_KEY_3)) {
			entity = 3;
		}

		glfwSwapBuffers(g_window);
	}


	glfwTerminate();
	return 0;
}