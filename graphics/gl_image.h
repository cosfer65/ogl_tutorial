#pragma once

#include <string>
#include "matrix.h"

namespace atlas {
    int load_heightmap_image_matrix(const std::string& filename, matrix<float>& m, int resolution);
}
