/* fonts for text output */
#ifndef __FONT_H__
#define __FONT_H__

#include <string>
#include <map>

#include "cg_primitives.h"

#define ALIGN_CENTER 0
#define ALIGN_LEFT   1
#define ALIGN_RIGHT  2

namespace atlas {
    class cg_shader;

    // simple font class supporting 2d and 3d fonts
    class cg_font : public cg_prim {
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
        cg_font(const char* fontName, int fontSize, float depth);
        // 2d font constructor (no depth)
        cg_font(const char* fontName, int fontSize);
        // default constructor
        cg_font();
        ~cg_font();

        // print 3d (aligned left/center/right), syntax like printf
        void render(cg_shader* shader, int alignment, const char* str, ...);
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

    typedef std::map<std::string, cg_font*> cg_fontMapper;

    // font manager
    class cg_font_manager
    {
        static cg_fontMapper m_fontMap;
        static cg_font* selected;
    public:
        cg_font_manager();
        ~cg_font_manager();
        void add_font(const std::string id, cg_font* fnt);
        cg_font* get_font(const std::string id);
        cg_font* select_font(const std::string id);

        cg_font* create_font(const char* fontName, const char* sysFontName, int fontSize);
        cg_font* create_font(const char* fontName, const char* sysFontName, int fontSize, float depth);

        cg_font* select_font() {
            return selected;
        }
    };

    cg_font_manager& get_font_manager();
}
#endif // __FONT_H__
