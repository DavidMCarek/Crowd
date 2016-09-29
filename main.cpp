#include <stdio.h>
#include <vector>
#include <algorithm>
#include "glUtils.h"
#include "loadShader.h"
#include "vec3.h"
#include "mat4.h"
#include "loadObj.h"

int g_gl_width = 640;
int g_gl_height = 480;

std::string cows = "triangulatedCowDos.obj";
std::string people = "Body_Mesh_Rigged.obj";
std::string file = cows;

float cam_sensitivity = 0.01;
float cam_speed = 0.05;
float entity_sensitivity = 0.025;
float entity_speed = 0.05;
float upper_map_bound = 10.0;
float lower_map_bound = -10.0;
float max_height = 1.0;
float min_height = 0.0;
float base_terrain_height = 0.0;
int entity_count = 30; // 1 to 9

GLFWwindow* g_window = NULL;

float bounds(float pos);

int main() {
	
	start_gl();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	float plane[] = {
		lower_map_bound, base_terrain_height, lower_map_bound,
		upper_map_bound, base_terrain_height, lower_map_bound,
		upper_map_bound, base_terrain_height, upper_map_bound,
		lower_map_bound, base_terrain_height, lower_map_bound,
		upper_map_bound, base_terrain_height, upper_map_bound,
		lower_map_bound, base_terrain_height, upper_map_bound
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

	const char* vertex_shader = load_shader(".\\vert.glsl");
	const char* fragment_shader = load_shader(".\\frag.glsl");

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

	vec3 cam_location = vec3(-1.0, 1.0, 0.0);
	vec3 cam_target = vec3(1.0, M_PI_4 + M_PI_2, 0.0);
	vec3 up = vec3(0, 1, 0);

	mat4 camMat = make_cam_mat(cam_location, to_cartesian(cam_target) + cam_location, up);

	mat4 persMat4 = perspective_projection(67, (float)g_gl_width / g_gl_height, 0.1, 100);

	GLuint persMat = glGetUniformLocation(shader_programme, "persMat");

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
	float y = base_terrain_height + 0.16;
	float z = 0.0;

	std::vector<vec3> entity_positions;

	for (int i = 1; i <= entity_count; i++) {
		if (i % 2 == 0) 
			entity_positions.push_back(vec3(x, y, z + (i / 2 * 0.5)));
		else 
			entity_positions.push_back(vec3(x, y, z - (i / 2 * 0.5)));
	}

	std::vector<float> entity_rotations;
	for (int i = 0; i < entity_count; i++) {
		entity_rotations.push_back(0.0);
	}

	std::vector<vec3> entity_colors;
	for (int i = 0; i < entity_count; i++) {
		entity_colors.push_back(vec3((rand() % 7892347) / 7892347.0 * 255.0, (rand() % 7892347) / 7892347.0 * 255.0, (rand() % 7892347) / 7892347.0 * 255.0));
	}

	int entity = 0;

	mat4 translation_mat1 = get_translation_mat(vec3(x, y, z));

	while (!glfwWindowShouldClose(g_window)) {
		static double previous_seconds = glfwGetTime();
		double current_seconds = glfwGetTime();
		double elapsed_seconds = current_seconds - previous_seconds;
		previous_seconds = current_seconds;

		update_fps_counter(g_window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, g_gl_width, g_gl_height);

		glUseProgram(shader_programme);

		glUniformMatrix4fv(persMat, 1, GL_TRUE, (persMat4 * camMat).mat);

		glUniformMatrix4fv(model_mat, 1, GL_TRUE, identity_mat().mat);

		glUniform3f(color, 0.75, 0.75, 0.75);
		glBindVertexArray(plane_vao);
		glDrawArrays(GL_TRIANGLES, 0, pointCount);

		for (int i = 0; i < entity_count; i++) {
			glUniform3f(color, entity_colors[i].x, entity_colors[i].y, entity_colors[i].z);
			glUniformMatrix4fv(model_mat, 1, GL_TRUE, (get_translation_mat(entity_positions[i]) * rotate_y_mat(entity_rotations[i]) * scale_mat(0.05)).mat);
			glBindVertexArray(entity_vao);
			glDrawArrays(GL_TRIANGLES, 0, points.size());
		}

		glfwPollEvents();

		if (GLFW_PRESS == glfwGetKey(g_window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(g_window, 1);
		}

		if (glfwGetKey(g_window, GLFW_KEY_S)) {
			if (entity == 0) {
				if (entity == 0) {
					vec3 movement_vec = vec3(cam_speed, M_PI_2, cam_target.z);
					cam_location = cam_location - to_cartesian(movement_vec);
					camMat = make_cam_mat(cam_location, to_cartesian(cam_target) + cam_location, up);
				}
			}
			else {
				entity_positions[entity - 1].x = entity_positions[entity - 1].x -
					(cosf(entity_rotations[entity - 1]) * entity_speed);

				entity_positions[entity - 1].z = entity_positions[entity - 1].z + 
					(sinf(entity_rotations[entity - 1]) * entity_speed);

				entity_positions[entity - 1].x = bounds(entity_positions[entity - 1].x);
				entity_positions[entity - 1].z = bounds(entity_positions[entity - 1].z);
			}
		}
		if (glfwGetKey(g_window, GLFW_KEY_W)) {
			if (entity == 0) {
				if (entity == 0) {
					vec3 movement_vec = vec3(cam_speed, M_PI_2, cam_target.z);
					cam_location = cam_location + to_cartesian(movement_vec);
					camMat = make_cam_mat(cam_location, to_cartesian(cam_target) + cam_location, up);
				}
			}
			else {
				entity_positions[entity - 1].x = entity_positions[entity - 1].x +
					(cosf(entity_rotations[entity - 1]) * entity_speed);

				entity_positions[entity - 1].z = entity_positions[entity - 1].z -
					(sinf(entity_rotations[entity - 1]) * entity_speed);

				entity_positions[entity - 1].x = bounds(entity_positions[entity - 1].x);
				entity_positions[entity - 1].z = bounds(entity_positions[entity - 1].z);
			}
		}
		if (glfwGetKey(g_window, GLFW_KEY_D)) {
			if (entity == 0) {
				vec3 movement_vec = vec3(cam_speed, M_PI_2, cam_target.z + M_PI_2);
				cam_location = cam_location + to_cartesian(movement_vec);
				camMat = make_cam_mat(cam_location, to_cartesian(cam_target) + cam_location, up);
			}
			else {
				entity_rotations[entity - 1] -= M_PI * entity_sensitivity;
				if (entity_rotations[entity - 1] < 0.0)
					entity_rotations[entity - 1] = 2 * M_PI;
			}
		}
		if (glfwGetKey(g_window, GLFW_KEY_A)) {
			if (entity == 0) {
				vec3 movement_vec = vec3(cam_speed, M_PI_2, cam_target.z - M_PI_2);
				cam_location = cam_location + to_cartesian(movement_vec);
				camMat = make_cam_mat(cam_location, to_cartesian(cam_target) + cam_location, up);
			}
			else {
				entity_rotations[entity - 1] += M_PI * entity_sensitivity;
				if (entity_rotations[entity - 1] >= 2 * M_PI)
					entity_rotations[entity - 1] = 0.0;
			}
		}
		if (glfwGetKey(g_window, GLFW_KEY_SPACE)) {
			if (entity == 0) {
				cam_location.y += cam_speed;
				camMat = make_cam_mat(cam_location, to_cartesian(cam_target) + cam_location, up);
			}
			else {
				entity_positions[entity - 1].y = std::min(entity_positions[entity - 1].y + entity_speed, max_height);
			}
		}
		if (glfwGetKey(g_window, GLFW_KEY_LEFT_SHIFT)) {
			if (entity == 0) {
				cam_location.y -= cam_speed;
				camMat = make_cam_mat(cam_location, to_cartesian(cam_target) + cam_location, up);
			}
			else {
				entity_positions[entity - 1].y = std::max(entity_positions[entity - 1].y - entity_speed, min_height);
			}
		}
		if (glfwGetKey(g_window, GLFW_KEY_LEFT)) {
			if (entity == 0) {
				cam_target.z -= M_PI * cam_sensitivity;
				if (cam_target.z < 0.0)
					cam_target.z = 2 * M_PI;

				camMat = make_cam_mat(cam_location, to_cartesian(cam_target) + cam_location, up);
			}
		}
		if (glfwGetKey(g_window, GLFW_KEY_RIGHT)) {
			if (entity == 0) {
				cam_target.z += M_PI * cam_sensitivity;
				if (cam_target.z >= 2 * M_PI)
					cam_target.z = 0.0;
				camMat = make_cam_mat(cam_location, to_cartesian(cam_target) + cam_location, up);
			}
		}
		if (glfwGetKey(g_window, GLFW_KEY_DOWN)) {
			if (entity == 0) {
				cam_target.y += M_PI * cam_sensitivity;
				if (cam_target.y >= M_PI - (M_PI / 64))
					cam_target.y = M_PI - (M_PI / 64);

				camMat = make_cam_mat(cam_location, to_cartesian(cam_target) + cam_location, up);
			}
		}
		if (glfwGetKey(g_window, GLFW_KEY_UP)) {
			if (entity == 0) {
				cam_target.y -= M_PI * cam_sensitivity;
				if (cam_target.y < (M_PI / 64))
					cam_target.y = (M_PI / 64);
				camMat = make_cam_mat(cam_location, to_cartesian(cam_target) + cam_location, up);
			}
		}
		if (glfwGetKey(g_window, GLFW_KEY_0)) {
			entity = 0;
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
		if (glfwGetKey(g_window, GLFW_KEY_4)) {
			entity = 4;
		}
		if (glfwGetKey(g_window, GLFW_KEY_5)) {
			entity = 5;
		}
		if (glfwGetKey(g_window, GLFW_KEY_6)) {
			entity = 6;
		}
		if (glfwGetKey(g_window, GLFW_KEY_7)) {
			entity = 7;
		}
		if (glfwGetKey(g_window, GLFW_KEY_8)) {
			entity = 8;
		}
		if (glfwGetKey(g_window, GLFW_KEY_9)) {
			entity = 9;
		}

		glfwSwapBuffers(g_window);
	}


	glfwTerminate();
	return 0;
}

float bounds(float pos) {
	if (pos > upper_map_bound)
		return upper_map_bound;
	if (pos < lower_map_bound)
		return lower_map_bound;
	return pos;
}