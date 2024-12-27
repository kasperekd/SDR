#include <SoapySDR/Device.h>
#include <SoapySDR/Formats.h>
#include <stdio.h> //printf
#include <stdlib.h> //free
#include <stdint.h>
#include <complex.h>

bool read_qpsk_signal(const char *filename, int16_t **tx_i, int16_t **tx_q,
                      size_t *num_samples) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Ошибка открытия файла");
        return false;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size % (2 * sizeof(int16_t)) != 0) {
        fprintf(stderr,
                "Ошибка: размер файла не кратен размеру комплексного числа\n");
        fclose(file);
        return false;
    }

    *num_samples = file_size / (2 * sizeof(int16_t));

    int16_t *combined = (int16_t *)malloc(file_size);
    if (combined == NULL) {
        fprintf(stderr, "Ошибка выделения памяти\n");
        fclose(file);
        return false;
    }

    size_t read_count =
        fread(combined, sizeof(int16_t), *num_samples * 2, file);
    if (read_count != *num_samples * 2) {
        fprintf(stderr, "Ошибка чтения данных из файла\n");
        free(combined);
        fclose(file);
        return false;
    }

    fclose(file);

    *tx_i = (int16_t *)malloc(*num_samples * sizeof(int16_t));
    *tx_q = (int16_t *)malloc(*num_samples * sizeof(int16_t));
    if (*tx_i == NULL || *tx_q == NULL) {
        fprintf(stderr, "Ошибка выделения памяти\n");
        free(combined);
        return false;
    }
    for (size_t i = 0; i < *num_samples; i++) {
        (*tx_i)[i] = 0;
        (*tx_q)[i] = 0;
    }

    for (size_t i = 0; i < *num_samples; i++) {
        (*tx_i)[i] = combined[2 * i];
        (*tx_q)[i] = combined[2 * i + 1];
    }

    free(combined);
    // for (size_t i = 0; i < *num_samples; i++) {
    //     printf("Sample %zu: %d + %di\n", i, (*tx_i)[i], (*tx_q)[i]);
    // }
    return true;
}

int main(void)
{
    // Создание сущности устройства
    // args can be user defined or from the enumeration result
    SoapySDRKwargs args = {};
    SoapySDRKwargs_set(&args, "driver", "plutosdr");
    if (1) {
        SoapySDRKwargs_set(&args, "uri", "usb:");
    } else {
        SoapySDRKwargs_set(&args, "uri", "ip:192.168.2.1");
    }
    SoapySDRKwargs_set(&args, "direct", "1");
    SoapySDRKwargs_set(&args, "timestamp_every", "1920");
    SoapySDRKwargs_set(&args, "loopback", "0");
    SoapySDRKwargs_set(&args, "gain_control_mode", "slow_attack");

    SoapySDRDevice *sdr = SoapySDRDevice_make(&args);
    SoapySDRKwargs_clear(&args);

    if (sdr == NULL)
    {
        printf("SoapySDRDevice_make fail: %s\n", SoapySDRDevice_lastError());
        return EXIT_FAILURE;
    }

    // Настраиваем устройства RX\TX
    if (SoapySDRDevice_setSampleRate(sdr, SOAPY_SDR_RX, 0, 1e6) != 0)
    {
        printf("setSampleRate rx fail: %s\n", SoapySDRDevice_lastError());
    }
    if (SoapySDRDevice_setFrequency(sdr, SOAPY_SDR_RX, 0, 800e6, NULL) != 0)
    {
        printf("setFrequency rx fail: %s\n", SoapySDRDevice_lastError());
    }
    if (SoapySDRDevice_setSampleRate(sdr, SOAPY_SDR_TX, 0, 1e6) != 0)
    {
        printf("setSampleRate tx fail: %s\n", SoapySDRDevice_lastError());
    }
    if (SoapySDRDevice_setFrequency(sdr, SOAPY_SDR_TX, 0, 800e6, NULL) != 0)
    {
        printf("setFrequency tx fail: %s\n", SoapySDRDevice_lastError());
    }
    printf("SoapySDRDevice_getFrequency tx: %lf\n", SoapySDRDevice_getFrequency(sdr, SOAPY_SDR_TX, 0));
    
    // Настройка каналов и стримов
    size_t channels[] = {0}; // {0} or {0, 1} 
    // TODO: - с вариантом {0, 1} необходимо разробраться, не работают 2 канала

    size_t channel_count = sizeof(channels) / sizeof(channels[0]);
    SoapySDRStream *rxStream = SoapySDRDevice_setupStream(sdr, SOAPY_SDR_RX, SOAPY_SDR_CS16, channels, channel_count, NULL);
    if (rxStream == NULL)
    {
        printf("setupStream rx fail: %s\n", SoapySDRDevice_lastError());
        SoapySDRDevice_unmake(sdr);
        return EXIT_FAILURE;
    }
    // if(SoapySDRDevice_setGain(sdr, SOAPY_SDR_RX, channels, 10.0) !=0 ){
    //     printf("setGain rx fail: %s\n", SoapySDRDevice_lastError());
    // }
    // if(SoapySDRDevice_setGain(sdr, SOAPY_SDR_TX, channels, -50.0) !=0 ){
    //     printf("setGain rx fail: %s\n", SoapySDRDevice_lastError());
    // }
    SoapySDRStream *txStream = SoapySDRDevice_setupStream(sdr, SOAPY_SDR_TX, SOAPY_SDR_CS16, channels, channel_count, NULL);
    if (txStream == NULL)
    {
        printf("setupStream tx fail: %s\n", SoapySDRDevice_lastError());
        SoapySDRDevice_unmake(sdr);
        return EXIT_FAILURE;
    }

    // Получение MTU (Maximum Transmission Unit), в нашем случае - размер буферов. 
    size_t rx_mtu = SoapySDRDevice_getStreamMTU(sdr, rxStream);
    size_t tx_mtu = SoapySDRDevice_getStreamMTU(sdr, txStream);
    printf("MTU - TX: %lu, RX: %lu\n", tx_mtu, rx_mtu);
    

    int16_t tx_buff[2*tx_mtu];
    int16_t *rx_buffer = (int16_t *)malloc(2 * rx_mtu * sizeof(int16_t));
    
    int16_t *tx_i = NULL;
    int16_t *tx_q = NULL;
    size_t num_samples = 0;

    if (!read_qpsk_signal("./qpsk_signal.bin", &tx_i, &tx_q, &num_samples)) {
        printf("failed to read!");
        return 1;
    }

    //заполнение tx_buff значениями сэмплов первые 16 бит - I, вторые 16 бит - Q.
    for (int i = 0; i < 2 * rx_mtu; i+=2)
    {
        if (i >= num_samples){
            tx_buff[i] = 0;
            tx_buff[i+1] = 0;
        }else{
            tx_buff[i] = tx_i[i] * (int16_t)15000;
            tx_buff[i+1] = tx_q[i] * (int16_t)15000;
        }
    }

    //prepare fixed bytes in transmit buffer
    //we transmit a pattern of FFFF FFFF [TS_0]00 [TS_1]00 [TS_2]00 [TS_3]00 [TS_4]00 [TS_5]00 [TS_6]00 [TS_7]00 FFFF FFFF
    //that is a flag (FFFF FFFF) followed by the 64 bit timestamp, split into 8 bytes and packed into the lsb of each of the DAC words.
    //DAC samples are left aligned 12-bits, so each byte is left shifted into place
    for(size_t i = 0; i < 2; i++)
    {
        tx_buff[0 + i] = 0xffff;
        // 8 x timestamp words
        tx_buff[10 + i] = 0xffff;
    }

    //activate streams
    SoapySDRDevice_activateStream(sdr, rxStream, 0, 0, 0); //start streaming
    SoapySDRDevice_activateStream(sdr, txStream, 0, 0, 0); //start streaming

    //here goes
    printf("Start test...\n");
    const long  timeoutUs = 400000; // arbitrarily chosen (взяли из srsRAN)

    // При первом запуске отчищаем rx_buff от возможного мусора.
    for (size_t buffers_read = 0; buffers_read < 128; /* in loop */)
    {
        void *buffs[] = {rx_buffer}; //void *buffs[] = {rx_buff[0][0], rx_buff[0][1]}; //array of buffers
        int flags; //flags set by receive operation
        long long timeNs; //timestamp for receive buffer
        // Read samples
        int sr = SoapySDRDevice_readStream(sdr, rxStream, buffs, rx_mtu, &flags, &timeNs, timeoutUs); // 100ms timeout
        if (sr < 0)
        {
            // Skip read on error (likely timeout)
            continue;
        }
        // Increment number of buffers read
        buffers_read++;
    }

    long long last_time = 0;

    // Пример по записи сэмплов в фай. Не использовать в Release, операция не быстрая
    #if 0
    FILE *file = fopen("txdata.pcm", "a+");
    fwrite(txp[0], proc->writeBlockSize * sizeof(int), 1, file);
    fclose(file);
    #endif

    // Создаем файл для записи сэмплов с rx_buff
    FILE *file = fopen("txdata_bark.pcm", "w");
    
    // Количество итерация
    size_t iteration_count = 100;
    // Начинается работа с получением и отправкой сэмплов
    for (size_t buffers_read = 0; buffers_read < iteration_count; buffers_read++)
    {
        void *rx_buffs[] = {rx_buffer};
        int flags;        // flags set by receive operation
        long long timeNs; //timestamp for receive buffer

        int sr = SoapySDRDevice_readStream(sdr, rxStream, rx_buffs, rx_mtu, &flags, &timeNs, timeoutUs);
        if (sr < 0)
        {
            // Skip read on error (likely timeout)
            continue;
        }

        // Dump info
        printf("Buffer: %lu - Samples: %i, Flags: %i, Time: %lli, TimeDiff: %lli\n", buffers_read, sr, flags, timeNs, timeNs - last_time);
        fwrite(rx_buffer, 2* rx_mtu * sizeof(int16_t), 1, file);
        last_time = timeNs;

        // Calculate transmit time 4ms in future
        long long tx_time = timeNs + (4 * 1000 * 1000);

        // Set samples
        for(size_t i = 0; i < 8; i++)
        {
            // Extract byte from tx time
            uint8_t tx_time_byte = (tx_time >> (i * 8)) & 0xff;

            // Add byte to buffer
            tx_buff[2 + i] = tx_time_byte << 4;
        }

        // Send buffer
        if( buffers_read == 10 ){
            void *tx_buffs[] = {tx_buff};
                flags = SOAPY_SDR_HAS_TIME;
                int st = SoapySDRDevice_writeStream(sdr, txStream, (const void * const*)tx_buffs, tx_mtu, &flags, tx_time, 400000);
                if ((size_t)st != tx_mtu)
                {
                    printf("TX Failed: %i\n", st);
            
            }
        }

        
    }
    fclose(file);

    //stop streaming
    SoapySDRDevice_deactivateStream(sdr, rxStream, 0, 0);
    SoapySDRDevice_deactivateStream(sdr, txStream, 0, 0);

    //shutdown the stream
    SoapySDRDevice_closeStream(sdr, rxStream);
    SoapySDRDevice_closeStream(sdr, txStream);

    //cleanup device handle
    SoapySDRDevice_unmake(sdr);

    // Process each rx buffer, looking for transmitted timestamp
    for (size_t j = 0; j < channel_count; j++)
    {
        printf("Checking channel %zu\n", j);
        //check_channel(sample_count, channel_count, j, tx_timestamps, rx_timestamps, (uint16_t**)rx_buff, rx_mtu);
    }

    //all done
    printf("test complete!\n");

    // //free buffers
    // for (size_t i = 0; i < sample_count; i++)
    // for (size_t j = 0; j < channel_count; j++)
    // {
    //     //free(rx_buff);
    //     //rx_buff[i][j] = NULL;
    // }

    return EXIT_SUCCESS;
}
