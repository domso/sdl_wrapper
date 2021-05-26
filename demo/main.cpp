
#include "src/window.h"
#include "src/renderer.h"
#include "src/image.h"
#include "src/gui/gui_container.h"  

#include <chrono>
#include <thread>
#include <cassert>
#include <iostream>
#include <memory>

int main() {
    SDL::window newWindow;
    SDL::window::config c;
    c.width = 1000;
    c.height = 1000;
    newWindow.open(c);

    SDL::renderer renderer;
    SDL::image::loader img_loader;
    font::loader font_loader;
    assert(renderer.init(newWindow));
    SDL::image::loader l;
    
    SDL::image map;
    assert(map.load("export_6.png"));
    
    std::cout << map.internal_handle()->w << std::endl;
    std::cout << map.internal_handle()->h << std::endl;
    
    auto tex = renderer.create_texture(20000, 20000);
    renderer.set_draw_color(250, 250, 250, 255);
    renderer.set_target(tex);
    
    c.width = 20000;
    c.height = 20000;
        std::vector<int32_t> data(c.width * c.height, 0);
        int size = 10;
        for (int gy = 0; gy < 511; gy++) {
            for (int gx = 0; gx < 511; gx++) {        
                for (int y = 0; y <= size; y++) {
                    for (int x = 0; x <= size; x++) {
                        int z00 = (static_cast<uint8_t*>(map.internal_handle()->pixels)[gx + gy * 512] & 255)*4;
                        int z01 = (static_cast<uint8_t*>(map.internal_handle()->pixels)[gx + 1 + gy * 512] & 255)*4;
                        int z10 = (static_cast<uint8_t*>(map.internal_handle()->pixels)[gx + (gy + 1) * 512] & 255)*4;
                        int z11 = (static_cast<uint8_t*>(map.internal_handle()->pixels)[gx + 1 + (gy + 1) * 512] & 255)*4;
                        
                        auto zs = z00 + static_cast<float>(z01 - z00) / size * y;
                        auto ze = z10 + static_cast<float>(z11 - z10) / size * y;
                        auto zc = zs + static_cast<float>(ze - zs) / size * x;
              
                        int col = 50 + 10 * std::max(std::abs(z00 - z01), std::max(std::abs(z00 - z10), std::abs(z00 - z11)));
              
                        for (int z = std::min(std::min(std::min(z00, z01), z10), z11); z <= zc; z++) {                       
                            int sx = 4000+(gy * size - gx * size + 400 + x - y);
                            int sy = 1000+(gy * (size / 2) + gx * (size / 2) + x / 2 + y / 2) - z;
                            col = z;
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
//              
    renderer.update();
    renderer.reset_target();
    
    
    c.width = 1000;
    c.height = 1000;
    
    SDL::gui::gui_container b(renderer);
    
    b.set_render_size({800, 800});
    b.set_viewport({0, 0, 800, 800});
    b.set_region({0, 0, c.width, c.height});

    b.set_background_color({60, 60, 60});
    b.enable();
    
    {
        auto& w = b["sdfsdf"];
        w.set_render_size({20000, 20000});
        w.set_viewport({0, 0, 500, 500});
        w.set_region({0, 0, 800, 800});
        w.set_background_color({125, 0, 0});
        w.set_background_tex(&tex);
        w.enable();
    }
    
    {
        auto& w = b["test"];
        w.set_render_size({100, 100});
        w.set_viewport({0, 0, 50, 50});
        w.set_region({100, 100, 50, 50});
        w.set_background_color({125, 0, 0});
        w.enable();
    }

    {
        auto& w = b["test.blub"];
        w.set_render_size({25, 25});
        w.set_viewport({0, 0, 25, 25});
        w.set_region({10, 10, 25, 25});
        w.set_background_color({0, 125, 0});
        w.enable();
    }

    {
        auto& w = b["asd"];
        w.set_render_size({100, 100});
        w.set_viewport({0, 0, 50, 50});
        w.set_region({300, 300, 100, 100});
        w.set_background_color({255, 125, 0});
        w.set_background_img("../test.png");
        w.enable();
    }

    {
        auto& w = b["asd2"];
        w.set_render_size({100, 100});
        w.set_viewport({0, 0, 50, 50});
        w.set_region({350, 300, 100, 100});
        w.set_background_color({255, 125, 0});
        w.set_text("../test.png", {255, 255, 255});
        w.enable();
    }

    {
        auto& w = b["dogh"];
        w.set_render_size({100, 100});
        w.set_viewport({25, 25, 50, 50});
        w.set_region({200, 200, 100, 100});
        w.set_background_color({0, 0, 125});
        w.enable();
        w.set_on_click([](SDL::gui::gui_container&, SDL::gui::gui_container& root) {
            if (root["test.blub"].is_enabled()) {
                root["test.blub"].disable();
            } else {
                root["test.blub"].enable();
            }
        });
    }

    while (newWindow.is_open()) {
        newWindow.update();

        int mx, my, mb;
        mb = SDL_GetMouseState(&mx, &my);
        b.mouse(mx, my, mb);


        b.update();
        b.draw();

        renderer.update();
    }
    return 0;
}
