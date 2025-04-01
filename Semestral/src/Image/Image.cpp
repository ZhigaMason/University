#include "Image.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

Image::Image(const std::string & filename) {
        int n_channels;
        if(!(m_data = stbi_load(filename.c_str(), &m_width, &m_height, &n_channels, WORKING_CHANNELS))) {
                throw std::invalid_argument("Could not read file " + filename + " or allocation error occured");
        }
}

Image::Image(std::unique_ptr<uint8_t[]> data, int32_t h, int32_t w)
        : m_data(data.get()), m_height(h), m_width(w) {
        data = nullptr;
}

Image::~Image() noexcept {
        stbi_image_free(m_data);
}

Image::Image(Image && oth) noexcept {
        swap(*this, oth);
}

Image& Image::operator=(Image && oth) noexcept {
        swap(*this, oth);
        return *this;
}

Image::Pixel & Image::at(uint32_t x, uint32_t y) {
        return (reinterpret_cast<Pixel*>(m_data))[y * m_width + x];
}
Image::Pixel   Image::at(uint32_t x, uint32_t y) const {
        return (reinterpret_cast<Pixel*>(m_data))[y * m_width + x];
}

uint32_t Image::width() const { return m_width; }

uint32_t Image::height() const { return m_height; }

void swap(Image & lhs, Image & rhs) noexcept {
        std::swap(lhs.m_data, rhs.m_data);
        std::swap(lhs.m_height, rhs.m_height);
        std::swap(lhs.m_width, rhs.m_width);
}

void Image::save(const std::string & filename) {
        if(!stbi_write_png(filename.c_str(), m_width, m_height, WORKING_CHANNELS, m_data, PNG_STRIDE))
                throw std::invalid_argument("Encountered an error while saving file " + filename);
}
