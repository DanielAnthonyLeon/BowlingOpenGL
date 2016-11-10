#include "Image.hpp"

#include <iostream>

Image::Image() : m_width(0), m_height(0)
{

}

Image::~Image() {

}

unsigned int Image::width() {
  return m_width;
}

unsigned int Image::height() {
  return m_height;
}

std::vector<unsigned char>& Image::data() {
  return m_data;
}

unsigned Image::loadPNG(const char *filename) {
  unsigned error = lodepng::decode(m_data, m_width, m_height, filename);
  return error;
}
