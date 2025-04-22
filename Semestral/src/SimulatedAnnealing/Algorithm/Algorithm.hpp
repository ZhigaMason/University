#ifndef _HPP_ALGORITHM_193801283019380
#define _HPP_ALGORITHM_193801283019380

#include "Image/Image.hpp"
#include "SimulatedAnnealing/Schedulers/Schedulers.hpp"

Image simulated_annealing(const std::string & src, scheduler_type scheduler, uint64_t max_iter, uint16_t n_rect, uint64_t seed, uint64_t save_cooldown, uint32_t n_samples, uint8_t n_thrs);

#endif//_HPP_ALGORITHM_193801283019380
