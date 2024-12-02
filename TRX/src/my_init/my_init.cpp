#include "my_init.h"

#include <iostream>
#include <stdexcept>

#define IIO_ENSURE(cond, msg)              \
    do {                                   \
        if (!(cond)) {                     \
            throw std::runtime_error(msg); \
        }                                  \
    } while (0)

bool init_adalm_pluto(const std::string& ip, const StreamConfig& txcfg,
                      const StreamConfig& rxcfg, PlutoDevices& devices,
                      size_t buffer_size, const std::string& gain_mode) {
    try {
        devices = {nullptr};

        std::cout << "* Creating IIO context at " << ip << "\n";
        devices.ctx = iio_create_context(NULL, ip.c_str());
        IIO_ENSURE(devices.ctx != nullptr, "Failed to create IIO context");

        std::cout << "* Finding devices\n";
        devices.tx_dev =
            iio_context_find_device(devices.ctx, "cf-ad9361-dds-core-lpc");
        devices.rx_dev = iio_context_find_device(devices.ctx, "cf-ad9361-lpc");
        devices.phy_dev = iio_context_find_device(devices.ctx, "ad9361-phy");
        IIO_ENSURE(devices.tx_dev != nullptr, "Failed to find TX device");
        IIO_ENSURE(devices.rx_dev != nullptr, "Failed to find RX device");
        IIO_ENSURE(devices.phy_dev != nullptr, "Failed to find PHY device");

        std::cout << "* Configuring TX\n";
        struct iio_channel* tx_chn =
            iio_device_find_channel(devices.phy_dev, "voltage0", true);
        IIO_ENSURE(tx_chn != nullptr, "Failed to find TX channel");
        iio_attr_write_string(iio_channel_find_attr(tx_chn, "rf_port_select"),
                              txcfg.rfport);
        iio_attr_write_longlong(iio_channel_find_attr(tx_chn, "rf_bandwidth"),
                                txcfg.bw_hz);
        iio_attr_write_longlong(
            iio_channel_find_attr(tx_chn, "sampling_frequency"), txcfg.fs_hz);
        iio_attr_write_longlong(
            iio_channel_find_attr(
                iio_device_find_channel(devices.phy_dev, "altvoltage1", true),
                "frequency"),
            txcfg.lo_hz);
        iio_attr_write_longlong(iio_channel_find_attr(tx_chn, "hardwaregain"),
                                70);

        std::cout << "* Configuring RX\n";
        struct iio_channel* rx_chn =
            iio_device_find_channel(devices.phy_dev, "voltage0", false);
        IIO_ENSURE(rx_chn != nullptr, "Failed to find RX channel");
        iio_attr_write_string(iio_channel_find_attr(rx_chn, "rf_port_select"),
                              rxcfg.rfport);
        iio_attr_write_longlong(iio_channel_find_attr(rx_chn, "rf_bandwidth"),
                                rxcfg.bw_hz);
        iio_attr_write_longlong(
            iio_channel_find_attr(rx_chn, "sampling_frequency"), rxcfg.fs_hz);
        iio_attr_write_longlong(
            iio_channel_find_attr(
                iio_device_find_channel(devices.phy_dev, "altvoltage0", true),
                "frequency"),
            rxcfg.lo_hz);

        const struct iio_attr* rx_gain_attr =
            iio_channel_find_attr(rx_chn, "gain_control_mode");
        IIO_ENSURE(rx_gain_attr != nullptr,
                   "Failed to find RX gain control attribute");
        iio_attr_write_string(rx_gain_attr, gain_mode.c_str());

        std::cout << "* Finding channels\n";
        devices.tx0_i =
            iio_device_find_channel(devices.tx_dev, "voltage0", true);
        devices.tx0_q =
            iio_device_find_channel(devices.tx_dev, "voltage1", true);
        devices.rx0_i =
            iio_device_find_channel(devices.rx_dev, "voltage0", false);
        devices.rx0_q =
            iio_device_find_channel(devices.rx_dev, "voltage1", false);
        IIO_ENSURE(
            devices.tx0_i && devices.tx0_q && devices.rx0_i && devices.rx0_q,
            "Failed to find required channels");

        std::cout << "* Creating channel masks\n";
        devices.rxmask = iio_create_channels_mask(
            iio_device_get_channels_count(devices.rx_dev));
        devices.txmask = iio_create_channels_mask(
            iio_device_get_channels_count(devices.tx_dev));
        IIO_ENSURE(devices.rxmask != nullptr,
                   "Failed to create RX channel mask");
        IIO_ENSURE(devices.txmask != nullptr,
                   "Failed to create TX channel mask");

        iio_channel_enable(devices.tx0_i, devices.txmask);
        iio_channel_enable(devices.tx0_q, devices.txmask);
        iio_channel_enable(devices.rx0_i, devices.rxmask);
        iio_channel_enable(devices.rx0_q, devices.rxmask);

        std::cout << "* Creating buffers\n";
        devices.txbuf =
            iio_device_create_buffer(devices.tx_dev, 0, devices.txmask);
        devices.rxbuf =
            iio_device_create_buffer(devices.rx_dev, 0, devices.rxmask);
        IIO_ENSURE(devices.txbuf != nullptr, "Failed to create TX buffer");
        IIO_ENSURE(devices.rxbuf != nullptr, "Failed to create RX buffer");
        devices.buffer_size = buffer_size;

        std::cout << "* Creating streams\n";
        devices.rxstream =
            iio_buffer_create_stream(devices.rxbuf, 4, buffer_size);
        devices.txstream =
            iio_buffer_create_stream(devices.txbuf, 4, buffer_size);
        std::cout << "Buffer size: " << buffer_size << std::endl;

        devices.rx_sample_sz =
            iio_device_get_sample_size(devices.rx_dev, devices.rxmask);
        devices.tx_sample_sz =
            iio_device_get_sample_size(devices.tx_dev, devices.txmask);

        std::cout << "ADALM-Pluto initialized successfully!\n";
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Error during initialization: " << e.what() << std::endl;
        cleanup_adalm_pluto(devices);
        return false;
    }
}

void cleanup_adalm_pluto(PlutoDevices& devices) {
    if (devices.rxstream) iio_stream_destroy(devices.rxstream);
    if (devices.txstream) iio_stream_destroy(devices.txstream);
    if (devices.txbuf) iio_buffer_destroy(devices.txbuf);
    if (devices.rxbuf) iio_buffer_destroy(devices.rxbuf);
    if (devices.rxmask) iio_channels_mask_destroy(devices.rxmask);
    if (devices.txmask) iio_channels_mask_destroy(devices.txmask);
    if (devices.ctx) iio_context_destroy(devices.ctx);
}
