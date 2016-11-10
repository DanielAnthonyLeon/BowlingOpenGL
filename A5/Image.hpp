#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "lodepng/lodepng.h"

class Image {
  unsigned m_width;
  unsigned m_height;
  std::vector<unsigned char> m_data;

public:
  Image();
  ~Image();

  unsigned width();
  unsigned height();
  std::vector<unsigned char>& data();

  unsigned loadPNG(const char *filename);
};

#endif
