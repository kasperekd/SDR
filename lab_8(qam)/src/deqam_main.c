#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "qam.h"
#include "utils.h"

int main(int argc, char **argv) {
    QAMConfig config = {.level = QAM_16,
                        .carrier_freq = 1000.0,
                        .samples_per_symbol = 100,
                        .is_binary = 0};

    const char *input_file = NULL;
    const char *output_file = "./output.txt";

    struct option long_options[] = {{"q", required_argument, 0, 'q'},
                                    {"samples", required_argument, 0, 's'},
                                    {"fc", required_argument, 0, 'f'},
                                    {"dest", required_argument, 0, 'd'},
                                    {0, 0, 0, 0}};

    int opt;
    while ((opt = getopt_long(argc, argv, "q:s:f:d:", long_options, NULL)) !=
           -1) {
        switch (opt) {
            case 'q':
                config.level = atoi(optarg);
                break;
            case 's':
                config.samples_per_symbol = atoi(optarg);
                break;
            case 'f':
                config.carrier_freq = atof(optarg);
                break;
            case 'd':
                output_file = optarg;
                break;
            default:
                fprintf(stderr,
                        "Usage: %s --q <level> --samples <n> --fc <frequency> "
                        "--dest <file>\n",
                        argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // if (!input_file) {
    //     fprintf(stderr, "Input file not provided.\n");
    //     exit(EXIT_FAILURE);
    // }

    // qam_demodulate(input_file, &config, output_file);
    qam_demodulate("./data.txt", &config, output_file);
    return 0;
}
