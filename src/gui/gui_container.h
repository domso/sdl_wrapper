#pragma once

#include <unordered_map>
#include <functional>

#include "gui_element.h"
#include "frame.h"
#include "../renderer.h"

namespace SDL::gui {
    class gui_container : public gui_element {
    public:
        gui_container(SDL::renderer& renderer);
        gui_container(SDL::renderer& renderer, gui_container& root);
        
        gui_container& operator[](const std::string& name);
        gui_container& child(const std::string& name);
        
        void set_background_img(const std::string& file); 
        void set_background_tex(SDL::texture* tex);     
        void set_text(const std::string& text, color c);
        void update_render_size(const size s) override;
        void update_viewport(const rect r) override;
        void update_region(const rect r) override;
        void update() override;
        void draw() override;
        void update_background_color(const color c) override;    
        void set_on_click(const std::function<void(gui_container&, gui_container&)> callback);
        void mouse(const int x, const int y, const int button) override;
    private:
        std::unordered_map<std::string, std::unique_ptr<gui_container>> m_slaves;

        float scale_x;

        bool is_dragged_x = false;
        int dragged_x;

        bool is_dragged_y = false;
        int dragged_y;

        frame m_main;
        frame m_scroll_x;
        frame m_scroll_y;

        frame m_content;
        
        gui_container& m_root;
        
        bool is_clicked = false;
        
        std::function<void(gui_container&, gui_container&)> m_on_click;
    };
}
