#include "my_init.h"

bool init_adalm_pluto(const std::string& ip, const StreamConfig& txcfg,
                      const StreamConfig& rxcfg, PlutoDevices& devices) {
    devices.ctx = iio_create_context(NULL, ip.c_str());
    if (!devices.ctx) {
        std::cerr << "Failed to create IIO context at " << ip << std::endl;
        return false;
    }
    std::cout << "IIO context created at " << ip << std::endl;

    devices.tx_dev =
        iio_context_find_device(devices.ctx, "cf-ad9361-dds-core-lpc");
    devices.rx_dev = iio_context_find_device(devices.ctx, "cf-ad9361-lpc");
    devices.phy_dev = iio_context_find_device(devices.ctx, "ad9361-phy");

    if (!devices.tx_dev || !devices.rx_dev || !devices.phy_dev) {
        std::cerr << "Failed to find required devices in context" << std::endl;
        iio_context_destroy(devices.ctx);
        return false;
    }

    // Configure TX
    struct iio_channel* tx_chn =
        iio_device_find_channel(devices.phy_dev, "voltage0", true);
    iio_attr_write_string(iio_channel_find_attr(tx_chn, "rf_port_select"),
                          txcfg.rfport);
    iio_attr_write_longlong(iio_channel_find_attr(tx_chn, "rf_bandwidth"),
                            txcfg.bw_hz);
    iio_attr_write_longlong(iio_channel_find_attr(tx_chn, "sampling_frequency"),
                            txcfg.fs_hz);
    iio_attr_write_longlong(
        iio_channel_find_attr(
            iio_device_find_channel(devices.phy_dev, "altvoltage1", true),
            "frequency"),
        txcfg.lo_hz);

    // Configure RX
    struct iio_channel* rx_chn =
        iio_device_find_channel(devices.phy_dev, "voltage0", false);
    iio_attr_write_string(iio_channel_find_attr(rx_chn, "rf_port_select"),
                          rxcfg.rfport);
    iio_attr_write_longlong(iio_channel_find_attr(rx_chn, "rf_bandwidth"),
                            rxcfg.bw_hz);
    iio_attr_write_longlong(iio_channel_find_attr(rx_chn, "sampling_frequency"),
                            rxcfg.fs_hz);
    iio_attr_write_longlong(
        iio_channel_find_attr(
            iio_device_find_channel(devices.phy_dev, "altvoltage0", true),
            "frequency"),
        rxcfg.lo_hz);

    // Initialize TX/RX channels
    devices.tx0_i = iio_device_find_channel(devices.tx_dev, "voltage0", true);
    devices.tx0_q = iio_device_find_channel(devices.tx_dev, "voltage1", true);
    devices.rx0_i = iio_device_find_channel(devices.rx_dev, "voltage0", false);
    devices.rx0_q = iio_device_find_channel(devices.rx_dev, "voltage1", false);

    if (!devices.tx0_i || !devices.tx0_q || !devices.rx0_i || !devices.rx0_q) {
        std::cerr << "Failed to find channels" << std::endl;
        iio_context_destroy(devices.ctx);
        return false;
    }

    // Create masks for RX and TX
    devices.rxmask =
        iio_create_channels_mask(iio_device_get_channels_count(devices.rx_dev));
    devices.txmask =
        iio_create_channels_mask(iio_device_get_channels_count(devices.tx_dev));
    if (!devices.rxmask || !devices.txmask) {
        std::cerr << "Failed to create channels mask" << std::endl;
        iio_context_destroy(devices.ctx);
        return false;
    }

    // Enable channels
    iio_channel_enable(devices.tx0_i, devices.txmask);
    iio_channel_enable(devices.tx0_q, devices.txmask);
    iio_channel_enable(devices.rx0_i, devices.rxmask);
    iio_channel_enable(devices.rx0_q, devices.rxmask);

    // Create buffers
    devices.txbuf = iio_device_create_buffer(devices.tx_dev, 0, devices.txmask);
    devices.rxbuf = iio_device_create_buffer(devices.rx_dev, 0, devices.rxmask);
    if (!devices.txbuf || !devices.rxbuf) {
        std::cerr << "Failed to create buffers" << std::endl;
        iio_context_destroy(devices.ctx);
        return false;
    }

    // TODO: ADD iio_buffer_create_stream to here

    return true;
}

void cleanup_adalm_pluto(PlutoDevices& devices) {
    if (devices.txbuf) iio_buffer_destroy(devices.txbuf);
    if (devices.rxbuf) iio_buffer_destroy(devices.rxbuf);
    if (devices.rxmask) iio_channels_mask_destroy(devices.rxmask);
    if (devices.txmask) iio_channels_mask_destroy(devices.txmask);
    if (devices.ctx) iio_context_destroy(devices.ctx);
}
