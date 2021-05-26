
#include "src/window.h"
#include "src/renderer.h"
#include "src/image.h"

#include <chrono>
#include <thread>
#include <cassert>
#include <iostream>
#include <memory>


#include "gui/region.h"  

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

    renderer.clear();
    renderer.set_draw_color(255, 255, 255, 255);

    renderer.draw_line(100, 100, 100, 200);
    renderer.draw_line(150, 100, 150, 200);
    renderer.draw_line(100, 150, 150, 150);

    renderer.draw_line(170, 150, 170, 200);
    renderer.draw_line(170, 100, 170, 130);

    window b(renderer) ;
    b.set_render_size({800, 800});
    b.set_viewport({0, 0, c.width, c.height});
    b.set_region({0, 0, c.width, c.height});

    b.set_background_color({60, 60, 60});
    b.enable();

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
        w.set_region({300, 100, 100, 100});
        w.set_background_color({255, 125, 0});
        w.set_background_img("../test.png");
        w.enable();
    }

    {
        auto& w = b["asd2"];
        w.set_render_size({100, 100});
        w.set_viewport({0, 0, 50, 50});
        w.set_region({300, 300, 100, 100});
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
        w.set_on_click([](window&, window& root) {
            if (root["test.blub"].is_enabled()) {
                root["test.blub"].disable();
            } else {
                root["test.blub"].enable();
            }
        });
    }

    while (newWindow.is_open()) {
        newWindow.update();
//         std::chrono::milliseconds delay(200);
//         std::this_thread::sleep_for(delay);


        int mx, my, mb;
        mb = SDL_GetMouseState(&mx, &my);
        b.mouse(mx, my, mb);


        b.update();
        b.draw();

        renderer.update();
    }
    return 0;
}
