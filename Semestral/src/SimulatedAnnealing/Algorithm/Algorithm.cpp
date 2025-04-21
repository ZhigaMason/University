#include "debug_utils.h"
#include "Algorithm.hpp"
#include "SimulatedAnnealing/Candidate/Candidate.hpp"
#include <random>
#include <filesystem>

Image simulated_annealing(const std::string & src_filename, scheduler_type scheduler, uint64_t max_iter, uint16_t n_rect, uint64_t seed, uint64_t save_cooldown) {
        using namespace SA_utils;
        using namespace std::filesystem;
        DEBUG_OUTPUT("SIMULATED ANNEALING\n");
        static std::mt19937 generator;
        std::uniform_real_distribution<double> dist(0, 1);

        Image img = Image(src_filename);
        Candidate curr(img, n_rect, seed);
        Candidate prev(curr);
        Candidate best(curr);

        std::string dir_name;
        if(save_cooldown) {
                uint32_t last_dir = src_filename.find_last_of("/");
                uint32_t last_dot = src_filename .find_last_of(".");
                dir_name = src_filename.substr(last_dir + 1, last_dot - last_dir - 1);
                DEBUG_OUTPUT("Last dir = %u, dot = %u, dir=%s\n", last_dir, last_dot, dir_name.c_str());
                if(!create_directory( dir_name + "/")) {
                        printf("Directory %s exists.\n", dir_name.c_str());
                }
        }

        for(uint64_t i = 0, last_copy = 0; i < max_iter; ++i, ++last_copy) {
                Rect src = curr.randomRect();
                auto [dst, mut] = curr.mutate(src);
                double p = exp((curr.MSE - prev.MSE) / scheduler(i));
                DEBUG_OUTPUT("=== i=%lu, best MSE=%lf, prev MSE=%lf, curr MSE=%lf\n", i, best.MSE, prev.MSE, curr.MSE);
                if(curr.MSE < prev.MSE || p >= dist(generator)) {
                        DEBUG_OUTPUT("==== change rect\n");
                        prev.changeRect(src, dst, mut);
                        prev.MSE = curr.MSE;
                }
                else {
                        curr.changeRect(dst, src, mut);
                        curr.MSE = prev.MSE;
                }
                if(prev.MSE < best.MSE && last_copy >= 500) {
                        DEBUG_OUTPUT("==== copying\n");
                        best.copy_from(prev);
                }

                if(save_cooldown && i % save_cooldown == 0) {
                        std::stringstream ss;
                        ss << dir_name << "/" << std::setfill('0') << std::setw(int(log10(max_iter) + 0.9999)) << i << ".png";
                        best.image().save(ss.str());
                }
        }

        if(prev.MSE < best.MSE) {
                DEBUG_OUTPUT("==== copying\n");
                best.copy_from(prev);
        }

        DEBUG_OUTPUT("  = FINISHED SA\n");
        return best.image();
}
