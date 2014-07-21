RPiSoC_Bootloader
=================

Bootloader allowing for USB programming of the RPiSoC board

Refer to the user guide for detailed instructions on using and programming the RPiSoC.

The RPiSoC_Bootloader folder contains two PSoC Creator projects, created in PSoC Creator 3.0 SP1:

1. Bootloader_RPiSoC. This is the RPiSoC's bootloader. It also contains a compiled Bootloader_RPiSoC.hex file, which is needed to bootload the RPiSoC with new programs.

2. Bootloadable_RPiSoC_example is an example project that makes use of the bootloader. You can use the Bootloader host tool in PSoc Creator to program the RPiSoC with this example.
