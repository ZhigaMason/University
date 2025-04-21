#include "Random.hpp"

#include <random>

std::vector<Image::Pixel> random_sampled_pixels(const Image::Pixel * arr, uint16_t w, uint16_t h, uint16_t samples, uint64_t seed) {
        std::mt19937 generator(seed ^ (seed << 1));
        std::uniform_int_distribution<uint32_t> dist(0, (w - 1) * (h - 1));

        std::vector<Image::Pixel> ret{};
        ret.reserve(samples);

        for(uint16_t i = 0; i < samples; ++i)
                ret.push_back(arr[dist(generator)]);
        return ret;
}
