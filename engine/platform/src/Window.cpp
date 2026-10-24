#include <glad/glad.h>
#include <fabrikk/platform/Window.hpp>
#include <cstdio>
#include <string>

namespace fabrikk {
void Window::framebuffer_size_cb(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}

Window::Window(int width, int height, const char* title) {
    static bool glfw_initialized;

    if (!glfw_initialized) {
        if (!glfwInit()) {
            fprintf(stderr, "[Fabrikk][App] Failed to initialize GLFW\n");
            return;
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
        glfw_initialized = true;
    }

    window_ = glfwCreateWindow(width, height, title, NULL, NULL);
    if (window_ == nullptr) {
        std::fprintf(stderr, "[Fabrikk][Window] glfwCreateWindow failed\n");
        return;
    }
    glfwMakeContextCurrent(window_);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::fprintf(stderr, "[Fabrikk][Window] gladLoadGLLoader failed\n");
        glfwDestroyWindow(window_);
        window_ = nullptr;
        return;
    }

    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window_, framebuffer_size_cb);
}

Window::~Window() {
    if (window_ != nullptr) {
        glfwMakeContextCurrent(nullptr);
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }
}

bool Window::should_close() const { return glfwWindowShouldClose(window_) == GLFW_TRUE; }
void Window::close() { glfwSetWindowShouldClose(window_, GLFW_TRUE); }

bool Window::was_resized() { return resized_; }
void Window::poll_events() { glfwPollEvents(); }
void Window::set_title(const std::string& title) { glfwSetWindowTitle(window_, title.c_str()); }

const char** Window::required_instance_extensions(u32* count) { return glfwGetRequiredInstanceExtensions(count); }
}