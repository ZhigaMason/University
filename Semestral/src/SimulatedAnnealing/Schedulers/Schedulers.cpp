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

const std::unordered_map<std::string, scheduler_factory_type> scheduler_factories = {
        {"GnG",  GermanNGermanFactory},
        {"Geom", GeometricFactory},
        {"Lin",  LinearFactory}
};

const std::unordered_map<std::string, double> default_factories_parameters  = {
        {"GnG",  1.0},
        {"Geom", 0.995},
        {"Lin",  -0.01}
};
