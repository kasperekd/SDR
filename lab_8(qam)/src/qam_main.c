#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qam.h"
#include "utils.h"

int main(int argc, char **argv) {
    QAMConfig config = {.level = QAM_16,
                        .carrier_freq = 1000.0,
                        .samples_per_symbol = 100,
                        .is_binary = 0};

    const char *message = NULL;
    const char *output_file = "./data.txt";

    struct option long_options[] = {{"q", required_argument, 0, 'q'},
                                    {"samples", required_argument, 0, 's'},
                                    {"fc", required_argument, 0, 'f'},
                                    {"text", no_argument, 0, 't'},
                                    {"bin", no_argument, 0, 'b'},
                                    {"m", required_argument, 0, 'm'},
                                    {"dest", required_argument, 0, 'd'},
                                    {0, 0, 0, 0}};

    int opt;
    while ((opt = getopt_long(argc, argv, "q:s:f:tbm:d:", long_options,
                              NULL)) != -1) {
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
            case 't':
                config.is_binary = 0;
                break;
            case 'b':
                config.is_binary = 1;
                break;
            case 'm':
                message = optarg;
                break;
            case 'd':
                output_file = optarg;
                break;
            default:
                fprintf(stderr, "Usage: %s [options]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (!message) {
        fprintf(stderr, "Message not provided.\n");
        exit(EXIT_FAILURE);
    }

    qam_modulate(message, &config, output_file);
    return 0;
}
