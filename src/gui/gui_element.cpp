#include "gui_element.h"
    
namespace SDL::gui {
    void gui_element::set_render_size(const gui_element::size s) {
        m_render_size = s;
        update_render_size(s);
    }

    void gui_element::set_viewport(const gui_element::rect r) {
        m_viewport = r;
        update_viewport(r);
    }

    void gui_element::set_region(const gui_element::rect r) {
        m_region = r;
        update_region(r);
    }

    void gui_element::set_background_color(const gui_element::color c) {
        m_background_color = c;
        update_background_color(c);
    }

    void gui_element::enable() {
        m_enable = true;
    }

    void gui_element::disable() {
        m_enable = false;
    }

    bool gui_element::is_enabled() const {
        return m_enable;
    }

    gui_element::size gui_element::render_size() const {
        return m_render_size;
    }

    gui_element::rect gui_element::viewport() const {
        return m_viewport;
    }

    gui_element::rect gui_element::region() const {
        return m_region;
    }

    gui_element::color gui_element::background_color() const {
        return m_background_color;
    }

    bool gui_element::mouse_over (const int x, const int y) const {
        return m_region.x <= x && x < m_region.x + m_region.width &&
                m_region.y <= y && y < m_region.y + m_region.height;
    }
}
