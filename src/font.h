#pragma once

#include "SDL_ttf.h"

class font {
public:
    class loader {
    public:
        loader() {
            TTF_Init();
        }
    };
    
    font(std::string& ttf, int size) {
        m_font = TTF_OpenFont(ttf.c_str(), size);
    }
    
    font(std::string ttf, int size) {
        m_font = TTF_OpenFont(ttf.c_str(), size);
    }
    
    font(font&& o) {
        m_font = o.m_font;
        o.m_font = nullptr;
    }
    
    font(const font& o) = delete;
    
    ~font() {
        if (m_font != nullptr) {
            TTF_CloseFont(m_font);
        }
    }
    
    bool valid() {
        return m_font != nullptr;
    }
    
    TTF_Font* internal_handle() {
        return m_font;
    }
private:
    TTF_Font* m_font = nullptr;
};
