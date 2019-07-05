# STM32 DMA Examples

This repository contains some simple examples of how to use the DMA peripheral on STM32 chips. It includes examples for three 'types' of DMA peripheral:

* 'Type 1': F0, F1, F3, L0, L1, L4
* 'Type 2': F2, F4, F7
* 'Type 3': G0, G4, L4+

Currently, the 'Type 1' and 'Type 2' directories only contain one example each, which sends a sine wave to the DAC peripheral. The 'Type 3' directory also contains a few examples which send colors or framebuffers to LEDs/displays over I2C/SPI peripherals.
