cd build
make
cd .. 
./build/main -c ./config/config.cfg -b ./generate/qpsk_signal.bin -iters 100 && python3 ./receive/main.py ./adalm_rx.txt