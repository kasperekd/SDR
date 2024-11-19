#include <fstream>
#include <iostream>
#include <vector>

#include "my_config.h"
#include "my_init.h"
#include "my_utils.h"

int main(int argc, char *argv[]) {
    auto args = parse_args(argc, argv);

    std::string config_path = "config.cfg";  // default
    if (args.find("c") != args.end()) {
        config_path = args["c"];
    }

    Config config;
    if (!load_config(config_path, config)) {
        std::cerr << "Failed to load configuration" << std::endl;
        return 1;
    }

    PlutoDevices devices;
    if (!init_adalm_pluto(config.device_address, config.txcfg, config.rxcfg,
                          devices)) {
        std::cerr << "Failed to initialize ADALM-Pluto" << std::endl;
        return 1;
    }

    std::cout << "ADALM-Pluto initialized successfully!" << std::endl;
    // TODO Перенести в инициализацию
    devices.rxbuf = iio_device_create_buffer(devices.rx_dev, 0, devices.rxmask);
    devices.txbuf = iio_device_create_buffer(devices.tx_dev, 0, devices.txmask);
    static struct iio_stream *rxstream = NULL;
    static struct iio_stream *txstream = NULL;
    rxstream = iio_buffer_create_stream(devices.rxbuf, 4, config.buffer_size);
    txstream = iio_buffer_create_stream(devices.txbuf, 4, config.buffer_size);
    std::cout << "Buffer size: " << config.buffer_size << std::endl;

    size_t rx_sample_sz, tx_sample_sz;
    rx_sample_sz = iio_device_get_sample_size(devices.rx_dev, devices.rxmask);
    tx_sample_sz = iio_device_get_sample_size(devices.tx_dev, devices.txmask);

    // ==================MAIN BLOCK==================
    const struct iio_block *txblock, *rxblock;

    std::ofstream outfile("single_adalm_rx.txt", std::ios::out);
    int16_t *tx_i = NULL;
    int16_t *tx_q = NULL;
    size_t num_samples = 0;
    read_qpsk_signal("qpsk_signal.bin", &tx_i, &tx_q, &num_samples);

    int32_t counter = 0;
    std::vector<int16_t> rx_data_i = {0};
    std::vector<int16_t> rx_data_q = {0};

    while (true) {
        const struct iio_block *txblock = iio_stream_get_next_block(txstream);
        const struct iio_block *rxblock = iio_stream_get_next_block(rxstream);

        int16_t *p_dat;
        ptrdiff_t p_inc = tx_sample_sz;
        int16_t *p_end = static_cast<int16_t *>(iio_block_end(txblock));
        for (p_dat = static_cast<int16_t *>(
                 iio_block_first(txblock, devices.tx0_i));
             p_dat < p_end; p_dat += p_inc / sizeof(*p_dat)) {
            p_dat[0] = tx_i[counter % num_samples];  // I
            p_dat[1] = tx_q[counter % num_samples];  // Q
            // p_dat[0] = 0; // I
            // p_dat[1] = 0; // Q
        }

        // read
        p_inc = rx_sample_sz;
        p_end = static_cast<int16_t *>(iio_block_end(rxblock));
        int sample_index = 0;
        for (p_dat = static_cast<int16_t *>(
                 iio_block_first(rxblock, devices.rx0_i));
             p_dat < p_end; p_dat += p_inc / sizeof(*p_dat)) {
            rx_data_i.push_back(p_dat[0]);  // I
            rx_data_q.push_back(p_dat[1]);  // Q
            sample_index++;
        }

        counter++;

        if (counter >= 100) {
            break;
        }
    }

    for (size_t j = 0; j < rx_data_i.size(); j++) {
        outfile << rx_data_i[j] << ", " << rx_data_q[j] << std::endl;
    }

    free(tx_i);
    free(tx_q);
    outfile.close();
    cleanup_adalm_pluto(devices);

    // TODO Вынести в функцию очистки
    if (rxstream) {
        iio_stream_destroy(rxstream);
    }
    if (txstream) {
        iio_stream_destroy(txstream);
    }

    return 0;
}
