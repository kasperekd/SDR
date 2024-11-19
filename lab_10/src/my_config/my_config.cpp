#include "my_config.h"

// Утилита для удаления пробелов с краев строки
static std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, (last - first + 1));
}

// Чтение конфигурации из файла
bool load_config(const std::string& file_path, Config& config) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Failed to open config file: " << file_path << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Пропускаем пустые строки и комментарии
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }

        // Разделяем строку на ключ и значение
        size_t eq_pos = line.find('=');
        if (eq_pos == std::string::npos) {
            std::cerr << "Invalid line in config file: " << line << std::endl;
            return false;
        }
        std::string key = trim(line.substr(0, eq_pos));
        std::string value = trim(line.substr(eq_pos + 1));

        // Сопоставляем ключи с полями структуры Config
        if (key == "device_address") {
            config.device_address = value;
        } else if (key == "tx_bandwidth_hz") {
            config.txcfg.bw_hz = std::stoll(value);
        } else if (key == "tx_sample_rate_hz") {
            config.txcfg.fs_hz = std::stoll(value);
        } else if (key == "tx_lo_frequency_hz") {
            config.txcfg.lo_hz = std::stoll(value);
        } else if (key == "tx_rf_port") {
            config.txcfg.rfport = value.c_str();
        } else if (key == "rx_bandwidth_hz") {
            config.rxcfg.bw_hz = std::stoll(value);
        } else if (key == "rx_sample_rate_hz") {
            config.rxcfg.fs_hz = std::stoll(value);
        } else if (key == "rx_lo_frequency_hz") {
            config.rxcfg.lo_hz = std::stoll(value);
        } else if (key == "rx_rf_port") {
            config.rxcfg.rfport = value.c_str();
        } else if (key == "buffer_size") {
            config.buffer_size = std::stoul(value);
        } else {
            std::cerr << "Unknown config key: " << key << std::endl;
            return false;
        }
    }

    return true;
}