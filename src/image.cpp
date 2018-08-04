#include "image.h"

#include "SDL2/SDL_image.h"
#include <atomic>

namespace SDL {   
    std::atomic<bool> sdlImageFlag(false);
    
    image::loader::loader() {
        IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);        
    }
    
    image::loader::~loader() {        
        IMG_Quit();        
    }
    
    image::image() : m_internalSurface(nullptr) {
        
    }
    
    image::image(image&& o) {
        m_internalSurface = o.m_internalSurface;
        o.m_internalSurface = nullptr;
    }

    image::~image() {
        if (m_internalSurface != nullptr) {
            SDL_FreeSurface(m_internalSurface);
        }
    }
    
    bool image::load(const std::string& filename) {
        m_internalSurface = IMG_Load(filename.c_str());        
        return m_internalSurface != nullptr;
    }    
    
    SDL_Surface* image::internal_handle() {
        return m_internalSurface;
    }
}

