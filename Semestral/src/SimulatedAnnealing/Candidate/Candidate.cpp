#include "Candidate.hpp"
#include <climits>
#include <random>

template <typename T>
Candidate::Array2D<T>::Array2D(uint32_t w_, uint32_t h_)
        :  data(new T[w_ * h_]), w(w_), h(h_) {}

template <typename T>
Candidate::Array2D<T>:: Array2D(const Array2D & oth) : Array2D(oth.w, oth.h){
        for(uint32_t i = 0; i < w*h; ++i)
                data[i] = oth.data[i];
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

Candidate::Candidate(const Image &img)
    : img(img), w(img.width), h(img.height), local_mses(img.width, img.height),
      pixels(img.width, img.height) {}

void Candidate::copy_from(const Candidate & oth) {
        h = oth.w;
        h = oth.h;
        MSE = oth.MSE;
        local_mses = oth.local_mses;
        pixels = oth.pixels;
        rects = oth.rects;
        rect_idx = oth.rect_idx;
}

Rect Candidate::randomRect() const {
        static std::mt19937 generator;
        std::uniform_int_distribution<uint16_t> distribution(0, rects.size() - 1);

        return rects[distribution(generator)];
}

// Mutates one Rect, calculates new MSE and returns new Rect
Rect Candidate::mutate(const Rect & src) {
        enum Mutation {
                COLOR = 0,
                LEFT_LOWER = 1,
                RIGHT_UPPER = 2
        };

        static std::mt19937 generator;
        std::discrete_distribution<uint8_t> distribution_mutations({1,1,1});

        Mutation val = Mutation(distribution_mutations(generator));

        Rect dst = src;
        switch(val) {
                case COLOR: {
                        std::uniform_int_distribution<uint32_t> distribution(0, UINT32_MAX);
                        dst.pxl = distribution(generator);
                        break;
                }
                case LEFT_LOWER: {
                        std::uniform_int_distribution<uint16_t> distribution_x(0, dst.x_min - 1);
                        std::uniform_int_distribution<uint16_t> distribution_y(0, dst.y_min - 1);
                        dst.x_min = distribution_x(generator);
                        dst.y_min = distribution_y(generator);
                        break;
                }
                case RIGHT_UPPER: {
                        std::uniform_int_distribution<uint16_t> distribution_x(dst.x_max + 1, w - 1);
                        std::uniform_int_distribution<uint16_t> distribution_y(dst.y_max + 1, h - 1);
                        dst.x_min = distribution_x(generator);
                        dst.y_min = distribution_y(generator);
                        break;
                }
        }

        changeRect(src, dst);
        return dst;
}


// returns false if src does not exist in this candidate
bool Candidate::changeRect(const Rect & src, const Rect & dst) {
        uint16_t idx;
        try {
                idx = rect_idx.at(src);
        }
        catch(const std::out_of_range &) {
                return false;
        }

        clean_rect(src);
        draw_rect(dst);
        rects[idx] = dst;
        return true;
}

void Candidate::clean_rect(const Rect & r) {
        for(uint16_t i = r.x_min; i <= r.x_max; ++i) {
                for(uint16_t j = r.y_min; j <= r.y_max; ++j) {
                        std::unordered_multiset<Image::Pixel> & pxls = pixels.at(i, j);
                        MSE -= local_mses.at(i, j);
                        pxls.erase(pxls.find(r.pxl));

                        Image::Pixel new_mixture = mix_colors(pxls);
                        local_mses.at(i, j) = compute_sq_error(new_mixture, img.at(i, j));
                        MSE += local_mses.at(i, j);
                }
        }
}

void Candidate::draw_rect(const Rect & r) {
        for(uint16_t i = r.x_min; i <= r.x_max; ++i) {
                for(uint16_t j = r.y_min; j <= r.y_max; ++j) {
                        std::unordered_multiset<Image::Pixel> & pxls = pixels.at(i, j);
                        MSE -= local_mses.at(i, j);
                        pxls.insert(r.pxl);

                        Image::Pixel new_mixture = mix_colors(pxls);
                        local_mses.at(i, j) = compute_sq_error(new_mixture, img.at(i, j));
                        MSE += local_mses.at(i, j);
                }
        }

}

Image::Pixel Candidate::mix_colors(const std::unordered_multiset<Image::Pixel> & pxls) {
        Image::Pixel base = 0x00'00'00'01; // R=0, G=0, B=0, A=1

        for(Image::Pixel pxl : pxls) {
                uint32_t alpha = base.a + pxl.a - base.a * pxl.a;
                if(alpha == 0)
                        base = 0; // transperant = black

                uint32_t red   = (pxl.r * pxl.a + base.r * base.a * (1 - pxl.a)) / alpha;
                uint32_t blue  = (pxl.b * pxl.a + base.b * base.a * (1 - pxl.a)) / alpha;
                uint32_t green = (pxl.g * pxl.a + base.g * base.a * (1 - pxl.a)) / alpha;
                base = Image::Pixel(red, blue, green, alpha);
        }
        return base;
}

double Candidate::compute_sq_error(Image::Pixel lhs, Image::Pixel rhs) {
        return (lhs.r - rhs.r) * (lhs.r - rhs.r) +
               (lhs.g - rhs.g) * (lhs.g - rhs.g) +
               (lhs.b - rhs.b) * (lhs.b - rhs.b) +
               (lhs.a - rhs.a) * (lhs.a - rhs.a);
}
