#include "Candidate.hpp"
#include "debug_utils.h"
#include <cassert>
#include <climits>
#include <random>
#include "stb_image.h"

Candidate::Candidate(const Image &img, uint16_t n_rect, uint64_t seed)
    :  MSE(0), img(img), w(img.width), h(img.height),local_mses(img.width, img.height),
      pixels(img.width, img.height), generator(seed) {
        std::uniform_int_distribution<uint16_t> dist_x(0, w-1);
        std::uniform_int_distribution<uint16_t> dist_y(0, h-1);
        std::uniform_int_distribution<uint32_t> dist_pxl(0, 0xFF'FF'FF);

        for(uint16_t i = 0; i < n_rect; ++i) {
                uint16_t x_min = dist_x(generator);
                uint16_t y_min = dist_y(generator);
                std::uniform_int_distribution<uint16_t> dist_xm(x_min, w-1);
                std::uniform_int_distribution<uint16_t> dist_ym(y_min, h-1);
                uint16_t x_max = dist_xm(generator);
                uint16_t y_max = dist_ym(generator);
                Rect r(x_min, y_min, x_max, y_max, dist_pxl(generator));
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
                        MSE += local_mses.at(i, j) / (w * h);
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
        std::uniform_int_distribution<uint16_t> distribution(0, rects.size() - 1);

        return rects[distribution(generator)];
}

// Mutates one Rect, calculates new MSE and returns new Rect
std::pair<Rect, Candidate::EMutation> Candidate::mutate(const Rect & src) {

        std::discrete_distribution<uint8_t> distribution_mutations({1,1,1,1,1});

        EMutation val = EMutation(distribution_mutations(generator));

        Rect dst = src;

        switch(val) {
                case COLOR: {
                        std::uniform_int_distribution<uint32_t> distribution(0, 0xFF'FF'FF);
                        dst.pxl = distribution(generator);
                        break;
                }
                case XMIN: {
                        std::uniform_int_distribution<uint16_t> dist(0, dst.x_max);
                        dst.x_min = dist(generator);
                        break;
                }
                case YMIN: {
                        std::uniform_int_distribution<uint16_t> dist(0, dst.y_max);
                        dst.y_min = dist(generator);
                        break;
                }
                case XMAX: {
                        std::uniform_int_distribution<uint16_t> dist(dst.x_min, w - 1);
                        dst.x_max = dist(generator);
                        break;
                }
                case YMAX: {
                        std::uniform_int_distribution<uint16_t> dist(dst.y_min, h - 1);
                        dst.y_max = dist(generator);
                        break;
                }
        }

        //DEBUG_OUTPUT("PRECHANGE MSE:  %lf\n", MSE);
        assert(changeRect(src, dst, val));
        //DEBUG_OUTPUT("POSTCHANGE MSE: %lf\n", MSE);
        return {dst, val};
}

// returns false if src does not exist in this candidate
bool Candidate::changeRect(const Rect & src, const Rect & dst, EMutation mut) {
        uint16_t idx;
        try {
                idx = rect_idx.at(src);
        }
        catch(const std::out_of_range &) {
                return false;
        }

        switch(mut) {

                case COLOR: {
                        clean_rect(src);
                        draw_rect(dst);
                        break;
                }
                case XMIN: {
                        if(src.x_min < dst.x_min) {
                                Rect rect = Rect(src.x_min, src.y_min, dst.x_min, src.y_max);
                                clean_rect(rect);
                        }
                        else {
                                Rect rect = Rect(dst.x_min, src.y_min, src.x_min, src.y_max);
                                draw_rect(rect);
                        }
                        break;
                }
                case YMIN: {
                        if(src.y_min < dst.y_min) {
                                Rect rect = Rect(src.x_min, src.y_min, src.x_max, dst.y_min);
                                clean_rect(rect);
                        }
                        else {
                                Rect rect = Rect(src.x_min, dst.y_min, src.x_max, src.y_min);
                                draw_rect(rect);
                        }
                        break;
                }
                case XMAX: {
                        if(src.x_max > dst.x_max) {
                                Rect rect = Rect(dst.x_max, src.y_min, src.x_max, src.y_max);
                                clean_rect(rect);
                        }
                        else {
                                Rect rect = Rect(src.x_max, src.y_min, dst.x_max, src.y_max);
                                draw_rect(rect);
                        }
                        break;
                }
                case YMAX: {
                        if(src.y_max > dst.y_max) {
                                Rect rect = Rect(src.x_max, dst.y_max, src.x_max, src.y_max);
                                clean_rect(rect);
                        }
                        else {
                                Rect rect = Rect(src.x_max, src.y_max, src.x_max, dst.y_max);
                                draw_rect(rect);
                        }
                        break;
                }
        }

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
                        MSE -= local_mses.at(i, j)/ (w * h);
                        pxls[r.pxl] -= 1;

                        Image::Pixel new_mixture = mix_colors(pxls);
                        local_mses.at(i, j) = compute_sq_error(new_mixture, img.at(i, j));
                        MSE += local_mses.at(i, j)/ (w * h);
                }
        }
}

void Candidate::draw_rect(const Rect & r) {
        //DEBUG_OUTPUT("DRAWING RECT (%u, %u) - (%u, %u)\n", r.x_min, r.y_min, r.x_max, r.y_max);
        for(uint16_t j = r.y_min; j <= r.y_max; ++j) {
                for(uint16_t i = r.x_min; i <= r.x_max; ++i) {
                        std::unordered_map<Image::Pixel, uint16_t> & pxls = pixels.at(i, j);
                        MSE -= local_mses.at(i, j)/ (w * h);
                        pxls[r.pxl] += 1;

                        Image::Pixel new_mixture = mix_colors(pxls);
                        local_mses.at(i, j) = compute_sq_error(new_mixture, img.at(i, j));
                        MSE += local_mses.at(i, j)/ (w * h);
                }
        }

}

Image::Pixel Candidate::mix_colors(const std::unordered_map<Image::Pixel, uint16_t> & pxls) {
        if(pxls.empty()) return 0;
        uint32_t r=0, g=0, b=0, tcnt =0;

        for(const auto & [pxl, cnt] : pxls) {
                tcnt += cnt;
                r += pxl.r * cnt;
                g += pxl.g * cnt;
                b += pxl.b * cnt;
        }
        if(tcnt == 0) return 0;
        r /= tcnt;
        g /= tcnt;
        b /= tcnt;
        return Image::Pixel(r, g, b);
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
