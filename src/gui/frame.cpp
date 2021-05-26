#include "frame.h"

namespace SDL::gui {
    frame::frame(SDL::renderer& renderer) : m_renderer(renderer) {

    }

    void frame::update_render_size(const size s) {
        m_tex = m_renderer.create_texture(s.width, s.height);
    }

    void frame::update_viewport(const rect r) {
        m_src_frame.x = r.x;
        m_src_frame.y = r.y;
        m_src_frame.w = r.width;
        m_src_frame.h = r.height;
    }

    void frame::update_region(const rect r) {
        m_dest_frame.x = r.x;
        m_dest_frame.y = r.y;
        m_dest_frame.w = r.width;
        m_dest_frame.h = r.height;
    }

    void frame::set_background_img(const std::string& file) {
        SDL::image img;
        
        if (img.load(file)) {
            m_tex = m_renderer.create_texture(img);            
            m_render_size = {m_tex.width(), m_tex.height()};
            
            update_viewport({0, 0, m_tex.width(), m_tex.height()});
            m_is_image = true;
        }        
    }

    void frame::set_background_tex(SDL::texture* tex) {
        m_background_tex = tex;
    }
    
    void frame::set_text(const std::string& text, color c) {
        auto f = font("/usr/share/fonts/fonts-go/Go-Medium.ttf", 24);
        m_tex = m_renderer.create_texture(f, text, c.r, c.g, c.b);        
        m_render_size = {m_tex.width(), m_tex.height()};
        update_viewport({0, 0, m_tex.width(), m_tex.height()});
        m_is_image = true;  
    }

    void frame::update() {
        m_renderer.set_target(m_tex);

        if (!m_is_image && m_background_tex == nullptr) {
            m_renderer.set_draw_color(background_color().r, background_color().g, background_color().b, 1);
            m_renderer.draw_fill_rect({0, 0, m_tex.width(), m_tex.height()});

            m_renderer.set_draw_color(255, 255, 255, 1);
        }
        m_renderer.update();

        m_renderer.reset_target();
    }

    void frame::draw() {
        if (m_background_tex == nullptr) {
            m_renderer.draw(m_tex, &m_src_frame, &m_dest_frame);
        } else {
            m_renderer.draw(*m_background_tex, &m_src_frame, &m_dest_frame);
        }        
    }

    void frame::update_background_color(const color) {

    }

    void frame::set_as_target() {
        m_renderer.set_target(m_tex);
    }

    void frame::reset_target() {
        m_renderer.reset_target();
    }

    void frame::mouse(const int, const int, const int) {
        
    }
    
    SDL::renderer& frame::renderer() {
        return m_renderer;
    }
}


