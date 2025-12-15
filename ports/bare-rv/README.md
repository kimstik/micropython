The bare RISC-V port
====================

This port is intended to be the bare-minimum amount of code and configuration
required to get MicroPython compiling and running on a bare-metal RISC-V based
target.  No external dependencies or libraries are needed for this build and
it shows exactly what hardware and system functionality MicroPython needs to
run.

To build, simply run `make` in this directory.  The output will be
`build/firmware.elf` (and also corresponding `.bin` file).

The default target is CH592 (WCH RISC-V BLE MCU).  The output is UART0
at 115200 baud.

To build for a different target, create `<target>.mk`, `<target>.ld` and
`<target>_system.c` files, then run `make TGT_SPEC=<target>`.

There are some simple demonstration code strings (see `main.c`) which are
compiled and executed when the firmware starts.  They produce output on the
system's stdout.  To build without the compiler (smaller binary), use
`make MICROPY_ENABLE_COMPILER=0`.
