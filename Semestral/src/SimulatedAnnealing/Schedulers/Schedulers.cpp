#include "Schedulers.hpp"
#include <cmath>

scheduler_factory_type GermanNGermanFactory = [] (double c, double start_temp) -> scheduler_type {
        return [=] (uint64_t it) -> double {
                return start_temp * (c / log(it + 1));
        };
};

scheduler_factory_type GeometricFactory = [] (double p, double start_temp) -> scheduler_type {
        return [=] (uint64_t it) -> double {
                return start_temp * (pow(p, it));
        };
};

scheduler_factory_type LinearFactory = [] (double p, double start_temp) -> scheduler_type {
        return [=] (uint64_t it) -> double {
                return start_temp  + p * it;
        };
};
