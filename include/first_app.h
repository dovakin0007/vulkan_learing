#pragma once

#include "window.h"

namespace the_engine {
    class FirstApp {
        public: 
            static constexpr int WIDTH = 800;
            static constexpr int HEIGHT = 600;

            void run();
        private: 
            EngineWindow engineWindow {WIDTH, HEIGHT, "Hello Vulkan"};
            
    };
}
