#pragma once

#include "gui_element.h"
#include "../renderer.h"

namespace SDL::gui {
    class frame : public gui_element {
    public:
        frame(SDL::renderer& renderer);

        virtual void update_render_size(const size s);
        virtual void update_viewport(const rect r);
        virtual void update_region(const rect r);    
        void set_background_img(const std::string& file); 
        void set_background_tex(SDL::texture* tex);     
        void set_text(const std::string& text, color c);
        virtual void update();
        virtual void draw();
        void update_background_color(const color c);
        void set_as_target();
        void reset_target();
        void mouse(const int x, const int y, const int button);
        SDL::renderer& renderer();
    private:
        bool m_is_image = false;
        SDL::renderer& m_renderer;
        SDL::texture m_tex;
        SDL::texture* m_background_tex = nullptr;
        SDL::rect m_dest_frame;
        SDL::rect m_src_frame;
    };    
}
