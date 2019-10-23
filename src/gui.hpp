#ifndef GUI_H
#define GUI_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

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

    setImage("lena.png");
    computeFFT();
  };

  void draw() {
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

    ImGui::Begin("Space Domain");
    {
      ImTextureID texid = (ImTextureID)(intptr_t)ift_texture_id;
      ImGui::Image(texid, ImVec2(image_width, image_height));
    }
    ImGui::End();

    bool refresh = false;
    ImGui::Begin("IFT Parameters");
    {
      refresh |= ImGui::SliderInt("Samples X", &samples_x, 0, image_width);
      refresh |= ImGui::SliderInt("Samples Y", &samples_y, 0, image_height);
    }
    ImGui::End();

    if (refresh) {
      computeIFT();
    }
  };

  private:
  int image_width; //画像の横幅
  int image_height; //画像の縦幅
  std::vector<float> image; //[0, 1]のグレースケール画像
  GLuint image_texture_id; //画像のテクスチャID

  fftw_complex* ft; //フーリエ変換後の画像
  GLuint ft_texture_id; //フーリエ変換後のテクスチャID

  int samples_x; //逆フーリエ変換に使用するx方向の周波数
  int samples_y; //逆フーリエ変換に使用するy方向の周波数
  GLuint ift_texture_id; //逆フーリエ変換のテクスチャID

  void setImage(const std::string& filename) {
    //load image
    int channels;
    unsigned char *img = stbi_load(filename.c_str(), &image_width, &image_height, &channels, 3);
    if (!img) {
      std::cerr << "failed to load image" << std::endl;
      std::exit(EXIT_FAILURE);
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

  void computeFFT() {
    //allocate input, ourput array
    const int N = image_width * image_height;
    fftw_complex *input;
    input = (fftw_complex*)(fftw_malloc(sizeof(fftw_complex) * N));
    ft = (fftw_complex*)(fftw_malloc(sizeof(fftw_complex) * N));

    //create plan
    fftw_plan plan;
    plan = fftw_plan_dft_2d(image_width, image_height, input, ft, FFTW_FORWARD, FFTW_ESTIMATE);

    //input data
    for (int j = 0; j < image_height; ++j) {
      for (int i = 0; i < image_width; ++i) {
        const float r = image[0 + 3*i + 3*image_width*j];
        input[i + image_width*j][0] = r;
        input[i + image_width*j][1] = 0;
      }
    }

    //fft
    fftw_execute(plan);
    fftw_destroy_plan(plan);
    fftw_free(input);

    //create frequency domain texture
    std::vector<float> tex_freq(3 * image_width * image_height, 0);
    createFFTTexture(ft, tex_freq);

    //normalize texture
    normalizeFFT(tex_freq);

    //send frequency domain texture
    glBindTexture(GL_TEXTURE_2D, ft_texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_FLOAT, tex_freq.data());
    glBindTexture(GL_TEXTURE_2D, 0);
  };


  void computeIFT() {
    const int N = image_width * image_height;

    //ift
    fftw_complex* input_ift = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
    fftw_complex* output_ift = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
    fftw_plan ift_plan = fftw_plan_dft_2d(image_width, image_height, input_ift, output_ift, FFTW_BACKWARD, FFTW_ESTIMATE);

    for (int j = 0; j < samples_y; ++j) {
      for (int i = 0; i < samples_x; ++i) {
        input_ift[i + image_width*j][0] = ft[i + image_width*j][0];
        input_ift[i + image_width*j][1] = ft[i + image_width*j][1];
      }
    }
    for (int j = samples_y; j < image_height; ++j) {
      for (int i = samples_x; i < image_width; ++i) {
        input_ift[i + image_width*j][0] = 0;
        input_ift[i + image_width*j][1] = 0;
      }
    }

    fftw_execute(ift_plan);
    fftw_destroy_plan(ift_plan);
    fftw_free(input_ift);

    //create space domain texture
    std::vector<float> tex_space(3 * image_width * image_height, 0);
    createIFFTTexture(output_ift, tex_space);
    fftw_free(output_ift);

    //send space domain texture
    glBindTexture(GL_TEXTURE_2D, ift_texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_FLOAT, tex_space.data());
    glBindTexture(GL_TEXTURE_2D, 0);
  };


  void createFFTTexture(const fftw_complex* fft, std::vector<float>& tex) {
    for (int j = 0; j < image_height/2; ++j) {
      for (int i = 0; i < image_width/2; ++i) {
        const float real = fft[i + image_width*j][0];
        const float imag = fft[i + image_width*j][1];
        const float amp = std::sqrt(real*real + imag*imag);

        //right-down
        const int i_rd = i + image_width/2;
        const int j_rd = j + image_height/2;
        tex[0 + 3*i_rd + 3*image_width*j_rd] = amp;
        tex[1 + 3*i_rd + 3*image_width*j_rd] = amp;
        tex[2 + 3*i_rd + 3*image_width*j_rd] = amp;

        //left-down
        const int i_ld = image_width/2 - i - 1;
        tex[0 + 3*i_ld + 3*image_width*j_rd] = amp;
        tex[1 + 3*i_ld + 3*image_width*j_rd] = amp;
        tex[2 + 3*i_ld + 3*image_width*j_rd] = amp;

        //right-up
        const int j_ru = image_height/2 - j - 1;
        tex[0 + 3*i_rd + 3*image_width*j_ru] = amp;
        tex[1 + 3*i_rd + 3*image_width*j_ru] = amp;
        tex[2 + 3*i_rd + 3*image_width*j_ru] = amp;

        //left-up
        tex[0 + 3*i_ld + 3*image_width*j_ru] = amp;
        tex[1 + 3*i_ld + 3*image_width*j_ru] = amp;
        tex[2 + 3*i_ld + 3*image_width*j_ru] = amp;
      }
    }
  };


  void createIFFTTexture(const fftw_complex* ifft, std::vector<float>& tex) {
    for (int j = 0; j < image_height; ++j) {
      for (int i = 0; i < image_width; ++i) {
        const float real = ifft[i + image_width*j][0];
        const float scale = image_width * image_height;
        tex[0 + 3*i + 3*image_width*j] = real / scale;
        tex[1 + 3*i + 3*image_width*j] = real / scale;
        tex[2 + 3*i + 3*image_width*j] = real / scale;
      }
    }
  };


  void normalizeFFT(std::vector<float>& data) {
    const float vmax = *std::max_element(data.begin(), data.end());
    const float c = 1 / std::log(1 + vmax);
    for (int i = 0; i < data.size(); ++i) {
      data[i] = c * std::log(1 + data[i]);
    }
  };
};

#endif