#pragma once
#include <GLFW/glfw3.h>
#include <fabrikk/core/types.hpp>
#include <string>

namespace fabrikk {
class Window {
private:
    GLFWwindow* window_ = nullptr;
    u32 width_ = 0;
    u32 height_ = 0;
    bool resized_ = false;
    static void framebuffer_size_cb(GLFWwindow*, int width, int height);
public:
    explicit Window(int width, int height, const char* title);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    [[nodiscard]] GLFWwindow* handle() const { return window_; }
    [[nodiscard]] bool should_close() const;
    void close();

    [[nodiscard]] bool was_resized();
    void clear_resized_flag() { resized_ = false; }

    [[nodiscard]] u32 width() const { return width_; }
    [[nodiscard]] u32 height() const { return height_; }

    void poll_events();
    void set_title(const std::string& title);

    bool valid() const noexcept { return window_ != nullptr; }
    [[nodiscard]] static const char** required_instance_extensions(u32* count);
};
}