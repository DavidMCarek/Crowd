#include "LoadObj.h"
#include "strUtils.h"

std::vector<vec3> load_obj_vertices(std::string file) {
	std::ifstream obj;

	obj.open(file, std::ios::binary);

	if (obj.fail()) {
		std::cout << "failed to open " << file << std::endl;
	}

	std::string line;

	std::vector<vec3> verts;
	std::vector<int> faceToVertexList;
	std::vector<vec3> triangles;

	while (std::getline(obj, line)) {
		std::vector<std::string> strs = split(line, " ");

		if (strs[0] == "v") {
			float x = std::stof(strs[1]);
			float y = std::stof(strs[2]);
			float z = std::stof(strs[3]);

			vec3 vec = vec3(x, y, z);
			verts.push_back(vec);
		}
		else if (strs[0] == "f") {
			faceToVertexList.push_back(std::stoi(split(strs[1], "/")[0]) - 1);
			faceToVertexList.push_back(std::stoi(split(strs[2], "/")[0]) - 1);
			faceToVertexList.push_back(std::stoi(split(strs[3], "/")[0]) - 1);
		}
	}

	int faceCount = faceToVertexList.size();

	for (int i = 0; i < faceCount; i++) {
		
		vec3 v = verts[faceToVertexList[i]];
		triangles.push_back(v);
	}

	return triangles;
}

std::vector<vec3> load_obj_normals(std::string file)
{
	std::ifstream obj;

	obj.open(file, std::ios::binary);

	if (obj.fail()) {
		std::cout << "failed to open " << file << std::endl;
	}

	std::string line;

	std::vector<vec3> verts;
	std::vector<int> faceToNormalList;
	std::vector<vec3> normals;

	while (std::getline(obj, line)) {
		std::vector<std::string> strs = split(line, " ");

		if (strs[0] == "vn") {
			float x = std::stof(strs[1]);
			float y = std::stof(strs[2]);
			float z = std::stof(strs[3]);

			vec3 vec = vec3(x, y, z);
			verts.push_back(vec);
		}
		else if (strs[0] == "f") {
			faceToNormalList.push_back(std::stoi(split(strs[1], "/")[2]) - 1);
			faceToNormalList.push_back(std::stoi(split(strs[2], "/")[2]) - 1);
			faceToNormalList.push_back(std::stoi(split(strs[3], "/")[2]) - 1);
		}
	}

	int faceCount = faceToNormalList.size();

	for (int i = 0; i < faceCount; i++) {

		vec3 v = verts[faceToNormalList[i]];
		normals.push_back(v);
	}

	return normals;
}
