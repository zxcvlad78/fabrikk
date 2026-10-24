#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fabrikk/app/App.hpp>
#include <chrono>
#include <fabrikk/app/ctx/AppContext.hpp>
#include <fabrikk/ecs/ctx/SimulationContext.hpp>


namespace fabrikk {
App::App(const std::string& name)
: name_(std::move(name)) {
    window_ = new Window(1280, 720, name.c_str());
    auto& sim_ctx = simulation.registry().ctx().emplace<ctx::SimulationContext>();

    auto& app_ctx = simulation.registry().ctx().emplace<ctx::AppContext>();
    app_ctx.window = window_;
}

void App::quit() { should_quit_ = true; }

App& App::run() {
    if (!window_) return *this;

    using clock = std::chrono::steady_clock;
    auto previous = clock::now();
    f64 accumulator = 0.0;

    auto& sim_ctx = simulation.registry().ctx().get<ctx::SimulationContext>();

    while (!window_->should_close() && !should_quit_) {
        window_->poll_events();
        simulation.run(SimulationPhase::Input); // run phase

        if (window_->was_resized()) {
            int w = 0;
            int h = 0;
            glfwGetFramebufferSize(window_->handle(), &w, &h);
            window_->clear_resized_flag();
            if (w > 0 && h > 0) {
                //renderer_->on_resize(static_cast<u32>(w), static_cast<u32>(h));
            }
        }

        const auto now = clock::now();
        const f64 frame_dt = std::chrono::duration<f64>(now - previous).count();
        previous = now;
        
        accumulator += frame_dt > 0.25 ? 0.25 : frame_dt;

        while (accumulator >= sim_ctx.fixed_dt) {
            simulation.run(SimulationPhase::FixedUpdate); // run phase
            simulation.tick_next();
            accumulator -= sim_ctx.fixed_dt;
        }

        simulation.run(SimulationPhase::Update); // run phase

        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        simulation.run(SimulationPhase::Render); // run phase
        simulation.run(SimulationPhase::PostRender); // run phase
    
        glfwSwapBuffers(window_->handle());
    }

    return *this;
}
}