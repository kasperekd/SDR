# Прием QPSK-сигнала на согласованный фильтр

Данный проект реализует прием QPSK-сигнала с использованием SDR (Software Defined Radio) и библиотеки SoapySDR. Основная цель практики заключалась в сборе отсчетов для последующей "оффлайн" обработки. В процессе работы были исправлены ошибки в коде, улучшена обработка сигналов и добавлены новые функции для повышения качества приема.

## Цели практики

- Собрать отсчеты для "оффлайн" обработки.
- Исправить ошибки в коде, связанные с буферами данных.
- Улучшить процесс обработки сигналов с помощью алгоритма "slow attack".

## Выполнение

### Исправления и улучшения

1. **Выделение памяти для буфера RX**: 
   - В процессе работы была выделена память для буфера RX, что позволило избежать ошибок при обработке получаемых данных.

2. **Исправление ошибок в коде**:
   - В коде была выявлена ошибка в части, отвечающей за обработку буферов: переменная `buffs` была заменена на `buffers`, что устранило проблему с обработкой данных.

3. **Добавление функции "slow attack"**:
   - Эта функция улучшила процесс обработки сигналов, снизив скорость атаки в алгоритме фильтрации. Это изменение позволило более эффективно обрабатывать сигналы и снизить искажения, что улучшило качество передачи и приема данных.

### Графики

- **Чистый QPSK сигнал**: График, показывающий форму чистого QPSK сигнала.
- **QPSK + код Баркера**: График, показывающий QPSK сигнал с добавленным кодом Баркера.

### Результаты

В ходе работы были сняты отсчеты для "оффлайн" обработки. Получено 4 файла отсчетов QPSK + код Баркера и 4 файла чистого QPSK сигнала.

## Структура кода

Код состоит из нескольких ключевых частей:

1. **Импорт библиотек**: Используются библиотеки SoapySDR для работы с SDR, а также стандартные библиотеки C для работы с памятью и файлами.

2. **Функция `read_qpsk_signal`**: Читает QPSK сигнал из файла и выделяет память для буферов I и Q.

3. **Основная функция `main`**:
   - Создает устройство SDR и настраивает его параметры (частота, скорость выборки и т.д.).
   - Настраивает потоки для передачи и приема данных.
   - Запускает процесс приема и передачи сигналов, записывая полученные данные в файл.

4. **Обработка данных**: В процессе работы происходит чтение данных из потока, их обработка и запись в файл.

## Использование

### Компиляция

Для компиляции программы необходимо установить библиотеку SoapySDR и использовать компилятор gcc. Пример команды для компиляции:

```bash
gcc -o qpsk_receiver qpsk_receiver.c -lSoapySDR
```

### Запуск

Для запуска программы необходимо подключить устройство SDR к компьютеру и убедиться, что оно доступно. Затем выполните:

```bash
./qpsk_receiver
```

### Входные данные

Программа ожидает файл `qpsk_signal.bin`, который должен содержать QPSK сигнал в формате, поддерживаемом функцией `read_qpsk_signal`.

### Выходные данные

Результаты работы программы будут записаны в файлы `txdata_bark.pcm`, содержащие отсчеты QPSK сигнала.

## Заключение

Данный проект демонстрирует основные принципы приема и обработки QPSK сигналов с использованием SDR.