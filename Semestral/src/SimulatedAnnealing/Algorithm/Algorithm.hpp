#ifndef _HPP_ALGORITHM_193801283019380
#define _HPP_ALGORITHM_193801283019380

#include "Image/Image.hpp"
#include "SimulatedAnnealing/Schedulers/Schedulers.hpp"

Image simulated_annealing(const Image & src, scheduler_type scheduler, uint64_t max_iter, uint16_t n_rect);

#endif//_HPP_ALGORITHM_193801283019380
