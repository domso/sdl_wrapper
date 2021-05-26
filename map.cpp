
#include "src/window.h"
#include "src/renderer.h"

#include <chrono>
#include <thread>
#include <cassert>
#include <vector>
#include <iostream>




int main() {
    SDL::window newWindow;

    SDL::window::config c;
    c.width = 1600;
    c.height = 900;
    
    newWindow.open(c);

    SDL::renderer renderer;
    assert(renderer.init(newWindow));
    
    SDL::image::loader l;
    
    SDL::image map;
    assert(map.load("export_6.png"));
    
    std::cout << map.internal_handle()->w << std::endl;
    std::cout << map.internal_handle()->h << std::endl;
    
    std::vector<bool> mask(100 * 100, false);
    while (newWindow.is_open()) {
//         renderer.clear();
        renderer.set_draw_color(255, 255, 255, 255);
        
        std::vector<int32_t> data(c.width * c.height, 0);
        
            
        int size = 2;
        for (int gy = 0; gy < 511; gy++) {
            for (int gx = 0; gx < 511; gx++) {        
                for (int y = 0; y <= size; y++) {
                    for (int x = 0; x <= size; x++) {
                        int z00 = (static_cast<uint8_t*>(map.internal_handle()->pixels)[gx + gy * 512] & 255);
                        int z01 = (static_cast<uint8_t*>(map.internal_handle()->pixels)[gx + 1 + gy * 512] & 255);
                        int z10 = (static_cast<uint8_t*>(map.internal_handle()->pixels)[gx + (gy + 1) * 512] & 255);
                        int z11 = (static_cast<uint8_t*>(map.internal_handle()->pixels)[gx + 1 + (gy + 1) * 512] & 255);
                        
                        auto zs = z00 + static_cast<float>(z01 - z00) / size * y;
                        auto ze = z10 + static_cast<float>(z11 - z10) / size * y;
                        auto zc = zs + static_cast<float>(ze - zs) / size * x;
              
                        int col = 50 + 10 * std::max(std::abs(z00 - z01), std::max(std::abs(z00 - z10), std::abs(z00 - z11)));
              
                        for (int z = std::min(std::min(std::min(z00, z01), z10), z11); z <= zc; z++) {                       
                            int sx = (gy * size - gx * size + 400 + x - y);
                            int sy = (gy * (size / 2) + gx * (size / 2) + x / 2 + y / 2) - z;
                            if (0 <= sx && sx < c.width && 0 <= sy && sy < c.height) {
                                data[sy * c.width + sx] = col;
                            }
                            if (0 <= sx + 1 && sx + 1< c.width && 0 <= sy && sy < c.height) {
                                data[sy * c.width + sx + 1] = col;
                            }
                            if (0 <= sx && sx < c.width && 0 <= sy + 1 && sy + 1 < c.height) {
                                data[(sy + 1) * c.width + sx] = col;
                            }
                            if (0 <= sx + 1 && sx + 1 < c.width && 0 <= sy + 1 && sy + 1 < c.height) {
                                data[(sy + 1) * c.width + sx + 1] = col;
                            }                            
                        }
                        
                        
                    }
                }
            }
        }
        
        for (int y = 0; y < c.height; y++) {
            for (int x = 0; x < c.width; x++) {
                if (data[x + c.width * y] > 1) {
                    renderer.set_draw_color(data[x + c.width * y], data[x + c.width * y], data[x + c.width * y], 255);
                    renderer.draw_point(x, y);
                }
            }
        }
                
        
        
        
//         for (int z = 0; z < 100; z++) {
//             for (int y = 0; y < 100; y++) {
//                 for (int x = 0; x < 100; x++) {
//                     if (mask.at(x + (99 - z) * 100)) {
//                         renderer.set_draw_color(100 + std::max(std::max(x, y), z), 100 + std::max(std::max(x, y), z), 100, 255);
//                         renderer.draw_rect({100 + x - y, 100 + x / 2 + y / 2 - z, 2, 2});
//                     }
//                 }
//             }
//         }
//         
//         renderer.draw_rect({400, 100, 400, 400});
//         renderer.draw_rect({400, 50, 100, 50});
//         
//         for (int y = 0; y < 100; y++) {
//             for (int x = 0; x < 100; x++) {                
//                 if (mask.at(x + y * 100)) {
//                     renderer.set_draw_color(200, 200, 200, 200);
//                 } else {
//                     renderer.set_draw_color(100, 100, 100, 100);
//                 }                    
//         
//                 renderer.draw_rect({400 + x * 4, 100 + y * 4, 4, 4});
//             }
//         }
//         SDL_PumpEvents();
//         int mx, my;
//         
//         
//         if (SDL_GetMouseState(&mx, &my) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
//             mx -= 400;
//             my -= 100;
//             
//             mx /= 4;
//             my /= 4;
//             std::cout << mx << " " << my << std::endl;
//             
//             if (0 <= mx && mx < 100 && 0 <= my && my < 100) {
//                 mask[mx + my * 100] = true;
//             }
//         }
        
        
        renderer.update();
        newWindow.update();
        
        
        
        std::chrono::milliseconds delay(200);
        std::this_thread::sleep_for(delay);
    }
    return 0;
}
