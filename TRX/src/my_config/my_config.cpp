#include "my_config.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

static std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, (last - first + 1));
}

long long parse_frequency(const std::string& value) {
    std::istringstream iss(value);
    double freq;
    std::string unit;

    if (!(iss >> freq >> unit)) {
        throw std::invalid_argument("Invalid frequency format");
    }

    if (unit == "Hz") {
        return static_cast<long long>(freq);
    } else if (unit == "kHz") {
        return static_cast<long long>(freq * 1000);
    } else if (unit == "MHz") {
        return static_cast<long long>(freq * 1000000);
    } else if (unit == "GHz") {
        return static_cast<long long>(freq * 1000000000);
    } else if (unit == "MSPS") {
        return static_cast<long long>(freq * 1000000);
    } else if (unit == "kSPS") {
        return static_cast<long long>(freq * 1000);
    } else {
        throw std::invalid_argument("Unknown frequency unit: " + unit);
    }
}

void validate_gain_mode(const std::string& gain_mode) {
    if (gain_mode != "fast_attack" && gain_mode != "slow_attack" &&
        gain_mode != "manual") {
        throw std::invalid_argument(
            "Invalid gain mode: " + gain_mode +
            ". Valid options are: fast_attack, slow_attack, manual.");
    }
}

void validate_config(const Config& config) {
    if (config.txcfg.lo_hz < 70000000 || config.txcfg.lo_hz > 6000000000) {
        throw std::invalid_argument(
            "Tx LO frequency must be between 70 MHz and 6 GHz");
    }
    if (config.rxcfg.lo_hz < 70000000 || config.rxcfg.lo_hz > 6000000000) {
        throw std::invalid_argument(
            "Rx LO frequency must be between 70 MHz and 6 GHz");
    }

    if (config.txcfg.bw_hz < 200000 || config.txcfg.bw_hz > 56000000) {
        throw std::invalid_argument(
            "Tx channel bandwidth must be between 200 kHz and 56 MHz");
    }
    if (config.rxcfg.bw_hz < 200000 || config.rxcfg.bw_hz > 56000000) {
        throw std::invalid_argument(
            "Rx channel bandwidth must be between 200 kHz and 56 MHz");
    }

    if (config.txcfg.fs_hz < 61100 || config.txcfg.fs_hz > 61440000) {
        throw std::invalid_argument(
            "Tx sample rate must be between 61.44 MSPS and 65.1 kSPS");
    }
    if (config.rxcfg.fs_hz < 61100 || config.rxcfg.fs_hz > 61440000) {
        throw std::invalid_argument(
            "Rx sample rate must be between 61.44 MSPS and 65.1 kSPS");
    }
    validate_gain_mode(config.gain_mode);
}

bool load_config(const std::string& file_path, Config& config) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Failed to open config file: " << file_path << std::endl;
        return false;
    }

    config.gain_mode = "slow_attack";

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }

        size_t eq_pos = line.find('=');
        if (eq_pos == std::string::npos) {
            std::cerr << "Invalid line in config file: " << line << std::endl;
            return false;
        }
        std::string key = trim(line.substr(0, eq_pos));
        std::string value = trim(line.substr(eq_pos + 1));

        try {
            if (key == "device_address") {
                config.device_address = value;
            } else if (key == "tx_bandwidth_hz") {
                config.txcfg.bw_hz = parse_frequency(value);
            } else if (key == "tx_sample_rate_hz") {
                config.txcfg.fs_hz = parse_frequency(value);
            } else if (key == "tx_lo_frequency_hz") {
                config.txcfg.lo_hz = parse_frequency(value);
            } else if (key == "tx_rf_port") {
                // config.txcfg.rfport = value.c_str();
                free((void*)config.txcfg.rfport);
                config.txcfg.rfport = strdup(value.c_str());
            } else if (key == "rx_bandwidth_hz") {
                config.rxcfg.bw_hz = parse_frequency(value);
            } else if (key == "rx_sample_rate_hz") {
                config.rxcfg.fs_hz = parse_frequency(value);
            } else if (key == "rx_lo_frequency_hz") {
                config.rxcfg.lo_hz = parse_frequency(value);
            } else if (key == "rx_rf_port") {
                // config.rxcfg.rfport = value.c_str();
                free((void*)config.rxcfg.rfport);
                config.rxcfg.rfport = strdup(value.c_str());
            } else if (key == "buffer_size") {
                config.buffer_size = std::stoul(value);
            } else if (key == "rx_gain_mode") {
                config.gain_mode = value;
            } else if (key == "multiplier") {
                try {
                    int value_ = std::stoi(value);
                    if (value_ < INT16_MIN || value_ > INT16_MAX) {
                        throw std::out_of_range(
                            "Value is out of range for int16_t");
                    }
                    config.multiplier = static_cast<int16_t>(value_);
                } catch (const std::invalid_argument& e) {
                    std::cerr << "Invalid argument for multiplier: " << e.what()
                              << std::endl;
                    return false;
                } catch (const std::out_of_range& e) {
                    std::cerr << "Out of range for multiplier: " << e.what()
                              << std::endl;
                    return false;
                }
            } else {
                std::cerr << "Unknown config key: " << key << std::endl;
                return false;
            }
        } catch (const std::invalid_argument& e) {
            std::cerr << "Error parsing value for key '" << key
                      << "': " << e.what() << std::endl;
            return false;
        }
    }

    try {
        validate_config(config);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Configuration validation error: " << e.what()
                  << std::endl;
        return false;
    }
    return true;
}

void free_config(Config& config) {
    free((void*)config.txcfg.rfport);
    free((void*)config.rxcfg.rfport);
}