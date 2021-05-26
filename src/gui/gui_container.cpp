#include "gui_container.h"

namespace SDL::gui {
    gui_container::gui_container(SDL::renderer& renderer) : m_main(renderer), m_scroll_x(renderer), m_scroll_y(renderer), m_content(renderer), m_root(*this) {
        m_scroll_x.set_background_color({125, 125, 125});
        m_scroll_y.set_background_color({125, 125, 125});
    }

    gui_container::gui_container(SDL::renderer& renderer, gui_container& root) : m_main(renderer), m_scroll_x(renderer), m_scroll_y(renderer), m_content(renderer), m_root(root) {
        m_scroll_x.set_background_color({125, 125, 125});
        m_scroll_y.set_background_color({125, 125, 125});
    }

    gui_container& gui_container::operator[](const std::string& name) {
        auto pos = name.find_first_of(".");

        if (pos != std::string::npos) {
            return child(name.substr(0, pos))[name.substr(pos + 1)];
        } else {
            return child(name);
        }
    }

    gui_container& gui_container::child(const std::string& name) {
        if (name == "") {
            return *this;
        } else {
            auto find = m_slaves.try_emplace(name);

            if (find.second) {
                find.first->second = std::make_unique<gui_container>(m_main.renderer(), m_root);
            }

            return *(find.first->second);
        }
    }

    void gui_container::set_background_img(const std::string& file) {
        m_content.set_background_img(file);
    }

    void gui_container::set_background_tex(SDL::texture* tex) {
        m_content.set_background_tex(tex);
    }
    
    void gui_container::set_text(const std::string& text, color c) {
        m_content.set_text(text, c);
    }

    void gui_container::update_render_size(const size s) {
        m_content.set_render_size(s);
    }

    void gui_container::update_viewport(const rect r) {
        m_content.set_viewport(r);
    }

    void gui_container::update_region(const rect r) {
        m_main.set_render_size({r.width, r.height});
        m_main.set_viewport(r);
        m_main.set_region(r);

        m_scroll_x.set_render_size({r.width - 16, 16});
        m_scroll_x.set_viewport({0, 0, r.width - 16, 16});
        m_scroll_x.set_region({r.x, r.y + r.height - 16, r.width/2 - 16, 16});

        m_scroll_y.set_render_size({16, r.height - 16});
        m_scroll_y.set_viewport({0, 0, 16, r.height - 16});
        m_scroll_y.set_region({r.x + r.width - 16, r.y, 16, r.height/2 - 16});

        m_content.set_region({r.x, r.y, r.width-16, r.height-16});


        auto view = m_content.viewport();
        m_scroll_x.set_region({
            r.x + static_cast<int>(m_content.region().width * (static_cast<float>(view.x) / static_cast<float>(m_content.render_size().width))),
            r.y + r.height - 16,
            static_cast<int>(m_content.region().width * (static_cast<float>(view.width) / static_cast<float>(m_content.render_size().width))),
            16
        });

        m_scroll_y.set_region({
            r.x + r.height - 16,
            r.y + static_cast<int>(m_content.region().height * (static_cast<float>(view.y) / static_cast<float>(m_content.render_size().height))),
            16,
            static_cast<int>(m_content.region().height * (static_cast<float>(view.height) / static_cast<float>(m_content.render_size().height)))
        });
    }

    void gui_container::update() {
        if (is_enabled()) {
            m_main.update();
            m_content.update();
            m_scroll_x.update();
            m_scroll_y.update();

            for (auto& w : m_slaves) {
                w.second->update();
            }

            m_content.set_as_target();

            for (auto& w : m_slaves) {
                w.second->draw();
            }

            m_content.reset_target();
        }
    }

    void gui_container::draw() {
        if (is_enabled()) {
            m_main.draw();

            m_content.draw();
            m_scroll_x.draw();
            m_scroll_y.draw();
        }
    }

    void gui_container::update_background_color(const color c) {
        m_main.set_background_color(c);
        m_content.set_background_color(c);
    }

    void gui_container::set_on_click(const std::function<void(gui_container&, gui_container&)> callback) {
        m_on_click = callback;
    }

    void gui_container::mouse(const int x, const int y, const int button) {

        if (m_content.mouse_over(x, y)) {
            int trans_x = m_content.viewport().x + m_content.viewport().width * static_cast<float>(x - m_content.region().x) / static_cast<float>(m_content.region().width);
            int trans_y = m_content.viewport().y + m_content.viewport().height * static_cast<float>(y - m_content.region().y) / static_cast<float>(m_content.region().height);

            for (auto& w : m_slaves) {
                w.second->mouse(trans_x, trans_y, button);
            }

            if (button != 0 && !is_clicked) {
                if (m_on_click != nullptr) {
                    m_on_click(*this, m_root);
                    is_clicked = true;
                }
            }
        }

        if (button == 0) {
            is_clicked = false;
        }

        if (button != 0) {
            if (m_scroll_y.mouse_over(x, y)) {
                if (!is_dragged_y) {
                    dragged_y = y;
                }
                is_dragged_y = true;

            }
        } else {
            is_dragged_y = false;
            dragged_y = 0;
        }

        if (is_dragged_y) {
            auto old_pos = m_scroll_y.region();

            old_pos.y = std::max(
                            m_main.region().y,
                            std::min(
                                m_main.region().y + m_main.region().height - old_pos.height - 16,
                                old_pos.y + (y - dragged_y)
                            )
                        );

            m_scroll_y.set_region(old_pos);


            auto scale_l = static_cast<float>(m_scroll_y.region().y - m_main.region().y) / static_cast<float>(m_main.region().height - 16);
            auto scale_r = (static_cast<float>(m_scroll_y.region().y - m_main.region().y + m_scroll_y.region().height)) / static_cast<float>(m_main.region().height - 16);


            auto old = m_content.viewport();
            auto rs = m_content.render_size();
            m_content.set_viewport(
            {
                old.x,
                (int)(rs.height * scale_l),
                old.width,
                int(rs.height * scale_r - rs.height * scale_l)
            }
            );


        }

        dragged_y = y;

        if (button != 0) {
            if (m_scroll_x.mouse_over(x, y)) {
                if (!is_dragged_x) {
                    dragged_x = x;
                }
                is_dragged_x = true;

            }
        } else {
            is_dragged_x = false;
            dragged_x = 0;
        }

        if (is_dragged_x) {

            auto old_pos = m_scroll_x.region();
            old_pos.x = std::max(
                            m_main.region().x,
                            std::min(
                                m_main.region().x + m_main.region().width - old_pos.width - 16,
                                old_pos.x + (x - dragged_x)
                            )
                        );

            m_scroll_x.set_region(old_pos);


            auto scale_l = static_cast<float>(m_scroll_x.region().x - m_main.region().x) / static_cast<float>(m_main.region().width - 16);
            auto scale_r = (static_cast<float>(m_scroll_x.region().x - m_main.region().x + m_scroll_x.region().width)) / static_cast<float>(m_main.region().width - 16);


            auto old = m_content.viewport();
            auto rs = m_content.render_size();

            m_content.set_viewport(
            {
                (int)(rs.width * scale_l),
                old.y,
                int(rs.width * scale_r - rs.width * scale_l),
                old.height
            }
            );


        }

        dragged_x = x;
    }
}
