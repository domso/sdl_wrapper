#ifndef gameClient_SDL_image_h
#define gameClient_SDL_image_h

#include <string>
#include <SDL2/SDL.h>

namespace SDL {
    class image {
    public:
        // this class wrappes the constructor/destructor for the SDL_image libary
        class loader {
        public:
            loader();
            ~loader();
        };
                
        image();
        image(image&& o);
        image(const image& o) = delete;
        ~image();
             
        bool load(const std::string& filename);     
        
        SDL_Surface* internal_handle();
    private:
        SDL_Surface* m_internalSurface;
    };   
}

#endif

