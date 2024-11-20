#ifndef MY_INIT_H
#define MY_INIT_H

#include <iio/iio-debug.h>
#include <iio/iio.h>

#include <iostream>
#include <string>

// #include "my_config.h"

struct StreamConfig {
    long long bw_hz;
    long long fs_hz;
    long long lo_hz;
    const char* rfport;
};

struct PlutoDevices {
    struct iio_context* ctx;
    struct iio_device* tx_dev;
    struct iio_device* rx_dev;
    struct iio_device* phy_dev;
    struct iio_channel* tx0_i;
    struct iio_channel* tx0_q;
    struct iio_channel* rx0_i;
    struct iio_channel* rx0_q;
    struct iio_buffer* txbuf;
    struct iio_buffer* rxbuf;
    struct iio_channels_mask* txmask;
    struct iio_channels_mask* rxmask;
    struct iio_stream* rxstream;
    struct iio_stream* txstream;
    size_t rx_sample_sz, tx_sample_sz;
};

bool init_adalm_pluto(const std::string& ip, const StreamConfig& txcfg,
                      const StreamConfig& rxcfg, PlutoDevices& devices,
                      size_t buffer_size);
void cleanup_adalm_pluto(PlutoDevices& devices);

#endif  // MY_INIT_H
