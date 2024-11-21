#ifndef CONFIG_H
#define CONFIG_H

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

#include "my_init.h"

struct Config {
    StreamConfig txcfg;
    StreamConfig rxcfg;
    unsigned long buffer_size;
    std::string device_address;
    std::string gain_mode;
};

bool load_config(const std::string& file_path, Config& config);

#endif  // CONFIG_H
