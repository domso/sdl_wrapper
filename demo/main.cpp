
#include "src/window.h"
#include "src/renderer.h"

#include <chrono>
#include <thread>
#include <cassert>

int main() {
    SDL::window newWindow;

    SDL::window::config c;
    c.width = 800;
    c.height = 400;
    
    newWindow.open(c);

    SDL::renderer renderer;
    assert(renderer.init(newWindow));
    
    renderer.clear();
    renderer.set_draw_color(255, 255, 255, 255);
    
    renderer.draw_line(100, 100, 100, 200);
    renderer.draw_line(150, 100, 150, 200);
    renderer.draw_line(100, 150, 150, 150);
    
    renderer.draw_line(170, 150, 170, 200);
    renderer.draw_line(170, 100, 170, 130);
    
    renderer.update();
    
    while (newWindow.is_open()) {
        newWindow.update();
        std::chrono::milliseconds delay(200);
        std::this_thread::sleep_for(delay);
    }
    return 0;
}
