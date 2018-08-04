#ifndef gameClient_SDL_texture_h
#define gameClient_SDL_texture_h

#include <string>
#include <memory>
#include <SDL2/SDL.h>
#include "image.h"
#include  <cstddef>

namespace SDL {
    class texture {
    public:
        texture();
        texture(SDL_Texture* tex);
        texture(const texture& o);
        texture(texture&& o);
        
        void operator=(texture&& tex);
        
        struct blend_modes {
            constexpr static const SDL_BlendMode none  = SDL_BLENDMODE_NONE;
            constexpr static const SDL_BlendMode blend = SDL_BLENDMODE_BLEND;
            constexpr static const SDL_BlendMode add   = SDL_BLENDMODE_ADD;
            constexpr static const SDL_BlendMode mod   = SDL_BLENDMODE_MOD;
        };
        
        void set_blend_mode(const SDL_BlendMode mode);
        
        int width();
        int height();
        int access();
        uint32_t format();
        
        SDL_Texture* internal_handle();
    private:
        struct info {
            int width;
            int height;
            int access;
            uint32_t format;
        };
        
        info m_texInfo;
        // simple hack; SDL_Texture is an incomplete type (C ftw)
        // --> just use a byte-ptr and recast it...
        static void wrapped_SDL_DestroyTexture(std::byte* ptr);
        std::shared_ptr<std::byte> m_internalTexture;
    };   
}

#endif
