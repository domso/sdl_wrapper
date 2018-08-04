#ifndef gameClient_SDL_res_manager_h
#define gameClient_SDL_res_manager_h


#include <atomic>
#include <thread>
#include "src/SDL/renderer.h"
#include "src/SDL/texture.h"
#include "src/SDL/image.h"
#include "src/util/obj_cache.h"
#include <assert.h>



namespace SDL {    
    class res_manager {
    public:    
        res_manager(SDL::renderer& renderer) : m_texAlloc(renderer), m_targetRenderer(renderer), m_textureCache(m_texAlloc) {
            m_runningFlag.store(true);
            m_updateThread = std::thread(&res_manager::updateRessources, this);
        }
        
        ~res_manager() {
            m_runningFlag.store(false);
            
            if (m_updateThread.joinable()) {
                m_updateThread.join();
            }
        }        
        
        void close() {
            m_runningFlag.store(false);
        }
        
        SDL::texture get(const uint64_t id) {
            return m_textureCache.load_sync(id);
        }        
    private:        
        struct texture_allocator {            
            texture_allocator(SDL::renderer& renderer) : m_targetRenderer(renderer) {
                
            }
            
            SDL::texture load(const uint64_t id) {
                SDL::image img;
                img.load(std::to_string(id) + ".png");
                return m_targetRenderer.create_texture(img);
            }
            
            SDL::renderer& m_targetRenderer;
        };
        
        void updateRessources() {
            while (m_runningFlag.load()) {
                m_textureCache.update(1);
            }
        }
                
        std::atomic<bool> m_runningFlag;
        texture_allocator m_texAlloc;
        SDL::renderer& m_targetRenderer;
        util::obj_cache<SDL::texture, SDL::res_manager::texture_allocator> m_textureCache;
        
        std::thread m_updateThread;
    };    
}
    

#endif
