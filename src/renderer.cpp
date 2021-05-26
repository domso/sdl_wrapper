#include "renderer.h"

#include "SDL_ttf.h"

namespace SDL {  
    renderer::renderer() : m_internalHandle(nullptr) {
    }
    
    bool renderer::init(window& targetWindow) {
        m_internalHandle = SDL_CreateRenderer(targetWindow.internal_handler(), -1, SDL_RENDERER_ACCELERATED);
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
        return m_internalHandle != nullptr;
    }        
    
    renderer::~renderer() {
        if (m_internalHandle != nullptr) {
            SDL_DestroyRenderer(m_internalHandle);
        }
    }
    
    void renderer::clear() {
        SDL_RenderClear(m_internalHandle);
    }
    
    void renderer::update() {
        SDL_RenderPresent(m_internalHandle);
    }      
        
    void renderer::draw(texture& tex, const rect* sourceRect, const rect* destRect) {
        SDL_RenderCopy(m_internalHandle, tex.internal_handle(), sourceRect, destRect);
    }
        
    void renderer::draw_line(const int srcX, const int srcY, const int destX, const int destY) {
        SDL_RenderDrawLine(m_internalHandle, srcX, srcY, destX, destY);
    }
    
    void renderer::draw_rect(const rect& box) {
        SDL_RenderDrawRect(m_internalHandle, &box);
    }
    
    void renderer::draw_fill_rect(const rect& box) {
        SDL_RenderFillRect(m_internalHandle, &box);
    }      
    
    void renderer::draw_point(const int x, const int y) {
        SDL_RenderDrawPoint(m_internalHandle, x, y);
    }
    
    void renderer::set_draw_color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) {
        SDL_SetRenderDrawColor(m_internalHandle, r, g, b, a);
    }
        
    texture renderer::create_texture(image& texImg) {
        texture tmp(SDL_CreateTextureFromSurface(m_internalHandle, texImg.internal_handle()));
        return tmp;
    }
    
    texture renderer::create_texture(const int width, const int height) {
        return SDL::texture(SDL_CreateTexture(m_internalHandle, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, width, height));
    }
    
    texture renderer::create_texture(font& textFont, const std::string& text, const uint8_t r, const uint8_t g, const uint8_t b) {
        SDL_Color textColor = {r, g, b, 255};
        if (textFont.internal_handle() == nullptr) {
            abort();
        }
        SDL_Surface* surface = TTF_RenderText_Blended(textFont.internal_handle(), text.c_str(), textColor);
        texture tmp(SDL_CreateTextureFromSurface(m_internalHandle, surface));
        
        SDL_FreeSurface(surface);
        return tmp;
    }
        
    void renderer::set_target(texture& target) {
        SDL_SetRenderTarget(m_internalHandle, target.internal_handle());
    }
    
    void renderer::reset_target() {
        SDL_SetRenderTarget(m_internalHandle, nullptr);
    }
    
    int renderer::height() {
        int h, w;        
        SDL_GetRendererOutputSize(m_internalHandle, &w, &h);
            
        return h;
    }
    
    int renderer::width() {
        int h, w;        
        SDL_GetRendererOutputSize(m_internalHandle, &w, &h);
            
        return w;        
    }
        
    void renderer::set_blend_mode(const SDL_BlendMode mode) {
        SDL_SetRenderDrawBlendMode(m_internalHandle, mode);
    }
        
    SDL_Renderer* renderer::internal_handle() {
        return m_internalHandle;
    }      
}

