#ifndef gameClient_src_SDL_renderer_h
#define gameClient_src_SDL_renderer_h

#include <SDL2/SDL.h>
#include "window.h"
#include "image.h"
#include "texture.h"

namespace SDL {
    class renderer {
    public:        
        renderer();
        
        renderer(const renderer&) = delete;
        
        bool init(window& targetWindow);
        
        ~renderer();
        
        void clear();
        
        void update();
        
        void draw(texture& tex, const rect* sourceRect = nullptr, const rect* destRect = nullptr);
        
        void draw_line(const int srcX, const int srcY, const int destX, const int destY);        
        
        void draw_rect(const rect& box);  
        
        void draw_fill_rect(const rect& box);  
        
        void draw_point(const int x, const int y);
        
        void set_draw_color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a);
        
        texture create_texture(image& texImg);
        
        texture create_texture(const int width, const int height);
        
        void set_target(texture& target);
                
        void reset_target();
        
        int height();
        int width();
        
        struct blend_modes {
            constexpr static const SDL_BlendMode none = SDL_BLENDMODE_NONE;
            constexpr static const SDL_BlendMode blend = SDL_BLENDMODE_BLEND;
            constexpr static const SDL_BlendMode add = SDL_BLENDMODE_ADD;
            constexpr static const SDL_BlendMode mod = SDL_BLENDMODE_MOD;
        };
        
        void set_blend_mode(const SDL_BlendMode mode);
        
        
        
        
        SDL_Renderer* internal_handle();
    private:
        SDL_Renderer* m_internalHandle;
    };
}

#endif
