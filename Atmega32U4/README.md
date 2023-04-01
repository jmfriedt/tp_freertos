To add support for the Atmega32U4 in your FreeRTOS version, copy the content of this directory
to  ``$(FREERTOS_PATH)/portable/GCC/Atmega32U4`` assuming the official source tree of FreeRTOS
is located in ``FREERTOS_PATH``. The ``Makefile.avr32u4`` in ``1basic`` assumes the files are
organized accordingly, otherwise change the paths in its ``SRCS`` variable to match your setup.
