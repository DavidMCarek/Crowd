#include "loadObj.h"
#include "strUtils.h"

std::vector<vec3> load_obj_vecs(vec_type type, std::string file) {
	std::ifstream obj;

	obj.open(file, std::ios::binary);

	if (obj.fail()) {
		std::cout << "failed to open " << file << std::endl;
	}

	std::string line;

	std::vector<vec3> dictionary_values;
	std::vector<int> dictionary_key;
	std::vector<vec3> complete_list;

	while (std::getline(obj, line)) {
		std::vector<std::string> strs = split(line, " ");

		if (strs[0] == (type == 0 ? "v" : "vn")) {
			float x = std::stof(strs[1]);
			float y = std::stof(strs[2]);
			float z = std::stof(strs[3]);

			vec3 vec = vec3(x, y, z);
			dictionary_values.push_back(vec);
		}
		else if (strs[0] == "f") {
			dictionary_key.push_back(std::stoi(split(strs[1], "/")[(type == 0 ? 0 : 2)]) - 1);
			dictionary_key.push_back(std::stoi(split(strs[2], "/")[(type == 0 ? 0 : 2)]) - 1);
			dictionary_key.push_back(std::stoi(split(strs[3], "/")[(type == 0 ? 0 : 2)]) - 1);
		}
	}

	int dictionary_size = dictionary_key.size();

	for (int i = 0; i < dictionary_size; i++) {

		vec3 v = dictionary_values[dictionary_key[i]];
		complete_list.push_back(v);
	}

	return complete_list;
}
