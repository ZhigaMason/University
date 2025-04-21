#include "debug_utils.h"
#include "Algorithm.hpp"
#include "SimulatedAnnealing/Candidate/Candidate.hpp"
#include <random>

Image simulated_annealing(const Image & src, scheduler_type scheduler, uint64_t max_iter, uint16_t n_rect) {
        using namespace SA_utils;
        DEBUG_OUTPUT("SIMULATED ANNEALING\n");
        static std::mt19937 generator;
        std::uniform_real_distribution<double> dist(0, 1);

        Candidate curr(src, n_rect);
        Candidate prev(curr);
        Candidate best(curr);

        for(uint64_t i = 0, last_copy = 0; i < max_iter; ++i, ++last_copy) {
                Rect src = curr.randomRect();
                Rect dst = curr.mutate(src);
                double p = exp((curr.MSE - prev.MSE) / scheduler(i));
                DEBUG_OUTPUT("=== i=%lu, best MSE=%lf, prev MSE=%lf, curr MSE=%lf\n", i, best.MSE, prev.MSE, curr.MSE);
                if(curr.MSE < prev.MSE || p >= dist(generator)) {
                        DEBUG_OUTPUT("==== change rect\n");
                        prev.changeRect(src, dst);
                        prev.MSE = curr.MSE;
                }
                else {
                        curr.changeRect(dst, src);
                        curr.MSE = prev.MSE;
                }
                if(prev.MSE < best.MSE && last_copy >= 100) {
                        DEBUG_OUTPUT("==== copying\n");
                        best.copy_from(prev);
                }
        }

        DEBUG_OUTPUT("  = FINISHED SA\n");
        return best.image();
}
