#pragma once
#include <string>

#include <fabrikk/platform/Window.hpp>
#include <fabrikk/ecs/Simulation.hpp>

namespace fabrikk {
class App {
private:
    std::string name_;
    Window* window_;
    
    bool should_quit_ = false;
public:
    Simulation simulation;
    App(const std::string& name);
    
    void quit();
    App& run();
};
}