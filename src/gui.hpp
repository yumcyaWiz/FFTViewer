#ifndef GUI_H
#define GUI_H

#include <iostream>
#include <string>
#include <vector>

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

    ImGui::Begin("Frequency Domain");
    {
      ImTextureID texid = (ImTextureID)(intptr_t)ft_texture_id;
      ImGui::Image(texid, ImVec2(image_width, image_height));
    }
    ImGui::End();
  };

  private:
  std::vector<float> image; //[0, 1]のグレースケール画像
  GLuint image_texture_id; //画像のテクスチャID
  int image_width;
  int image_height;

  GLuint ft_texture_id; //フーリエ変換後のテクスチャID
  GLuint ift_texture_id; //逆フーリエ変換のテクスチャID

  void setImage(const std::string& filename) {
    //load image
    int channels;
    unsigned char *img = stbi_load(filename.c_str(), &image_width, &image_height, &channels, 3);
    if (!img) {
      std::cerr << "failed to load image" << std::endl;
      return;
    }
    std::cout << "image loaded " << image_width << "x" << image_height << ", " << channels << std::endl;

    //grayscale
    image.resize(3 * image_width * image_height, 0);
    for (int j = 0; j < image_height; ++j) {
      for (int i = 0; i < image_width; ++i) {
        const float r = img[0 + 3*i + 3*image_width*j] / 255.0f;
        const float g = img[1 + 3*i + 3*image_width*j] / 255.0f;
        const float b = img[2 + 3*i + 3*image_width*j] / 255.0f;
        const float avg = (r + g + b)/3.0f;
        image[0 + 3*i + 3*image_width*j] = avg;
        image[1 + 3*i + 3*image_width*j] = avg;
        image[2 + 3*i + 3*image_width*j] = avg;
      }
    }

    glBindTexture(GL_TEXTURE_2D, image_texture_id); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_FLOAT, image.data());
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(img);
  };

  void setFFT() {
    const int N = image_width * image_height;
    fftw_complex *input, *output;
    input = (fftw_complex*)(fftw_malloc(sizeof(fftw_complex) * N));
    output = (fftw_complex*)(fftw_malloc(sizeof(fftw_complex) * N));

    fftw_plan plan;
    plan = fftw_plan_dft_2d(image_width, image_height, input, output, FFTW_FORWARD, FFTW_ESTIMATE);

    for (int j = 0; j < image_height; ++j) {
      for (int i = 0; i < image_width; ++i) {
        const float r = image[0 + 3*i + 3*image_width*j];
        const float g = image[1 + 3*i + 3*image_width*j];
        const float b = image[2 + 3*i + 3*image_width*j];
        input[i + image_width*j][0] = (r + g + b)/3.0f;
      }
    }
  };
};

#endif