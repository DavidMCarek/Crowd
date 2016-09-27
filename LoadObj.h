#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include "vec3.h"
#include <vector>

std::vector<vec3> load_obj_vertices(std::string file);
std::vector<vec3> load_obj_normals(std::string file);