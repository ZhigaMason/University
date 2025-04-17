#include "Image.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

Image::Pixel::Pixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    : r(r), g(g), b(b), a(a) {}

Image::Pixel::Pixel(uint32_t val)
    : r(val & 0xff'00'00'00), g(val & 0x00'ff'00'00), b(val & 0x00'00'ff'00),
      a(val & 0x00'00'00'ff) {}

Image::Image(const std::string & filename) {
        int n_channels;
        if(!(data = stbi_load(filename.c_str(), &width, &height, &n_channels, WORKING_CHANNELS))) {
                throw std::invalid_argument("Could not read file " + filename + " or allocation error occured");
        }
}

Image::Image(std::unique_ptr<uint8_t[]> data, int32_t h, int32_t w)
        : data(data.get()), height(h), width(w) {
        data = nullptr;
}

Image::~Image() noexcept {
        stbi_image_free(data);
}

Image::Image(Image && oth) noexcept {
        swap(*this, oth);
}

Image& Image::operator=(Image && oth) noexcept {
        swap(*this, oth);
        return *this;
}

Image::Pixel & Image::at(uint32_t x, uint32_t y) {
        return (reinterpret_cast<Pixel*>(data))[y * width + x];
}
Image::Pixel   Image::at(uint32_t x, uint32_t y) const {
        return (reinterpret_cast<Pixel*>(data))[y * width + x];
}

void swap(Image & lhs, Image & rhs) noexcept {
        std::swap(lhs.data, rhs.data);
        std::swap(lhs.height, rhs.height);
        std::swap(lhs.width, rhs.width);
}

void Image::save(const std::string & filename) {
        if(!stbi_write_png(filename.c_str(), width, height, WORKING_CHANNELS, data, PNG_STRIDE))
                throw std::invalid_argument("Encountered an error while saving file " + filename);
}
