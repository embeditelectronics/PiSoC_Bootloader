PiSoC_Bootloader
=================

CHANGES to v1.2: Updated pinout for PiSoC Rev E.
CHANGES to v1.1: Changed intitial wait time to 100ms.

Bootloader allowing for USB programming of the PiSoC board

For detailed instructions on using and programming the PiSoC, refer to:
http://www.embeditelectronics.com/blog/project/using-psoc-creator-with-the-pisoc/

Each PiSoC_Bootloader folder contains two PSoC Creator projects, created in PSoC Creator 3.3 SP2:

1. pisoc_bootloader. This is the PiSoC's bootloader. It also contains a compiled pisoc_bootloader.hex file, which is needed to bootload the RPiSoC with new programs.

2. pisoc_bootloadable_example is an example project that makes use of the bootloader. You can use the Bootloader host tool in PSoC Creator to program the PiSoC with this example.
