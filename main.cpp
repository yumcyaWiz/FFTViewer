#include <iostream>

#include "glad/glad.h"
#include <GLFW/glfw3.h>


int main() {
  //Initialize GLFW
  if (!glfwInit()) {
    std::cerr << "failed to initialize glfw" << std::endl;
    return -1;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(512, 512, "FFT Viewer", nullptr, nullptr);
  if (window == nullptr) {
    std::cerr << "failed to create glfw window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  //Initialize GLAD
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "failed to initialize glad" << std::endl;
    glfwTerminate();
    return -1;
  }

  return 0;
}
