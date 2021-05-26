#pragma once

#include <stdint.h>

namespace SDL::gui {
    class gui_element {
    public:
        struct rect {
            int x;
            int y;
            int width;
            int height;
        };

        struct size {
            int width;
            int height;
        };

        struct color {
            uint8_t r;
            uint8_t g;
            uint8_t b;
        };
                
        void set_render_size(const size s);
        void set_viewport(const rect r);
        void set_region(const rect r);
        void set_background_color(const color c);
        void enable();
        void disable();
        bool is_enabled() const;
        size render_size() const;
        rect viewport() const;
        rect region() const;
        color background_color() const;
        bool mouse_over (const int x, const int y) const;

        virtual void mouse(const int x, const int y, const int button) = 0;
        virtual void update_render_size(const size s) = 0;
        virtual void update_viewport(const rect r) = 0;
        virtual void update_region(const rect r) = 0;
        virtual void update() = 0;
        virtual void draw() = 0;
        virtual void update_background_color(const color c) = 0;
    protected:
        size m_render_size = {0, 0};
        rect m_viewport = {0, 0, 0, 0};
        rect m_region = {0, 0, 0, 0};
        color m_background_color;
        bool m_enable = false;    
    };
}
