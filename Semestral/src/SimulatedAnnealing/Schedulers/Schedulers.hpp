#ifndef _HPP_SCHEDULER_SA_192839012830182038120380080
#define _HPP_SCHEDULER_SA_192839012830182038120380080

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>

using scheduler_type=std::function<double(uint64_t)>;

using scheduler_factory_type=std::function<scheduler_type(double, double)>;

extern scheduler_factory_type GermanNGermanFactory;
extern scheduler_factory_type GeometricFactory;
extern scheduler_factory_type LinearFactory;

const std::unordered_map<std::string, scheduler_factory_type> schedulerFactories = {
        {"G&G",  GermanNGermanFactory},
        {"Geom", GeometricFactory},
        {"Lin",  LinearFactory}
};

#endif//_HPP_SCHEDULER_SA_192839012830182038120380080
