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
        -h/--help   - display this help menu

        -f/--file   - input filename

        -o/--output - output filename.
                      default: out.png
        -t/--temp   - starting temperature.
                        default: 1000.0

        -i/--iter   - maximal iterations.
                        default 10'000
        -r/--rect   - number of rectangles for approximation.
                        default 100

        --scheduler - temperature scheduler.
                        options: G&G, Geom, Lin
                        default: G&G

        -p/--params - parameters of schedulers.
                        options and defaults:
                                G&G - c=1
                                Geom - multiplier=0.995
                                Lin - slope=-0.01

)###");
}

int32_t input(int argc, char ** argv, Image & filein_img, std::string & fileout, scheduler_type & sch, uint64_t & max_iter, uint16_t & n_rect) {
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
                RECTANGLE    = 'r'
        };

        static struct option LONG_OPTIONS[] = {
                {"help",      no_argument, nullptr, HELP},
                {"file",      required_argument, nullptr, FILEIN},
                {"output",    required_argument, nullptr, FILEOUT},
                {"scheduler", required_argument, nullptr, SCHEDULER},
                {"params",    required_argument, nullptr, PARAMS},
                {"temp",      required_argument, nullptr, TEMPERATURE},
                {"iter",      required_argument, nullptr, ITERATIONS},
                {"rect",      required_argument, nullptr, RECTANGLE},
                {0, 0, 0, 0}
        };

        static constexpr char OPT_STR[] = "hf:o:p:t:i:r:";

        int32_t c = -1;
        std::string filein = "";
        std::string scheduler = "G&G";
        double params        = NAN;
        double temperature   = 1000;

        while((c = getopt_long(argc, argv, OPT_STR, LONG_OPTIONS, nullptr)) != NOARGS) {
                switch(c) {
                        case FLAGGED:
                                break;
                        case HELP:
                                print_help();
                                return EXIT_HELP;
                        case FILEIN:
                                DEBUG_OUTPUT("BBBBBBBBBBBBBBBBBBBBBBBBBBBBBB\n");
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
                                DEBUG_OUTPUT("AAAAAAAAAAAAAAAAAAAAAAAAaa\n");
                                max_iter = std::stoull(optarg);
                                break;
                        case RECTANGLE:
                                n_rect = std::stoul(optarg);
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
        filein_img = Image(filein);


        #ifdef DEBUG
        for(int i = 0; i < argc; ++i)
                printf("%s\n", argv[i]);
        #endif

        DEBUG_OUTPUT("FILEIN:   %s\n", filein.c_str());
        DEBUG_OUTPUT("FILEOUT:  %s\n", fileout.c_str());
        DEBUG_OUTPUT("MAX ITER: %lu\n", max_iter);
        DEBUG_OUTPUT("N RECT:   %u\n", n_rect);
        return EXIT_SUCCESS;
}

int32_t main(int argc, char *argv[]) {

        Image img;
        scheduler_type sch;
        std::string fileout = "out.png";
        uint64_t max_iter = 10'000;
        uint16_t n_rect = 100;

        int32_t code = input(argc, argv, img, fileout, sch, max_iter, n_rect);

        if(code == EXIT_HELP)
                return EXIT_SUCCESS;
        if(code != EXIT_SUCCESS)
                return code;


        Image img_res = simulated_annealing(img, sch, max_iter, n_rect);

        img_res.save(fileout);
}
