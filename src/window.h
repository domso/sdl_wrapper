#ifndef gameClient_SDL_window_h
#define gameClient_SDL_window_h

#include <SDL2/SDL.h>
#include <string>

namespace SDL {
    typedef SDL_Rect rect;
    
    class window {
    public:        
        window();
        ~window();
        window(const window&) = delete;
        
        struct config {
            int width;
            int height;
            std::string title = "window";
            int x = SDL_WINDOWPOS_UNDEFINED;
            int y = SDL_WINDOWPOS_UNDEFINED;
            uint32_t flags = SDL_WINDOW_SHOWN;
        };
        
        bool open(const config& c);
        void close();
        
        bool is_open() const;       
        
        
        void update();
        
        SDL_Window* internal_handler();        
    private:
        SDL_Window* m_internalHandle;
    };
}

#endif
