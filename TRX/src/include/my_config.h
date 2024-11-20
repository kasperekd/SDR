#ifndef CONFIG_H
#define CONFIG_H

#include <fstream>
#include <iostream>
#include <sstream>

#include "my_init.h"

struct Config {
    std::string device_address;
    StreamConfig txcfg;
    StreamConfig rxcfg;
    size_t buffer_size;
};

bool load_config(const std::string& file_path, Config& config);

#endif  // CONFIG_H
