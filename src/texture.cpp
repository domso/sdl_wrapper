#include "texture.h"

#include "SDL2/SDL_image.h"

namespace SDL {        
    texture::texture() : m_internalTexture(nullptr, wrapped_SDL_DestroyTexture) {
        
    }
    
    texture::texture(SDL_Texture* tex) : m_internalTexture((std::byte*) tex, wrapped_SDL_DestroyTexture) {
        SDL_QueryTexture((SDL_Texture*) m_internalTexture.get(), &m_texInfo.format, &m_texInfo.access, &m_texInfo.width, &m_texInfo.height);
    }

    texture::texture(const texture& tex) {
        m_internalTexture = tex.m_internalTexture;
        m_texInfo = tex.m_texInfo;
    }
    
    texture::texture(texture&& tex) {
        m_internalTexture = tex.m_internalTexture;
        m_texInfo = tex.m_texInfo;
        tex.m_internalTexture = nullptr;
    }
    
    void texture::operator=(texture&& tex) {
        m_internalTexture = tex.m_internalTexture;
        m_texInfo = tex.m_texInfo;
        tex.m_internalTexture = nullptr;        
    }
    
    void texture::set_blend_mode(const SDL_BlendMode mode) {
        SDL_SetTextureBlendMode((SDL_Texture*) m_internalTexture.get(), mode);
    }
    
    int texture::width() {
        return m_texInfo.width;
    }
    
    int texture::height() {
        return m_texInfo.height;
    }
    
    int texture::access() {
        return m_texInfo.access;
    }
    
    uint32_t texture::format() {
        return m_texInfo.format;
    }
    
    SDL_Texture* texture::internal_handle() {
        return (SDL_Texture*) m_internalTexture.get();
    }
    
    void texture::wrapped_SDL_DestroyTexture(std::byte* ptr) {
        SDL_DestroyTexture((SDL_Texture*) ptr);
    }    
}
