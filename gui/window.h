#pragma once

#include "src/window.h"
#include "src/renderer.h"

#include "gui/frame.h"
#include "gui/window.h"

#include <chrono>
#include <thread>
#include <cassert>
#include <iostream>

class window {
public:
    window(SDL::renderer& renderer) : m_main(renderer), m_scroll_x(renderer), m_scroll_y(renderer), m_content(renderer) {
        m_main.set_render_size(200, 200);
        m_main.set_position(0, 0);

        m_scroll_y.set_render_size(16, 200-16);
        m_scroll_y.set_position(200-16, 0);

        m_scroll_x.set_render_size(200-16, 16);
        m_scroll_x.set_position(0, 200-16);

        
        m_content.set_render_size(800, 184);
        m_content.set_position(0,0);
        m_content.set_size(200-16, 200-16);
        m_content.set_view(0, 0, 200, 200);
        
        
        
        m_scroll_x.set_size(((200.0-16.0) / 800.0) * (200.0-16.0), 16);
        m_scroll_y.set_size(16, ((200.0-16.0) / 184.0) * (200.0-16.0));
    }

    void update() {
        m_main.update();
        m_scroll_x.update();
        m_scroll_y.update();
        
        m_main.draw();
        m_content.draw();
        m_scroll_x.draw();
        m_scroll_y.draw();
    }

    void mouse(const int x, const int y, const int button) {
        if (button != 0) {
            if (m_scroll_y.contains(x, y)) {
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
            m_scroll_y.set_position(
                m_scroll_y.get_position().first,
                std::max(
                    m_main.get_position().second,
                    std::min(
                        m_main.get_position().second + m_main.get_size().second - m_scroll_y.get_size().second - 16,
                        m_scroll_y.get_position().second + (y - dragged_y)
                    )
                )
            );
            
            auto scale_l = static_cast<float>(m_scroll_y.get_position().second) / static_cast<float>(m_main.get_size().second - 16);
            auto scale_r = (static_cast<float>(m_scroll_y.get_position().second + m_scroll_y.get_size().second)) / static_cast<float>(m_main.get_size().second - 16);
            
            
            auto old = m_content.get_view();
            auto rs = m_content.get_render_size();
            m_content.set_view(
                old.first.first,
                rs.second * scale_l,
                old.second.first,
                rs.second * scale_r - rs.second * scale_l
            );
            
            
        }

        dragged_y = y;
        
        if (button != 0) {
            if (m_scroll_x.contains(x, y)) {
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
            m_scroll_x.set_position(
                std::max(
                    m_main.get_position().first,
                    std::min(
                        m_main.get_position().first + m_main.get_size().first - m_scroll_x.get_size().first - 16,
                        m_scroll_x.get_position().first + (x - dragged_x)
                    )
                ),
                m_scroll_x.get_position().second
            );
            
            
            auto scale_l = static_cast<float>(m_scroll_x.get_position().first) / static_cast<float>(m_main.get_size().first - 16);
            auto scale_r = (static_cast<float>(m_scroll_x.get_position().first + m_scroll_x.get_size().first)) / static_cast<float>(m_main.get_size().first - 16);
            
            
            auto old = m_content.get_view();
            auto rs = m_content.get_render_size();
            m_content.set_view(
                rs.first * scale_l,
                old.first.second,
                rs.first * scale_r - rs.first * scale_l,
                old.second.second
            );
            
            
            
            
        }

        dragged_x = x;
    }

    float scale_x;
    
    bool is_dragged_x = false;
    int dragged_x;

    bool is_dragged_y = false;
    int dragged_y;

    frame m_main;
    frame m_scroll_x;
    frame m_scroll_y;
    
    
    frame m_content;
};

