#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include "vec3.h"
#include <vector>

enum vec_type {VERTEX, NORMAL};
std::vector<vec3> load_obj_vecs(vec_type type, std::string file);