#pragma once

#include "src/window.h"
#include "src/renderer.h"
#include "src/font.h"


#include <chrono>
#include <thread>
#include <cassert>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <functional>

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

class gui_element {
public:
    void set_render_size(const size s) {
        m_render_size = s;
        update_render_size(s);
    }

    void set_viewport(const rect r) {
        m_viewport = r;
        update_viewport(r);
    }

    void set_region(const rect r) {
        m_region = r;
        update_region(r);
    }

    void set_background_color(const color c) {
        m_background_color = c;
        update_background_color(c);
    }

    void enable() {
        m_enable = true;
    }

    void disable() {
        m_enable = false;
    }

    bool is_enabled() {
        return m_enable;
    }

    size render_size() const {
        return m_render_size;
    }

    rect viewport() const {
        return m_viewport;
    }

    rect region() const {
        return m_region;
    }

    color background_color() const {
        return m_background_color;
    }

    bool mouse_over (const int x, const int y) {
        return m_region.x <= x && x < m_region.x + m_region.width &&
               m_region.y <= y && y < m_region.y + m_region.height;
    }



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


class frame : public gui_element {
public:
    frame(SDL::renderer& renderer) : m_renderer(renderer) {

    }

    virtual void update_render_size(const size s) {
        m_tex = m_renderer.create_texture(s.width, s.height);
    }

    virtual void update_viewport(const rect r) {
        m_src_frame.x = r.x;
        m_src_frame.y = r.y;
        m_src_frame.w = r.width;
        m_src_frame.h = r.height;
    }

    virtual void update_region(const rect r) {
        m_dest_frame.x = r.x;
        m_dest_frame.y = r.y;
        m_dest_frame.w = r.width;
        m_dest_frame.h = r.height;
    }
    
    void set_background_img(const std::string& file) {
        SDL::image img;
        
        if (img.load(file)) {
            m_tex = m_renderer.create_texture(img);            
            m_render_size = {m_tex.width(), m_tex.height()};
            
            update_viewport({0, 0, m_tex.width(), m_tex.height()});
            m_is_image = true;
        }        
    }
    
    void set_text(const std::string& text, color c) {
        auto f = font("/usr/share/fonts/fonts-go/Go-Medium.ttf", 24);
        m_tex = m_renderer.create_texture(f, text, c.r, c.g, c.b);        
        m_render_size = {m_tex.width(), m_tex.height()};
        update_viewport({0, 0, m_tex.width(), m_tex.height()});
        m_is_image = true;  
    }

    virtual void update() {
        m_renderer.set_target(m_tex);

        if (!m_is_image) {
            m_renderer.set_draw_color(background_color().r, background_color().g, background_color().b, 1);
            m_renderer.draw_fill_rect({0, 0, m_tex.width(), m_tex.height()});

            m_renderer.set_draw_color(255, 255, 255, 1);
//             m_renderer.draw_line(0, 0, render_size().width - 1, render_size().height - 1);

//             m_renderer.draw_line(render_size().width - 1, 0, 0, render_size().height - 1);
        }
        m_renderer.update();

        m_renderer.reset_target();
    }

    virtual void draw() {
        m_renderer.draw(m_tex, &m_src_frame, &m_dest_frame);
    }

    void update_background_color(const color c) {

    }

    void set_as_target() {
        m_renderer.set_target(m_tex);
    }

    void reset_target() {
        m_renderer.reset_target();
    }


    void mouse(const int x, const int y, const int button) {
    }

    bool m_is_image = false;
    SDL::renderer& m_renderer;
    SDL::texture m_tex;
    SDL::rect m_dest_frame;
    SDL::rect m_src_frame;
};

class window : public gui_element {
public:
    window(SDL::renderer& renderer) : m_main(renderer), m_scroll_x(renderer), m_scroll_y(renderer), m_content(renderer), m_root(*this) {
        m_scroll_x.set_background_color({125, 125, 125});
        m_scroll_y.set_background_color({125, 125, 125});
    }

    window(SDL::renderer& renderer, window& root) : m_main(renderer), m_scroll_x(renderer), m_scroll_y(renderer), m_content(renderer), m_root(root) {
        m_scroll_x.set_background_color({125, 125, 125});
        m_scroll_y.set_background_color({125, 125, 125});
    }
    
    window& operator[](const std::string& name) {
        auto pos = name.find_first_of(".");

        if (pos != std::string::npos) {
            return child(name.substr(0, pos))[name.substr(pos + 1)];
        } else {
            return child(name);
        }
    }

    window& child(const std::string& name) {
        if (name == "") {
            return *this;
        } else {
            auto find = m_slaves.try_emplace(name);

            if (find.second) {
                find.first->second = std::make_unique<window>(m_main.m_renderer, m_root);
            }

            return *(find.first->second);
        }
    }
    
    void set_background_img(const std::string& file) {
        m_content.set_background_img(file);
    }
    
    void set_text(const std::string& text, color c) {
        m_content.set_text(text, c);
    }

    virtual void update_render_size(const size s) {
        m_content.set_render_size(s);
    }

    virtual void update_viewport(const rect r) {
        m_content.set_viewport(r);
    }

    virtual void update_region(const rect r) {
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

    void update() {
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


    void draw() {
        if (is_enabled()) {
            m_main.draw();

            m_content.draw();
            m_scroll_x.draw();
            m_scroll_y.draw();
        }
    }

    void update_background_color(const color c) {
        m_main.set_background_color(c);
//         m_scroll_x.set_background_color(c);
//         m_scroll_y.set_background_color(c);
        m_content.set_background_color(c);
    }
    
    void set_on_click(const std::function<void(window&, window&)> callback) {
        m_on_click = callback;
    }

    void mouse(const int x, const int y, const int button) {

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

    std::unordered_map<std::string, std::unique_ptr<window>> m_slaves;


    float scale_x;

    bool is_dragged_x = false;
    int dragged_x;

    bool is_dragged_y = false;
    int dragged_y;

    frame m_main;
    frame m_scroll_x;
    frame m_scroll_y;

    frame m_content;
    
    window& m_root;
    
    bool is_clicked = false;
    
    std::function<void(window&, window&)> m_on_click;
};
