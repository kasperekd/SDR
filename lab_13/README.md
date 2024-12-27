# Adalm Pluto SDR & Timestamp

## Описание

Adalm Pluto SDR представляет собой мощное устройство, состоящее из двух основных компонентов: аналогового радиочастотного приемопередатчика AD9363 и системы на кристалле (SoC) Xilinx Zynq 7000 Series. Это устройство позволяет осуществлять передачу и прием IQ-сэмплов, а также настраивать различные параметры радиосвязи, такие как несущая частота и частота дискретизации.

### Архитектура

AD9363 включает в себя необходимые усилители, фильтры, а также цифро-аналоговые и аналого-цифровые преобразователи (12-bit ADC и DAC). Система на кристалле оснащена процессором ARM Cortex A9, работающим на частоте 667 МГц, и программируемой вентильной матрицей (FPGA), что позволяет реализовывать пользовательские аппаратные модули.

### Характеристики (Analog Devices)

- Диапазон частот: от 325 МГц до 3.8 ГГц
- Мгновенная полоса пропускания: до 20 МГц
- Гибкая частота, 12-битный ADC и DAC
- Один передатчик и один приемник, полудуплексный или полный дуплекс
- Поддержка MATLAB®, Simulink®
- Поддержка блоков sink и source для GNU Radio
- API libiio для C, C++, C# и Python

Передача данных (IQ-сэмплов) между Adalm Pluto и хост-компьютером осуществляется через USB 2.0. Важно отметить, что для SDR необходимо обеспечивать непрерывную передачу данных в обе стороны одновременно. Хотя теоретическая пропускная способность USB 2.0 составляет 480 Mb/s, работа в полудуплексном режиме значительно снижает фактическую пропускную способность. Рекомендуемая частота дискретизации составляет около 6 Msps.

## Timestamp и SoapySDR

В рамках проекта srsran/zynq_timestamping были добавлены функции получения временных меток (timestamp) с FPGA Xilinx Zynq. Эти временные метки привязаны к каждому запросу данных из буфера FPGA, что позволяет синхронно передавать и получать данные в потоках RX и TX. Проблемы с пропускной способностью USB 2.0 могут привести к увеличению частоты дискретизации, что затрудняет полноценную передачу и прием сэмплов.

### Обновление прошивки для получения временных меток

1. Скачайте образ прошивки: [прошивка с поддержкой временных меток](https://github.com/pgreenland/plutosdr-fw/releases/tag/v0.37_with_timestamping).

### Установка необходимых библиотек

1. Удалите старую версию libiio, если она установлена:
   ```bash
   sudo rm /usr/lib/x86_64-linux-gnu/libiio.so.1
   sudo rm /usr/lib/x86_64-linux-gnu/libiio.so.1.0
   ```

2. Установите SoapySDR:
   ```bash
   sudo apt-get install cmake g++ libpython3-dev python3-numpy swig
   git clone --branch soapy-sdr-0.8.1 https://github.com/pothosware/SoapySDR.git
   cd SoapySDR
   mkdir build && cd build
   cmake ../
   make -j`nproc`
   sudo make install
   sudo ldconfig
   ```

3. Установите libiio:
   ```bash
   sudo apt-get install libxml2 libxml2-dev bison flex libcdk5-dev cmake
   sudo apt-get install libusb-1.0-0-dev libaio-dev
   git clone --branch v0.24 https://github.com/analogdevicesinc/libiio.git
   cd libiio
   mkdir build && cd build
   cmake ../
   make -j`nproc`
   sudo make install
   ```

4. Установите LibAD9361:
   ```bash
   git clone --branch v0.3 https://github.com/analogdevicesinc/libad9361-iio.git
   cd libad9361-iio
   mkdir build && cd build
   cmake ../
   make -j`nproc`
   sudo make install
   sudo ldconfig
   ```

5. Установите SoapyPlutoSDR:
   ```bash
   git clone --branch sdr_gadget_timestamping https://github.com/pgreenland/SoapyPlutoSDR.git
   cd SoapyPlutoSDR
   mkdir build && cd build
   cmake ../
   make -j`nproc`
   sudo make install
   sudo ldconfig
   ```

### Работа напрямую с USB

Для корректной работы с устройством Adalm Pluto SDR необходимо настроить правила udev:

```bash
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="0456", ATTR{idProduct}=="b673", MODE="666"' | sudo tee /etc/udev/rules.d/90-libiio_pluto.rules
sudo udevadm control --reload-rules && sudo udevadm trigger
```

### Пример работы с временными метками

Для тестирования работы с временными метками можно использовать следующий пример кода:

1. Клонируйте репозиторий с примерами:
   ```bash
   git clone https://github.com/fzybot/sdrLessons.git
   cd sdrLessons
   mkdir build && cd build
   cmake ../
   make -j8
   ```

2. Перейдите в директорию с тестами и запустите бинарный файл:
   ```bash
   cd tests/soapy_pluto
   ./soapy_pluto_sdr_timestamp
   ```

Этот пример демонстрирует, как использовать временные метки для синхронизации передачи и приема данных через Adalm Pluto SDR.

## Заключение

Проект Adalm Pluto SDR с поддержкой временных меток предоставляет мощные возможности для работы с радиосигналами. Использование временных меток позволяет улучшить синхронизацию данных и решить проблемы, связанные с пропускной способностью USB 2.0. Установка необходимых библиотек и настройка системы обеспечивают эффективную работу с SDR-устройством, что открывает новые горизонты для исследований и разработок в области радиосвязи.