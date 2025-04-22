#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>
#include <string>
#include "debug_utils.h"
#include "Image/Image.hpp"
#include "SimulatedAnnealing/Schedulers/Schedulers.hpp"
#include "SimulatedAnnealing/Algorithm/Algorithm.hpp"

constexpr int32_t EXIT_HELP = 100;
void print_help() {
        printf(R"###(
Usage:
        paint [-f/--file] <input-file> [[-o/--output] <output-file>]

Options:
        -h/--help    - display this help menu

        -f/--file    - input filename

        -o/--output  - output filename.
                         default: s<seed>-i<iter>-r<rect>-<file:.png>

        -s/--seed    - seed used for simulated annealing
                         default: 0

        -t/--temp    - starting temperature.
                         default: 1000.0

        -i/--iter    - maximal iterations.
                         default: 10'000

        -r/--rect    - number of rectangles for approximation.
                         default: 100

        -c/--colors  - number of sampled colors from picture
                         default: 32

        -w/--workers - number of worker threads to perform mutations. Setting to one discards any multithreading.
                         default: 2

        --cooldown   - saving cooldown. If set to zero, do not save until complete.
                       Saves best result with prefix inficating number of iteration into <file> directory without extension and path.
                         default: 0

        --scheduler  - temperature scheduler.
                         options: G&G, Geom, Lin
                         default: G&G

        -p/--params  - parameters of schedulers.
                        options and defaults:
                                G&G - c=1
                                Geom - multiplier=0.995
                                Lin - slope=-0.01

)###");
}

int32_t input(int argc, char ** argv, std::string & filein, std::string & fileout, scheduler_type & sch, uint64_t & max_iter, uint16_t & n_rect, uint64_t & seed, uint64_t & cooldown, uint32_t & n_samples, uint8_t & n_thrs) {
        enum EOptions : int {
                FLAGGED      = 0,
                UNKNOWN      = '?',
                NOARGS       = -1,

                OPTARG       = ':',

                HELP         = 'h',
                FILEIN       = 'f',
                FILEOUT      = 'o',
                SCHEDULER    = 1000,
                PARAMS       = 'p',
                TEMPERATURE  = 't',
                ITERATIONS   = 'i',
                RECTANGLE    = 'r',
                SEED         = 's',
                COLORS       = 'c',
                WORKERS      = 'w',
                COOLDOWN     = 1001,
        };

        static struct option LONG_OPTIONS[] = {
                {"help",      no_argument, nullptr, HELP},
                {"file",      required_argument, nullptr, FILEIN},
                {"output",    required_argument, nullptr, FILEOUT},
                {"scheduler", required_argument, nullptr, SCHEDULER},
                {"params",    required_argument, nullptr, PARAMS},
                {"temp",      required_argument, nullptr, TEMPERATURE},
                {"iter",      required_argument, nullptr, ITERATIONS},
                {"colors",    required_argument, nullptr, COLORS},
                {"rect",      required_argument, nullptr, RECTANGLE},
                {"seed",      required_argument, nullptr, SEED},
                {"cooldown",  required_argument, nullptr, COOLDOWN},
                {"workers",   required_argument, nullptr, WORKERS},
                {0, 0, 0, 0}
        };

        static constexpr char OPT_STR[] = "hf:o:p:t:i:r:s:w:";

        int32_t c = -1;
        std::string scheduler = "G&G";
        double params        = NAN;
        double temperature   = 1000;

        while((c = getopt_long(argc, argv, OPT_STR, LONG_OPTIONS, nullptr)) != NOARGS) {
                DEBUG_OUTPUT("c=%c\n", c);
                switch(c) {
                        case FLAGGED:
                                break;
                        case HELP:
                                print_help();
                                return EXIT_HELP;
                        case FILEIN:
                                filein = optarg;
                                break;
                        case FILEOUT:
                                fileout = optarg;
                                break;
                        case SCHEDULER:
                                scheduler = optarg;
                                break;
                        case PARAMS:
                                params = std::stod(optarg);
                                break;
                        case TEMPERATURE:
                                temperature = std::stod(optarg);
                                break;
                        case ITERATIONS:
                                max_iter = std::stoull(optarg);
                                break;
                        case RECTANGLE:
                                n_rect = std::stoul(optarg);
                                break;
                        case SEED:
                                seed = std::stoull(optarg);
                                break;
                        case COOLDOWN:
                                cooldown = std::stoull(optarg);
                                break;
                        case COLORS:
                                n_samples = std::stoul(optarg);
                                break;
                        case WORKERS:
                                n_thrs = std::stoul(optarg);
                                break;
                        case UNKNOWN:
                        default:
                                break;
                };
        }

        DEBUG_OUTPUT("ARGC: %d\n", argc);

        if(filein == "") {
                printf("Provide file with %s <filename> or %s -f/--file <filename>\n", argv[0], argv[0]);
                return EXIT_FAILURE;
        }

        if(!scheduler_factories.contains(scheduler)) {
                printf("Scheduler \"%s\" is not known. Refer to %s -h for help", scheduler.c_str(), argv[0]);
                return EXIT_FAILURE;
        }

        if(params == NAN) {
                params = default_factories_parameters.at(scheduler);
        }

        DEBUG_OUTPUT("VALID INPUT\n");

        sch = scheduler_factories.at(scheduler)(params, temperature);

        #ifdef DEBUG
        for(int i = 0; i < argc; ++i)
                printf("%s\n", argv[i]);
        #endif

        DEBUG_OUTPUT("FILEIN:   %s\n", filein.c_str());
        DEBUG_OUTPUT("FILEOUT:  %s\n", fileout.c_str());
        DEBUG_OUTPUT("MAX ITER: %lu\n", max_iter);
        DEBUG_OUTPUT("N RECT:   %u\n", n_rect);
        DEBUG_OUTPUT("SEED:     %lu\n", seed);
        DEBUG_OUTPUT("COOLDOWN: %lu\n", cooldown);
        DEBUG_OUTPUT("NSAMPLES: %u\n", n_samples);
        DEBUG_OUTPUT("NTHREADS: %u\n", n_thrs);
        return EXIT_SUCCESS;
}

int32_t main(int argc, char *argv[]) {

        std::string filein;
        scheduler_type sch;
        std::string fileout = "";
        uint64_t max_iter = 10'000;
        uint64_t save_cooldown = 0;
        uint64_t seed = 0;
        uint16_t n_rect = 100;
        uint32_t n_samples = 32;
        uint8_t  n_thrs = 2;

        int32_t code = input(argc, argv, filein, fileout, sch, max_iter, n_rect, seed, save_cooldown, n_samples, n_thrs);

        if(code == EXIT_HELP)
                return EXIT_SUCCESS;
        if(code != EXIT_SUCCESS)
                return code;


        Image img_res = simulated_annealing(filein, sch, max_iter, n_rect, seed, save_cooldown, n_samples, n_thrs);

        if(fileout == "") {
                uint32_t last_dir = filein.find_last_of("/");
                uint32_t last_dot = filein.find_last_of(".");
                DEBUG_OUTPUT("Last dir = %u, dot = %u\n", last_dir, last_dot);
                fileout = "s" + std::to_string(seed) + "-i" + std::to_string(max_iter) + "-r" + std::to_string(n_rect) + "-" + filein.substr(last_dir + 1, last_dot - last_dir - 1) + ".png";
        }
        img_res.save(fileout);
}
