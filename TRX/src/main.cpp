#include <atomic>
#include <csignal>
#include <fstream>
#include <iostream>
#include <thread>
#include <unordered_map>
#include <vector>

#include "my_config.h"
#include "my_init.h"
#include "my_utils.h"

// DEFAULT PATHs
std::string config_path = "config.cfg";
std::string binar_path = "qpsk_signal.bin";
bool enable_rx = true;
bool enable_tx = true;
std::atomic<bool> keep_running(true);
size_t tx_iters = 0;  // 0 - inf
size_t rx_iters = 0;

void signal_handler(int signal) {
    if (signal == SIGINT) {
        keep_running = false;
        std::cout << "Кeceived SIGINT" << std::endl;
    }
}

void arg_process(std::unordered_map<std::string, std::string> args) {
    if (args.find("c") != args.end()) {
        config_path = args["c"];
    }

    if (args.find("b") != args.end()) {
        binar_path = args["b"];
    }

    if (args.find("rx_only") != args.end()) {
        enable_tx = false;
    }

    if (args.find("tx_only") != args.end()) {
        enable_rx = false;
    }

    if (args.find("tx_iters") != args.end()) {
        tx_iters = std::stoi(args["tx_iters"]);
    }

    if (args.find("rx_iters") != args.end()) {
        rx_iters = std::stoi(args["rx_iters"]);
    }

    if (args.find("iters") != args.end()) {
        rx_iters = std::stoi(args["iters"]);
        tx_iters = rx_iters;
    }
}

void transmit(PlutoDevices &devices, const int16_t *tx_i, const int16_t *tx_q,
              size_t num_samples) {
    std::cout << "TX is run" << std::endl;
    const struct iio_block *txblock;

    size_t _counter = 0;
    size_t iterations = 0;
    while (keep_running && (tx_iters == 0 || iterations < tx_iters)) {
        txblock = iio_stream_get_next_block(devices.txstream);
        int16_t *p_dat;
        ptrdiff_t p_inc = devices.tx_sample_sz;
        int16_t *p_end = static_cast<int16_t *>(iio_block_end(txblock));
        for (p_dat = static_cast<int16_t *>(
                 iio_block_first(txblock, devices.tx0_i));
             p_dat < p_end; p_dat += p_inc / sizeof(*p_dat)) {
            if (_counter < num_samples) {
                p_dat[0] = tx_i[_counter] * 512;  // I
                p_dat[1] = tx_q[_counter] * 512;  // Q
                _counter++;
            } else {
                _counter = 0;
            }
        }
        iterations++;
    }
    std::cout << "TX is down" << std::endl;
}

void receive(PlutoDevices &devices, std::ofstream &outfile) {
    std::cout << "RX is run" << std::endl;

    const struct iio_block *rxblock;

    std::vector<int16_t> rx_data_i;
    std::vector<int16_t> rx_data_q;
    size_t iterations = 0;

    while (keep_running && (rx_iters == 0 || iterations < rx_iters)) {
        rxblock = iio_stream_get_next_block(devices.rxstream);
        int16_t *p_dat;
        ptrdiff_t p_inc = devices.rx_sample_sz;
        int16_t *p_end = static_cast<int16_t *>(iio_block_end(rxblock));
        for (p_dat = static_cast<int16_t *>(
                 iio_block_first(rxblock, devices.rx0_i));
             p_dat < p_end; p_dat += p_inc / sizeof(*p_dat)) {
            rx_data_i.push_back(p_dat[0]);  // I
            rx_data_q.push_back(p_dat[1]);  // Q
        }

        for (size_t j = 0; j < rx_data_i.size(); j++) {
            outfile << rx_data_i[j] << ", " << rx_data_q[j] << std::endl;
        }

        rx_data_i.clear();
        rx_data_q.clear();
        iterations++;
    }
    std::cout << "RX is down" << std::endl;
}

int main(int argc, char *argv[]) {
    std::signal(SIGINT, signal_handler);

    auto args = parse_args(argc, argv);
    arg_process(args);

    Config config;
    if (!load_config(config_path, config)) {
        std::cerr << "Failed to load configuration" << std::endl;
        return 1;
    }

    PlutoDevices devices;
    if (!init_adalm_pluto(config.device_address, config.txcfg, config.rxcfg,
                          devices, config.buffer_size)) {
        std::cerr << "Failed to initialize ADALM-Pluto" << std::endl;
        return 1;
    }
    std::cout << "ADALM-Pluto initialized successfully!" << std::endl;

    int16_t *tx_i = NULL;
    int16_t *tx_q = NULL;
    size_t num_samples = 0;
    read_qpsk_signal(binar_path.c_str(), &tx_i, &tx_q, &num_samples);

    std::ofstream outfile("adalm_rx.txt", std::ios::out);

    std::thread tx_thread;
    std::thread rx_thread;

    if (enable_tx) {
        tx_thread =
            std::thread(transmit, std::ref(devices), tx_i, tx_q, num_samples);
    }
    if (enable_rx) {
        rx_thread = std::thread(receive, std::ref(devices), std::ref(outfile));
    }

    if (enable_tx) {
        tx_thread.join();
    }
    if (enable_rx) {
        rx_thread.join();
    }

    free(tx_i);
    free(tx_q);
    outfile.close();
    cleanup_adalm_pluto(devices);

    return 0;
}
