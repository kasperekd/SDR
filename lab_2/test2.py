import numpy as np
import adi
import matplotlib.pyplot as plt

sdr = adi.Pluto('ip:192.168.3.1')

sdr.sample_rate = int(2.5e6)
sdr.rx_lo = int(800e6)
sdr.tx_lo = int(800e6)

# sdr.sample_rate = int(2.6e6)
# sdr.rx_lo = int(2e9)
# sdr.tx_lo = int(2e9)

sdr.tx_hardwaregain_chan0 = 0


rectangle = np.empty(1024, dtype=complex)
for i in range(1024):
    if i < 300 or i > 700:
        rectangle[i] = complex(0) 
    else:
        rectangle[i] = complex(400)

plt.figure(figsize=(12, 6))
plt.subplot(2, 1, 1)
plt.plot(rectangle.real, label='Real Part')
plt.plot(rectangle.imag, label='Imaginary Part', linestyle='--')
plt.ylabel('Amplitude')
plt.title('Rectangle')
plt.legend()

big_array = np.empty((200, 1024), dtype=complex)
for i in range(200):
    sdr.tx(rectangle)
    big_array[i] = sdr.rx()

plt.subplot(2, 1, 2)
plt.plot(big_array.real.flatten(), label='Real Part')
# plt.plot(big_array.imag.flatten(), label='Imaginary Part', linestyle='--')
plt.ylabel('Amplitude')
plt.title('Received')
plt.legend()

plt.tight_layout()
plt.show()
# plt.plot(big_array)
