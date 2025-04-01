#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string>
#include "debug_utils.h"

void print_help() {
        printf(R"###(
Usage:
        paint [-f/--file] <input-file> [[-o/--output] <output-file>]

Options:
        -h/--help   - display this help menu

        -f/--file   - specify input filename

        -o/--output - specify output filename.
                      default: out.png

        --scheduler - specify temperature scheduler.
                      default: G&G

        -p/--params - specify parameters

)###");
}

int main(int argc, char *argv[]) {

        enum EOptions : int {
                FLAGGED      = 0,
                UNKNOWN      = '?',
                NOARGS       = -1,

                HELP         = 'h',
                FILEIN       = 'f',
                FILEOUT      = 'o',
                SCHEDULER    = 1000,
                PARAMS       = 'p'
        };

        static struct option LONG_OPTIONS[] = {
                {"help",      no_argument, nullptr, HELP},
                {"file",      required_argument, nullptr, FILEIN},
                {"output",    required_argument, nullptr, FILEOUT},
                {"scheduler", required_argument, nullptr, SCHEDULER},
                {"params",    required_argument, nullptr, PARAMS},
                {0, 0, 0, 0}
        };

        static constexpr char OPT_STR[] = "hf:";

        int c = -1, idx = 0;
        bool set_out_file = false;
        std::string filein = "";
        std::string fileout  = "out.png";
        std::string schedule = "G&G";
        std::string params   = "";

        while((c = getopt_long(argc, argv, OPT_STR, LONG_OPTIONS, &idx)) != NOARGS) {
                switch(c) {
                        case FLAGGED:
                                break;
                        case HELP:
                                print_help();
                                return EXIT_SUCCESS;
                        case FILEIN:
                                filein = optarg;
                                break;
                        case FILEOUT:
                                fileout = optarg;
                                set_out_file = true;
                                break;
                        case SCHEDULER:
                                schedule = optarg;
                                break;
                        case PARAMS:
                                params = optarg;
                                break;
                        case UNKNOWN:
                        default:
                                break;
                };
        }

        DEBUG_OUTPUT("ARGC: %d, idx: %d\n", argc, idx);

        if(idx == (argc - 1) && filein.empty()) {
                printf("Provide file with %s <filename> or %s -f/--file <filename>\n", argv[0], argv[0]);
                return EXIT_FAILURE;
        }

        if(idx == (argc - 2)) {
                if(set_out_file)
                        filein = argv[idx+1];
                else
                        fileout = argv[idx+1];
        }
        else if(idx == (argc - 3)) {
                filein = argv[idx+1];
                fileout = argv[idx+2];
        }
        else {
                printf("Too many positional arguments provided: expected 1, obtained %d\n", argc - idx - 1);
                return EXIT_FAILURE;
        }

        DEBUG_OUTPUT("FILEIN:  %s\n", filein.c_str());
        DEBUG_OUTPUT("FILEOUT: %s\n", fileout.c_str());

}
