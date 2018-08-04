#ifndef gameClient_data_map_h
#define gameClient_data_map_h

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <stdint.h>
#include "src/SDL/renderer.h"
#include "src/SDL/image.h"
#include "src/SDL/util.h"
#include "src/SDL/res_manager.h"
#include <random>

#include "src/region/context.h"

namespace data {
    class map {
    public:
        constexpr static const int resolution = 128;
        map(SDL::renderer& targetRenderer, region::context& context) : m_dataContext(context), m_tiles(resolution * resolution) {                       
            m_renderContext.renderWidth = targetRenderer.width();
            m_renderContext.renderHeight = targetRenderer.height();
        }      
        
        struct tile {
            uint32_t key;
            
            SDL::rect srcRectTop;
            SDL::rect srcRectBot;            
            SDL::rect destRect;

            bool valid = false;
        };
        
        struct normal_vector {
            double x;
            double y;
            double z;
            
            void calc_and_set(const int a1, const int a2, const int a3, const int b1, const int b2, const int b3) {
                x = a2 * b3 - a3 * b2;
                y = a3 * b1 - a1 * b3;
                z = a1 * b2 - a2 * b1;
            }
            
            void normalize() {
                double length = std::sqrt(x * x + y * y + z * z);
                x /= length;
                y /= length;
                z /= length;
                
                if (z <= 0) {
                    x = -x;
                    y = -y;
                    z = -z;
                }                    
            }
            
            double distance_rate(const normal_vector& inputVector) const{
                double result = 0;
                double tmp;
                
                tmp = std::abs(x - inputVector.x);
                result += tmp * tmp;
                
                tmp = std::abs(y - inputVector.y);
                result += tmp * tmp;
                
                tmp = std::abs(z - inputVector.z);
                result += tmp * tmp;
                
                result = std::sqrt(result) / 2;
                
                return result;                
            }
        };
        
        struct mask {
            int layerTop;
            int layerBot;
            
            SDL::rect srcRectTop;
            SDL::rect srcRectBot;
            
            normal_vector topNormal;
            normal_vector botNormal;
        };
                        
        void draw(SDL::renderer& targetRenderer, SDL::res_manager& resManager) {                                                       
            build_masks(targetRenderer);
            std::cout << "finish loading" << std::endl;
            normal_vector sunVector;
            sunVector.x = 0;
            sunVector.y = 1;
            sunVector.z = 1;
            
            sunVector.normalize();
            
            render_map(targetRenderer, sunVector, resManager);
            
            std::cout << "finish complete" << std::endl;
            
        }  
    private:        
        
        void build_masks(SDL::renderer& targetRenderer) {
            int x;
            int y;  
            
            m_renderTextures.clear();
            m_renderTextures.push_back(targetRenderer.create_texture(3000, 2000));
            targetRenderer.set_target(m_renderTextures[m_renderTextures.size() - 1]);                        
            targetRenderer.set_draw_color(0, 0, 0, 0);
            targetRenderer.clear();

            for (int i = 0; i < m_tiles.size(); i++) {
                x = i % resolution;
                y = i / resolution;
                
                if (x > 0 && (resolution - 1) > x && y > 0 && (resolution - 1) > y) { 
                    m_renderContext.deltaLeft  = m_renderContext.size / 2 + (m_dataContext[0][i] - m_dataContext[0][i + resolution]) * m_renderContext.scale;
                    m_renderContext.deltaRight = m_renderContext.size / 2 + (m_dataContext[0][i] - m_dataContext[0][i +   1]) * m_renderContext.scale;
                    m_renderContext.deltaBot   = m_renderContext.size     + (m_dataContext[0][i] - m_dataContext[0][i + (resolution + 1)]) * m_renderContext.scale;
                    
                    m_renderContext.deltaHeight = std::max<int>(std::max<int>(m_renderContext.deltaLeft, std::max<int>(m_renderContext.deltaRight, m_renderContext.deltaBot)), 0);
                    m_renderContext.offsetY      = std::min<int>(std::min<int>(m_renderContext.deltaLeft, std::min<int>(m_renderContext.deltaRight, m_renderContext.deltaBot)), 0);
                    m_renderContext.lineHeight  = std::max<int>(m_renderContext.deltaHeight - m_renderContext.offsetY, m_renderContext.lineHeight);

                    m_tiles[i].key = (((uint8_t)m_dataContext[0][i]) | ((uint8_t)m_dataContext[0][i + 1] << 8) | ((uint8_t)m_dataContext[0][i + resolution] << 16)| ((uint8_t)m_dataContext[0][i + resolution + 1] << 24));
                                                           
                    if (m_masks.count(m_tiles[i].key) == 0) {  
                        find_next_position_on_layer(targetRenderer);
                        add_new_mask_top(targetRenderer, i);
                        find_next_position_on_layer(targetRenderer);
                        add_new_mask_bot(targetRenderer, i);
                    }
                    
                    m_tiles[i].srcRectTop = m_masks[m_tiles[i].key].srcRectTop;
                    m_tiles[i].srcRectBot = m_masks[m_tiles[i].key].srcRectBot;
                    
                    m_tiles[i].destRect.w = 1 * (2 * (m_renderContext.size));
                    m_tiles[i].destRect.h = 1 * (m_renderContext.deltaHeight - m_renderContext.offsetY);
                    m_tiles[i].destRect.x = 1 * ((x - y) * (m_renderContext.size) - m_renderContext.size);
                    m_tiles[i].destRect.y = 1 * ((y + x) * m_renderContext.size/2 - m_dataContext[0][i] * m_renderContext.scale + m_renderContext.offsetY);
                    m_tiles[i].valid = true;                   
                }
            }  
            
            commit_new_mask_layer(targetRenderer);
        }
        
        void find_next_position_on_layer(SDL::renderer& targetRenderer) {
            int renderWidth = targetRenderer.width();
            int renderHeight = targetRenderer.height(); 
            int pad = m_renderContext.size * 2 + 2;
            m_renderContext.tileX += pad;
            
            if (renderWidth <= m_renderContext.tileX + pad + m_renderContext.size * 4) {
                m_renderContext.tileX = pad;
                m_renderContext.tileY += m_renderContext.lineHeight + pad;
                m_renderContext.lineHeight = 0;
            }
            
            if (renderHeight <= m_renderContext.tileY + m_renderContext.deltaHeight - m_renderContext.offsetY) {
                m_renderContext.tileX = pad;
                m_renderContext.tileY = pad;
                
                m_renderContext.lineHeight = 0;
                commit_new_mask_layer(targetRenderer);
                
                targetRenderer.update();
                m_renderTextures.push_back(targetRenderer.create_texture(3000, 2000));
                targetRenderer.set_target(m_renderTextures[m_renderTextures.size() - 1]);
                targetRenderer.set_draw_color(0, 0, 0, 0);
                targetRenderer.clear();
            }
        }
        
        
        void add_new_mask_top(SDL::renderer& targetRenderer, const int i) {
            targetRenderer.set_draw_color(255, 255 * (m_renderContext.tileCounter & 1), 255, 255);                                               
            
            targetRenderer.draw_line(m_renderContext.tileX, m_renderContext.tileY - m_renderContext.offsetY, m_renderContext.tileX - m_renderContext.size, m_renderContext.tileY - m_renderContext.offsetY + m_renderContext.deltaLeft);
            targetRenderer.draw_line(m_renderContext.tileX - m_renderContext.size, m_renderContext.tileY - m_renderContext.offsetY + m_renderContext.deltaLeft, m_renderContext.tileX + m_renderContext.size, m_renderContext.tileY - m_renderContext.offsetY + m_renderContext.deltaRight);                
            targetRenderer.draw_line(m_renderContext.tileX, m_renderContext.tileY - m_renderContext.offsetY, m_renderContext.tileX + m_renderContext.size, m_renderContext.tileY - m_renderContext.offsetY + m_renderContext.deltaRight);

            m_masks[m_tiles[i].key].layerTop = m_renderTextures.size() - 1;
            
            m_masks[m_tiles[i].key].topNormal.calc_and_set(0, 1, m_dataContext[0][i + resolution] - m_dataContext[0][i], 1, 0, m_dataContext[0][i + 1] - m_dataContext[0][i]);   
            m_masks[m_tiles[i].key].topNormal.normalize();                      
            
            m_masks[m_tiles[i].key].srcRectTop.w = 2 * (m_renderContext.size);
            m_masks[m_tiles[i].key].srcRectTop.h = m_renderContext.deltaHeight - m_renderContext.offsetY;
            m_masks[m_tiles[i].key].srcRectTop.x = m_renderContext.tileX - m_renderContext.size;
            m_masks[m_tiles[i].key].srcRectTop.y = m_renderContext.tileY;

            m_renderContext.tileCounter++;
        }
                                    
        void add_new_mask_bot(SDL::renderer& targetRenderer, const int i) {            
            targetRenderer.set_draw_color(255, 255 * (m_renderContext.tileCounter & 1), 255, 255);                                               
            
            targetRenderer.draw_line(m_renderContext.tileX - m_renderContext.size, m_renderContext.tileY - m_renderContext.offsetY + m_renderContext.deltaLeft, m_renderContext.tileX + m_renderContext.size, m_renderContext.tileY - m_renderContext.offsetY + m_renderContext.deltaRight);
            targetRenderer.draw_line(m_renderContext.tileX - m_renderContext.size, m_renderContext.tileY - m_renderContext.offsetY + m_renderContext.deltaLeft, m_renderContext.tileX, m_renderContext.tileY - m_renderContext.offsetY + m_renderContext.deltaBot);    
            targetRenderer.draw_line(m_renderContext.tileX + m_renderContext.size, m_renderContext.tileY - m_renderContext.offsetY + m_renderContext.deltaRight, m_renderContext.tileX, m_renderContext.tileY - m_renderContext.offsetY + m_renderContext.deltaBot);

            m_masks[m_tiles[i].key].layerBot = m_renderTextures.size() - 1;
            
            m_masks[m_tiles[i].key].botNormal.calc_and_set(-1, 0, m_dataContext[0][i + resolution] - m_dataContext[0][i + resolution + 1], 0, -1, m_dataContext[0][i + 1] - m_dataContext[0][i + resolution + 1]);                                                m_masks[m_tiles[i].key].botNormal.normalize();                      
            
            m_masks[m_tiles[i].key].srcRectBot.w = 2 * (m_renderContext.size);
            m_masks[m_tiles[i].key].srcRectBot.h = m_renderContext.deltaHeight - m_renderContext.offsetY;
            m_masks[m_tiles[i].key].srcRectBot.x = m_renderContext.tileX - m_renderContext.size;
            m_masks[m_tiles[i].key].srcRectBot.y = m_renderContext.tileY;

            m_renderContext.tileCounter++;
        }        
        
        void commit_new_mask_layer(SDL::renderer& targetRenderer) {
            int x;
            int y;
            uint32_t tileColor;
            int max;
            int renderWidth = targetRenderer.width();
            int renderHeight = targetRenderer.height();          
            
            std::vector<uint32_t> buffer(renderWidth * renderHeight);
            targetRenderer.update();                            
            SDL_RenderReadPixels(targetRenderer.internal_handle(), nullptr, 0, buffer.data(), renderWidth * 4);  

            for (int i = 0; i < buffer.size(); i++) {
                x = i % renderWidth;
                y = i / renderWidth;
                if (buffer[i] != 0) {                   
                    tileColor = buffer[i];
                    targetRenderer.set_draw_color(255, 255, 255, 255);
                    for (int j = i; j < (i + 2 * m_renderContext.size + 2); j++) {
                        if (buffer[j] == tileColor) {
                            max = j;
                            x = j % renderWidth;
                            y = j / renderWidth;
                            targetRenderer.draw_point(x,y);
                        }
                    }
                    targetRenderer.set_draw_color(255, 255, 255, 255);
                    for (;i < max; i++) {
                        x = i % renderWidth;
                        y = i / renderWidth;
                        if (buffer[i] == 0) {
                            targetRenderer.draw_point(x,y);
                        }
                    }
                }
            }
        }
        
        void create_render_texture(SDL::renderer& targetRenderer) {
            int minX = 0x7FFFFFFF;
            int maxX = 0;
            int minY = 0x7FFFFFFF;
            int maxY = 0;
            
            for (tile& t : m_tiles) {                                
                if (t.valid) {   
                    minX = std::min(minX, t.destRect.x);
                    minY = std::min(minY, t.destRect.y);
                    
                    maxX = std::max(maxX, t.destRect.x + t.destRect.w);
                    maxY = std::max(maxY, t.destRect.y + t.destRect.h);
                }
            }
            
            for (tile& t : m_tiles) {                                
                if (t.valid) {   
                    t.destRect.x -= minX;
                    t.destRect.y -= minY;
                }
            }
            
            m_renderOutput = targetRenderer.create_texture(maxX - minX, maxY - maxY);
        }
        
        void render_map(SDL::renderer& targetRenderer, const normal_vector& sunVector, SDL::res_manager& resManager) {
            int8_t drawColor;
            SDL::rect box;
                    
            targetRenderer.update();   
            targetRenderer.reset_target();
            
            SDL::image grasTile;
            grasTile.load("tile_scale.bmp");
            
            SDL::texture grasTexture = targetRenderer.create_texture(grasTile);
            SDL::texture drawTexture = targetRenderer.create_texture(128, 64);
                                   
            box.h = 64;
            box.w = 128;
            box.x = 0;
            box.y = 0;    
            
            std::srand(0);
            
            create_render_texture(targetRenderer);
                       
            for (tile& t : m_tiles) {                                
                if (t.valid) {                    
                    mask& currentMask = m_masks[t.key];
                    SDL::texture& currentMaskTextureTop = m_renderTextures[currentMask.layerTop];
                    SDL::texture& currentMaskTextureBot = m_renderTextures[currentMask.layerBot];
 
                    targetRenderer.set_target(drawTexture);
                    currentMaskTextureTop.set_blend_mode(SDL::texture::blend_modes::none);       
                    
                    targetRenderer.draw(currentMaskTextureTop, &t.srcRectTop);
                    drawColor = (1 - sunVector.distance_rate(currentMask.topNormal)) * 255;
                    
                    targetRenderer.set_blend_mode(SDL::renderer::blend_modes::mod);
                    targetRenderer.set_draw_color(drawColor, drawColor, drawColor, 255);
                    targetRenderer.draw_fill_rect(box);
                    
                    grasTexture.set_blend_mode(SDL::texture::blend_modes::mod);
                    targetRenderer.draw(grasTexture);
                                        
                    targetRenderer.set_target(m_renderOutput);
                    drawTexture.set_blend_mode(SDL::texture::blend_modes::blend);
                    targetRenderer.draw(drawTexture, nullptr, &t.destRect);
                    
                    
                    targetRenderer.set_target(drawTexture);
                    currentMaskTextureBot.set_blend_mode(SDL::texture::blend_modes::none);       
                    
                    targetRenderer.draw(currentMaskTextureBot, &t.srcRectBot);
                    drawColor = (1 - sunVector.distance_rate(currentMask.botNormal)) * 255;
                    
                    targetRenderer.set_blend_mode(SDL::renderer::blend_modes::mod);
                    targetRenderer.set_draw_color(drawColor, drawColor, drawColor, 255);
                    targetRenderer.draw_fill_rect(box);
                    
                    grasTexture.set_blend_mode(SDL::texture::blend_modes::mod);
                    targetRenderer.draw(grasTexture);
                                        
                    targetRenderer.set_target(m_renderOutput);
                    drawTexture.set_blend_mode(SDL::texture::blend_modes::blend);
                    targetRenderer.draw(drawTexture, nullptr, &t.destRect);     
                    
                    
                    if ((rand() % 100) == 0) {
                        SDL::rect destTree;
                        
                        SDL::texture treeTexture = resManager.get(1);
                        
                        destTree.w = m_renderContext.size * 4;
                        destTree.h = destTree.w * ((double)treeTexture.height() / treeTexture.width());
                        destTree.x = t.destRect.x + t.destRect.w / 2;
                        destTree.y = t.destRect.y + t.destRect.h - destTree.h;
                        
                        targetRenderer.draw(treeTexture, nullptr, &destTree);
                    }
                }
            }
            
            targetRenderer.reset_target();
            targetRenderer.draw(m_renderOutput);
        }

        struct tile_render_context {
            int size = 10;
            int scale = 8;                                           
            
            int tileX = 0;
            int tileY = size * 2 + 2;
            
            int deltaLeft;
            int deltaRight;
            int deltaBot;
            int deltaHeight;
                        
            int lineHeight = 0;
            int offsetY;
            
            int renderWidth;
            int renderHeight;
            int tileCounter = 0;
        };
        
        tile_render_context m_renderContext;
        region::context& m_dataContext;
        
        
        std::vector<tile> m_tiles;
        std::unordered_map<uint32_t, mask> m_masks;        
        std::vector<SDL::texture> m_renderTextures;
        SDL::texture m_renderOutput;
        

    };
}


#endif

