#ifndef GUI_H
#define GUI_H

#include <iostream>
#include <string>

#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include "imgui.h"

#include "stb_image.h"

class GUI {
  public:
  GUI() {
    glGenTextures(1, &image_texture_id);
    glGenTextures(1, &ft_texture_id);
    glGenTextures(1, &ift_texture_id);

    setImage("test.jpg");
  };

  void draw() const {
    ImGui::Begin("Image");
    {
      ImTextureID texid = (ImTextureID)(intptr_t)image_texture_id;
      ImGui::Image(texid, ImVec2(image_width, image_height));
    }
    ImGui::End();
  };

  private:
  GLuint image_texture_id; //画像のテクスチャID
  int image_width;
  int image_height;

  GLuint ft_texture_id; //フーリエ変換後のテクスチャID
  GLuint ift_texture_id; //逆フーリエ変換のテクスチャID

  void setImage(const std::string& filename) {
    int channels;
    unsigned char *img = stbi_load(filename.c_str(), &image_width, &image_height, &channels, 3);
    if (!img) {
      std::cerr << "failed to load image" << std::endl;
      return;
    }
    std::cout << "image loaded " << image_width << "x" << image_height << std::endl;

    glBindTexture(GL_TEXTURE_2D, image_texture_id); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(img);
  };
};

#endif