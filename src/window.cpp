#include "window.h"

namespace SDL {
    window::window() : m_internalHandle(nullptr) {

    }
    
    window::~window() {
        if (m_internalHandle != nullptr) {
            SDL_DestroyWindow(m_internalHandle);
        }
    }

    bool window::open(const config& c) {
        m_internalHandle = SDL_CreateWindow(c.title.c_str(), c.x, c.y, c.width, c.height, c.flags);
        return m_internalHandle != nullptr;
    }

    void window::close() {
        if (m_internalHandle != nullptr) {
            SDL_DestroyWindow(m_internalHandle);
            m_internalHandle = nullptr;
        }
    }

    bool window::is_open() const {
        return m_internalHandle != nullptr;
    }

    SDL_Window* window::internal_handler() {
        return m_internalHandle;
    }
    
    void window::update() {
        SDL_Event events;
        while (SDL_PollEvent(&events)) {
            if (events.type == SDL_QUIT) {
                close();
                return;
            }
        }
    }
}

