#include "Candidate.hpp"
#include "debug_utils.h"
#include <cassert>
#include <climits>
#include <random>
#include "stb_image.h"

Candidate::Candidate(const Image &img, uint16_t n_rect)
    :  MSE(0), img(img), w(img.width), h(img.height),local_mses(img.width, img.height),
      pixels(img.width, img.height) {
        static std::mt19937 generator;
        std::uniform_int_distribution<uint16_t> dist_x(0, w-1);
        std::uniform_int_distribution<uint16_t> dist_y(0, h-1);
        std::uniform_int_distribution<uint32_t> dist_pxl(0, UINT32_MAX >> 8);

        for(uint16_t i = 0; i < n_rect; ++i) {
                uint16_t x_min = dist_x(generator);
                uint16_t y_min = dist_y(generator);
                std::uniform_int_distribution<uint16_t> dist_xm(x_min, w-1);
                std::uniform_int_distribution<uint16_t> dist_ym(y_min, h-1);
                uint16_t x_max = dist_xm(generator);
                uint16_t y_max = dist_ym(generator);
                Rect r(x_min, y_min, x_max, y_max, (dist_pxl(generator) << 8) | 0xFF);
                DEBUG_OUTPUT("xmin=%u, ymin=%u, xmax=%u, ymax=%u\n", x_min, y_min, x_max, y_max);
                rects.push_back(r);
                rect_idx.emplace(r, rects.size() - 1);

                for(uint16_t j = y_min; j < y_max; ++j) {
                        for(uint16_t k = x_min; k < x_max; ++k) {
                                std::unordered_map<Image::Pixel, uint16_t> & pxls = pixels.at(k, j);
                                pxls[r.pxl] += 1;
                        }
                }
        }

        for(uint16_t j = 0; j < h; ++j) {
                for(uint16_t i = 0; i < w; ++i) {
                        local_mses.at(i, j) = compute_sq_error(img.at(i,j), mix_colors(pixels.at(i, j)));
                        MSE += local_mses.at(i, j);
                }
        }
}

void Candidate::copy_from(const Candidate & oth) {
        assert(oth.h == h);
        assert(oth.w == w);
        MSE = oth.MSE;
        DEBUG_OUTPUT("cccc\n");
        rects = oth.rects;
        DEBUG_OUTPUT("xxxx\n");
        rect_idx = oth.rect_idx;

        DEBUG_OUTPUT("aaaa\n");
        for(uint32_t i = 0; i < w * h; ++i) {
                local_mses.data[i] = oth.local_mses.data[i];
                pixels.data[i] = oth.pixels.data[i];
        }
        DEBUG_OUTPUT("bbbb\n");

        DEBUG_OUTPUT("Finished copying\n");
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

        //DEBUG_OUTPUT("PRECHANGE MSE:  %lf\n", MSE);
        assert(changeRect(src, dst));
        //DEBUG_OUTPUT("POSTCHANGE MSE: %lf\n", MSE);
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
        rect_idx.erase(src);
        rect_idx.emplace(dst, idx);
        return true;
}

void Candidate::clean_rect(const Rect & r) {
        //DEBUG_OUTPUT("CLEANING RECT (%u, %u) - (%u, %u)\n", r.x_min, r.y_min, r.x_max, r.y_max);
        for(uint16_t j = r.y_min; j <= r.y_max; ++j) {
                for(uint16_t i = r.x_min; i <= r.x_max; ++i) {
                        std::unordered_map<Image::Pixel, uint16_t> & pxls = pixels.at(i, j);
                        MSE -= local_mses.at(i, j);
                        pxls[r.pxl] -= 1;

                        Image::Pixel new_mixture = mix_colors(pxls);
                        local_mses.at(i, j) = compute_sq_error(new_mixture, img.at(i, j));
                        MSE += local_mses.at(i, j);
                }
        }
}

void Candidate::draw_rect(const Rect & r) {
        //DEBUG_OUTPUT("DRAWING RECT (%u, %u) - (%u, %u)\n", r.x_min, r.y_min, r.x_max, r.y_max);
        for(uint16_t j = r.y_min; j <= r.y_max; ++j) {
                for(uint16_t i = r.x_min; i <= r.x_max; ++i) {
                        std::unordered_map<Image::Pixel, uint16_t> & pxls = pixels.at(i, j);
                        MSE -= local_mses.at(i, j);
                        pxls[r.pxl] += 1;

                        Image::Pixel new_mixture = mix_colors(pxls);
                        local_mses.at(i, j) = compute_sq_error(new_mixture, img.at(i, j));
                        MSE += local_mses.at(i, j);
                }
        }

}

Image::Pixel Candidate::mix_colors(const std::unordered_map<Image::Pixel, uint16_t> & pxls) {
        if(pxls.empty()) return 0xff;
        uint32_t r=0, g=0, b=0, tcnt =0;

        for(const auto & [pxl, cnt] : pxls) {
                tcnt += cnt;
                r += pxl.r * cnt;
                g += pxl.g * cnt;
                b += pxl.b * cnt;
        }
        if(tcnt == 0) return 0xFF;
        r /= tcnt;
        g /= tcnt;
        b /= tcnt;
        return Image::Pixel(r, g, b, 0xFF);
}

double Candidate::compute_sq_error(Image::Pixel lhs, Image::Pixel rhs) {
        return (lhs.r - rhs.r) * (lhs.r - rhs.r) +
               (lhs.g - rhs.g) * (lhs.g - rhs.g) +
               (lhs.b - rhs.b) * (lhs.b - rhs.b);
}

Image Candidate::image() const {
        Image::Pixel * img_data = reinterpret_cast<Image::Pixel * >(malloc(w*h * sizeof(Image::Pixel)));
        for(uint16_t j = 0; j < h; ++j) {
                for(uint16_t i = 0; i < w; ++i) {
                        img_data[i + j * w] = mix_colors(pixels.at(i, j));
                }
        }
        return Image(reinterpret_cast<uint8_t*>(img_data), h, w);
}
