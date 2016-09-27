#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include "GL\glew.h"

char *load_shaders_run(std::string file);
void check_shader(GLuint shader);

