#include "gl_image.h"
#include "image.h"

namespace atlas {
    int load_heightmap_image_matrix(const std::string& filename, matrix<float>& m) {
        cg_image img;
        img.load(filename.c_str());

        int rows = img.height();
        int cols = img.width();

        unsigned char* data = img.image();

        if (!data) return 0;

        int res = 8;
        m.resize(rows/res, cols/res);

        int step = img.bytes_per_pixel();

        for (int r = 0; r < rows; r += res) {
            for (int c = 0; c < cols; c += res) {
                // read alpha channel
                //m(i, j) = (data[(i * w + j) * step + step - 1] / 255.0f);
                m(r / res, c / res) = (data[(r * cols + c) * step + step - 1] / 255.0f);
                //m(r/res, c/res) = (data[(r * cols + c) * step] / 255.0f);
                //m(r, c) = 0;
            }
        }

        return 1;
    }
}
