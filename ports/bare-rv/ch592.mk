# CH592-specific build configuration

# Define toolchain.
CROSS_COMPILE ?= riscv-none-elf-

# RISC-V architecture flags for CH592 (RV32IMAC).
CFLAGS += -march=rv32imac_zicsr -mabi=ilp32

# Use picolibc if available.
PICOLIBC_SPECS = $(shell $(CC) --print-file-name=picolibc.specs)
ifneq ($(PICOLIBC_SPECS),picolibc.specs)
CFLAGS += --specs=$(PICOLIBC_SPECS)
endif
