#ifndef gameClient_SDL_util_h
#define gameClient_SDL_util_h

#include <cmath>

namespace SDL {
    namespace utilasd {
        template <typename T>
        T line_distance(const T x1, const T y1, const T x2, const T y2) {
            return std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
        }
        
        
    }
}

#endif
