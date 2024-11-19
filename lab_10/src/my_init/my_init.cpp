#include "my_init.h"

// TODO: Изучить правильный подход к обработке ошибок в libiio

bool init_adalm_pluto(const std::string& ip, const StreamConfig& txcfg,
                      const StreamConfig& rxcfg, PlutoDevices& devices) {
    devices = {nullptr};

    // FIXME: Возвращает не нуль даже при отсутствии устройства
    devices.ctx = iio_create_context(NULL, ip.c_str());
    // IIO_ENSURE(devices.ctx && "No context")
    if (!devices.ctx) {
        std::cerr << "Error: Failed to create IIO context at " << ip
                  << std::endl;
        return false;
    }
    std::cout << "IIO context created at " << ip << std::endl;

    // Находим устройства
    devices.tx_dev =
        iio_context_find_device(devices.ctx, "cf-ad9361-dds-core-lpc");
    devices.rx_dev = iio_context_find_device(devices.ctx, "cf-ad9361-lpc");
    devices.phy_dev = iio_context_find_device(devices.ctx, "ad9361-phy");

    if (!devices.tx_dev || !devices.rx_dev || !devices.phy_dev) {
        std::cerr << "Error: Failed to find one or more required devices in "
                     "the context"
                  << std::endl;
        cleanup_adalm_pluto(devices);
        return false;
    }

    // TX
    struct iio_channel* tx_chn =
        iio_device_find_channel(devices.phy_dev, "voltage0", true);
    if (!tx_chn ||
        !iio_attr_write_string(iio_channel_find_attr(tx_chn, "rf_port_select"),
                               txcfg.rfport) ||
        !iio_attr_write_longlong(iio_channel_find_attr(tx_chn, "rf_bandwidth"),
                                 txcfg.bw_hz) ||
        !iio_attr_write_longlong(
            iio_channel_find_attr(tx_chn, "sampling_frequency"), txcfg.fs_hz) ||
        !iio_attr_write_longlong(
            iio_channel_find_attr(
                iio_device_find_channel(devices.phy_dev, "altvoltage1", true),
                "frequency"),
            txcfg.lo_hz)) {
        std::cerr << "Error: Failed to configure transmitter" << std::endl;
        cleanup_adalm_pluto(devices);
        return false;
    }

    // RX
    struct iio_channel* rx_chn =
        iio_device_find_channel(devices.phy_dev, "voltage0", false);
    if (!rx_chn ||
        !iio_attr_write_string(iio_channel_find_attr(rx_chn, "rf_port_select"),
                               rxcfg.rfport) ||
        !iio_attr_write_longlong(iio_channel_find_attr(rx_chn, "rf_bandwidth"),
                                 rxcfg.bw_hz) ||
        !iio_attr_write_longlong(
            iio_channel_find_attr(rx_chn, "sampling_frequency"), rxcfg.fs_hz) ||
        !iio_attr_write_longlong(
            iio_channel_find_attr(
                iio_device_find_channel(devices.phy_dev, "altvoltage0", true),
                "frequency"),
            rxcfg.lo_hz)) {
        std::cerr << "Error: Failed to configure receiver" << std::endl;
        cleanup_adalm_pluto(devices);
        return false;
    }

    devices.tx0_i = iio_device_find_channel(devices.tx_dev, "voltage0", true);
    devices.tx0_q = iio_device_find_channel(devices.tx_dev, "voltage1", true);
    devices.rx0_i = iio_device_find_channel(devices.rx_dev, "voltage0", false);
    devices.rx0_q = iio_device_find_channel(devices.rx_dev, "voltage1", false);

    if (!devices.tx0_i || !devices.tx0_q || !devices.rx0_i || !devices.rx0_q) {
        std::cerr << "Error: Failed to find required channels" << std::endl;
        cleanup_adalm_pluto(devices);
        return false;
    }

    devices.rxmask =
        iio_create_channels_mask(iio_device_get_channels_count(devices.rx_dev));
    devices.txmask =
        iio_create_channels_mask(iio_device_get_channels_count(devices.tx_dev));
    if (!devices.rxmask || !devices.txmask) {
        std::cerr << "Error: Failed to create channel masks" << std::endl;
        cleanup_adalm_pluto(devices);
        return false;
    }

    iio_channel_enable(devices.tx0_i, devices.txmask);
    iio_channel_enable(devices.tx0_q, devices.txmask);
    iio_channel_enable(devices.rx0_i, devices.rxmask);
    iio_channel_enable(devices.rx0_q, devices.rxmask);

    // буферы
    devices.txbuf = iio_device_create_buffer(devices.tx_dev, 0, devices.txmask);
    devices.rxbuf = iio_device_create_buffer(devices.rx_dev, 0, devices.rxmask);
    if (!devices.txbuf || !devices.rxbuf) {
        std::cerr << "Error: Failed to create buffers" << std::endl;
        cleanup_adalm_pluto(devices);
        return false;
    }

    std::cout << "ADALM-Pluto initialized successfully!" << std::endl;
    return true;
}

void cleanup_adalm_pluto(PlutoDevices& devices) {
    if (devices.txbuf) iio_buffer_destroy(devices.txbuf);
    if (devices.rxbuf) iio_buffer_destroy(devices.rxbuf);
    if (devices.rxmask) iio_channels_mask_destroy(devices.rxmask);
    if (devices.txmask) iio_channels_mask_destroy(devices.txmask);
    if (devices.ctx) iio_context_destroy(devices.ctx);
}