/* fonts for text output */
#ifndef __FONT_H__
#define __FONT_H__

#include <string>
#include <map>

#include "primitives.h"

#define ALIGN_CENTER 0
#define ALIGN_LEFT   1
#define ALIGN_RIGHT  2

namespace atlas {
    class c_shader;

    // simple font class supporting 2d and 3d fonts
    class gl_font : public gl_prim {
        unsigned int listBase;				// display list base
        GLYPHMETRICSFLOAT gmf[256];			// holds orientation and placement

        unsigned int texID;		// font texture id
        unsigned int callList;	// font display list

        vec4 color2d;		// RGBA
        ivec2 screenPos;
        std::string m_fontName;
        int m_fontSize;
        float m_depth;

        void init();

    public:
        // 3d font constructor (depth)
        gl_font(const char* fontName, int fontSize, float depth);
        // 2d font constructor (no depth)
        gl_font(const char* fontName, int fontSize);
        // default constructor
        gl_font();
        ~gl_font();

        // print 3d (aligned left/center/right), syntax like printf
        void render(c_shader* shader, int alignment, const char* str, ...);
        // print 2d (no align), syntax like printf
        void render(const char* str, ...);

        // actual creation functions
        void build_3d(const char* name, int size, float depth);
        void build_2d(const char* name, int size);

        // explicit cleanup
        void clear();

        // text positioning
        // 2d positioning is in pixels
        void set_position(int x, int y) { screenPos = ivec2(x, y); }
        void set_color(const vec4& col) { color2d = col; }
    };

    typedef std::map<std::string, gl_font*> cg_fontMapper;

    // font manager
    class gl_font_manager
    {
        static cg_fontMapper m_fontMap;
        static gl_font* selected;
    public:
        gl_font_manager();
        ~gl_font_manager();
        void add_font(const std::string id, gl_font* fnt);
        gl_font* get_font(const std::string id);
        gl_font* select_font(const std::string id);

        gl_font* create_font(const char* fontName, const char* sysFontName, int fontSize);
        gl_font* create_font(const char* fontName, const char* sysFontName, int fontSize, float depth);

        gl_font* select_font() {
            return selected;
        }
    };

    gl_font_manager& get_font_manager();
}
#endif // __FONT_H__
