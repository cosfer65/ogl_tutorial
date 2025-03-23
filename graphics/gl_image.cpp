#include "gl_image.h"
#include "image.h"

namespace atlas {
    int load_heightmap_image_matrix(const std::string& filename, matrix<float>& m, int resolution) {
        cg_image img;
        img.load(filename.c_str());

        int rows = img.height();
        int cols = img.width();

        unsigned char* data = img.image();

        if (!data) return 0;

        m.resize(rows/ resolution, cols/ resolution);

        int step = img.bytes_per_pixel();

        for (int r = 0; r < rows; r += resolution) {
            for (int c = 0; c < cols; c += resolution) {
                // read alpha channel
                m(r / resolution, c / resolution) = (data[(r * cols + c) * step + step - 1] / 255.0f);
            }
        }

        return 1;
    }
}
