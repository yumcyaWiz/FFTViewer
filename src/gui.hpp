#ifndef GUI_H
#define GUI_H

#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include "imgui.h"

class GUI {
  public:
  GLuint image_texture_id; //画像のテクスチャID
  GLuint ft_texture_id; //フーリエ変換後のテクスチャID
  GLuint ift_texture_id; //逆フーリエ変換のテクスチャID

  GUI() {
    glGenTextures(1, &image_texture_id);
    glGenTextures(1, &ft_texture_id);
    glGenTextures(1, &ift_texture_id);
  };

  void draw() const {
    ImGui::Begin("Image");
    {
      ImTextureID texid = (ImTextureID)(intptr_t)image_texture_id;
      ImGui::Image(texid, ImVec2(256, 256));
    }
  };
};

#endif