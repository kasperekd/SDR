#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <string>
#include <unordered_map>

void read_qpsk_signal(const char *filename, int16_t **tx_i, int16_t **tx_q,
                      size_t *num_samples);
std::unordered_map<std::string, std::string> parse_args(int argc, char *argv[]);

#endif  // UTILS_H