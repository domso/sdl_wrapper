#pragma once

#include "src/window.h"
#include "src/renderer.h"

#include "gui/frame.h"

#include <chrono>
#include <thread>
#include <cassert>
#include <iostream>


class widget {
public:
    widget(SDL::renderer& renderer) : m_content(renderer) {        
        m_content.set_render_size(800, 184);
        m_content.set_position(0,0);
        m_content.set_size(200-16, 200-16);
        m_content.set_view(0, 0, 200, 200);
    }

    void update() {
        m_content.draw_cross();
        m_content.draw();
    }
    
    frame m_content;
};
