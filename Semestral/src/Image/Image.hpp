#ifndef _HPP_IMAGE
#define _HPP_IMAGE

#include <string>
#include <cstdint>
#include <memory>

class Image {
public:

        struct Pixel{
                uint8_t r, g, b, a;
        };

        static constexpr int WORKING_CHANNELS = 4;

        explicit Image(const std::string & filename);
        Image(std::unique_ptr<uint8_t[]> data, int32_t h, int32_t w);

        ~Image() noexcept;

        Image(const Image&) = delete;
        Image& operator=(const Image&) = delete;
        Image(Image &&) noexcept;
        Image& operator=(Image&&) noexcept;

        Pixel & at(uint32_t, uint32_t);
        Pixel at(uint32_t, uint32_t) const;

        uint32_t width() const;
        uint32_t height() const;

        friend void swap(Image &, Image &) noexcept;

        void save(const std::string & filename);
private:
        uint8_t * m_data;
        int32_t m_height, m_width;

        static constexpr int PNG_STRIDE = 0;
};
#endif//_HPP_IMAGE
