#ifndef _HPP_CANDIDATE
#define _HPP_CANDIDATE

#include <cstdint>
#include <vector>

struct Candidate {

        Candidate(uint32_t w, uint32_t h, uint32_t seed);




private:

        using color_data_t=uint32_t;

        template<typename T>
        struct Array2D {

                Array2D(uint32_t w, uint32_t h);
                ~Array2D() noexcept;

                Array2D(const Array2D &);
                Array2D(Array2D &&) noexcept = default;
                Array2D & operator=(Array2D);

                T & at(uint32_t x, uint32_t y);
                T at(uint32_t x, uint32_t y) const;

                T * data;
                uint32_t w, h;
        };

        struct Pixel {
                color_data_t r,g,b,a;
        };

        struct Rect {
                uint16_t x_min, y_min, x_max, y_max;
        };

        uint32_t w, h;
        Array2D<uint16_t> n_rect_intersect;
        Array2D<double>   local_mses;
        Array2D<Pixel>    pixels;
        std::vector<Rect> rects;
};

#endif//_HPP_CANDIDATE

