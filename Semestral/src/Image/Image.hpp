#ifndef _HPP_IMAGE
#define _HPP_IMAGE

#include <string>
#include <cstdint>
#include <memory>

#include "stb_image.h"

struct Image {

        struct Pixel{
                uint8_t r=0, g=0, b=0, a=0;
                Pixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
                Pixel(uint32_t val = 0);

                friend bool operator==(Pixel l, Pixel r) = default;
        };

        static constexpr int WORKING_CHANNELS = 4;

        explicit Image(const std::string & filename);
        Image(uint8_t * data, int32_t h, int32_t w);

        Image();

        ~Image() noexcept;

        Image(const Image&) = delete;
        Image& operator=(const Image&) = delete;
        Image(Image &&) noexcept;
        Image& operator=(Image&&) noexcept;

        Pixel & at(uint32_t, uint32_t);
        Pixel at(uint32_t, uint32_t) const;

        friend void swap(Image &, Image &) noexcept;

        void save(const std::string & filename);

        uint8_t * data;
        int32_t height, width;

        static constexpr int PNG_STRIDE = 0;
};

namespace std {
    template <>
    struct hash<Image::Pixel> {
        uint64_t operator()(Image::Pixel pxl) const {
            return (pxl.r << 16) ^ (pxl.g << 8) ^ (pxl.b);
        }
    };
}

#endif//_HPP_IMAGE
