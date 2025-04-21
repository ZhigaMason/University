#ifndef _HPP_CANDIDATE
#define _HPP_CANDIDATE

#include "Image/Image.hpp"
#include "debug_utils.h"

#include <random>
#include <vector>
#include <unordered_map>

namespace SA_utils{
        struct Rect {
                uint16_t x_min=0, y_min=0, x_max=0, y_max=0;
                Image::Pixel pxl{};

                friend bool operator==(Rect l, Rect r) = default;
        };
}

using namespace SA_utils;

namespace std {
        template<>
        struct hash<Rect> {
                uint64_t operator()(Rect r) const {
                        static const std::hash<Image::Pixel> hashPxl{};
                        return (r.x_min << 23) ^ (r.y_min << 11) ^ (r.x_max >> 7) ^ (r.y_max >> 13) ^ hashPxl(r.pxl);
                }
        };
};

struct Candidate {

        enum EMutation {
                COLOR = 0,
                XMIN = 1,
                YMIN = 2,
                XMAX = 3,
                YMAX = 4
        };

        Candidate(const Image & img, uint16_t n_rect, uint64_t seed);

        ~Candidate() noexcept = default;
        Candidate(const Candidate & oth) = default;
        Candidate(Candidate && oth) = default;

        Candidate & operator=(const Candidate & oth) = delete;
        Candidate & operator=(Candidate && oth) = delete;

        void copy_from(const Candidate & oth);

        Rect randomRect() const;

        // Mutates one Rect, calculates new MSE and returns new Rect
        std::pair<Rect, EMutation> mutate(const SA_utils::Rect & src);

        // returns false if src does not exist in this candidate
        bool changeRect(const SA_utils::Rect & src, const SA_utils::Rect & dst, EMutation mut);

        Image image() const;

        double MSE;
private:

        template<typename T>
        struct Array2D {

                Array2D(uint32_t w, uint32_t h);
                ~Array2D() noexcept;

                Array2D(const Array2D &);
                Array2D(Array2D &&) noexcept = default;
                Array2D & operator=(Array2D);

                T & at(uint32_t x, uint32_t y);
                const T & at(uint32_t x, uint32_t y) const;

                T * data;
                uint32_t w, h;
        };

        using color_data_t=uint32_t;

        const Image & img;
        uint32_t w, h;
        Array2D<double>   local_mses;
        Array2D<std::unordered_map<Image::Pixel, uint16_t>>    pixels;
        std::vector<SA_utils::Rect> rects;
        std::unordered_map<Rect, uint16_t> rect_idx;

        void clean_rect(const Rect &);
        void draw_rect(const Rect &);
        static Image::Pixel mix_colors(const std::unordered_map<Image::Pixel, uint16_t> &);
        static double compute_sq_error(Image::Pixel lhs, Image::Pixel rhs);

        //const uint16_t seed;
        mutable std::mt19937 generator;
};

template <typename T>
Candidate::Array2D<T>::Array2D(uint32_t w_, uint32_t h_)
        :  data(new T[w_ * h_]), w(w_), h(h_) {}

template <typename T>
Candidate::Array2D<T>:: Array2D(const Array2D & oth) : Array2D(oth.w, oth.h){
        DEBUG_OUTPUT("COPYING ARRAY, w=%u, h=%u\n", w, h);
        for(uint32_t i = 0; i < w*h; ++i) {
                data[i] = oth.data[i];
        }
}

template <typename T>
Candidate::Array2D<T>::~Array2D<T>() noexcept {
        delete [] data;
}

template <typename T>
Candidate::Array2D<T> & Candidate::Array2D<T>::operator=(Array2D<T> oth) {
        std::swap(*this, oth);
        return *this;
}

template <typename T>
T & Candidate::Array2D<T>::at(uint32_t x, uint32_t y) {
        return data[y * w + x];
}

template <typename T>
const T & Candidate::Array2D<T>::at(uint32_t x, uint32_t y) const {
        return data[y * w + x];
}

#endif//_HPP_CANDIDATE
