#ifndef _HPP_COLOR_EXTRACTION_RANDOM_918230981209380912830912
#define _HPP_COLOR_EXTRACTION_RANDOM_918230981209380912830912

#include <vector>
#include <Image/Image.hpp>

std::vector<Image::Pixel> random_sampled_pixels(const Image::Pixel * arr, uint16_t w, uint16_t h, uint16_t samples, uint64_t seed);

#endif//_HPP_COLOR_EXTRACTION_RANDOM_918230981209380912830912
