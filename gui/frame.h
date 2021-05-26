#pragma once

#include "src/window.h"
#include "src/renderer.h"

#include <chrono>
#include <thread>
#include <cassert>
#include <iostream>

class frame {
public:
    frame(SDL::renderer& renderer) : m_renderer(renderer) {

    }

    void set_render_size(const int width, const int height) {
        m_dest_frame.x = 0;
        m_dest_frame.y = 0;
        m_dest_frame.w = width;
        m_dest_frame.h = height;

        m_src_frame.x = 0;
        m_src_frame.y = 0;
        m_src_frame.w = width;
        m_src_frame.h = height;

        m_width = width;
        m_height = height;

        m_tex = m_renderer.create_texture(width, height);
    }   
    
    std::pair<int, int> get_render_size() {
        return {m_width, m_height};
    }

    void set_size(const int width, const int height) {
        m_dest_frame.w = width;
        m_dest_frame.h = height;
    }

    std::pair<int, int> get_size() {
        return {m_dest_frame.w, m_dest_frame.h};
    }

    void set_position(const int x, const int y) {
        m_dest_frame.x = x;
        m_dest_frame.y = y;
    }

    std::pair<int, int> get_position() {
        return {m_dest_frame.x, m_dest_frame.y};
    }

    void set_view(const int x, const int y, const int width, const int height) {
        m_src_frame.x = x;
        m_src_frame.y = y;
        m_src_frame.w = width;
        m_src_frame.h = height;
    }
    
    std::pair<std::pair<int, int>, std::pair<int, int>> get_view() {
        return {{m_src_frame.x, m_src_frame.y}, {m_src_frame.w, m_src_frame.h}};
    }
    
    void update() {
        m_renderer.set_target(m_tex);

        m_renderer.update();

        m_renderer.reset_target();
    }
    
    bool contains(const int x, int y) {
        return m_dest_frame.x <= x && x < m_dest_frame.x + m_dest_frame.w &&
               m_dest_frame.y <= y && y < m_dest_frame.y + m_dest_frame.h;
    }

    void draw() {
        m_renderer.draw(m_tex, &m_src_frame, &m_dest_frame);
    }
    


    int m_width;
    int m_height;
    SDL::renderer& m_renderer;
    SDL::texture m_tex;
    SDL::rect m_dest_frame;
    SDL::rect m_src_frame;
};

