# Target name.
TGT_SPEC ?= ch592

# Include the core environment definitions; this will set $(TOP).
include ../../py/mkenv.mk

# Include py core make definitions.
include $(TOP)/py/py.mk

# Set makefile-level MicroPython feature configurations.
MICROPY_ROM_TEXT_COMPRESSION ?= 0
MICROPY_ENABLE_COMPILER ?= 1

# Include target-specific makefile.
include $(TGT_SPEC).mk

# Set CFLAGS.
CFLAGS += -I. -I$(TOP) -I$(BUILD)
CFLAGS += -Wall -Werror -std=c99 -nostdlib
CFLAGS += -DMICROPY_ENABLE_COMPILER=$(MICROPY_ENABLE_COMPILER)
CFLAGS += $(CFLAGS_EXTRA)
CSUPEROPT = -Os # save some code space for performance-critical code

# Select debugging or optimisation build.
ifeq ($(DEBUG), 1)
CFLAGS += -Og
else
CFLAGS += -Os -DNDEBUG
CFLAGS += -fdata-sections -ffunction-sections
endif

# Set linker flags (use CC for linking to support LTO).
LDFLAGS += -nostdlib -T $(TGT_SPEC).ld -Wl,--gc-sections
LIBS += -lgcc

# Define the required source files.
SRC_C += lib.c main.c $(TGT_SPEC)_system.c

# Define the required object files.
OBJ += $(PY_CORE_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))

# Define the top-level target, the main firmware.
all: $(BUILD)/firmware.bin

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(ECHO) "Create $@"
	$(Q)$(OBJCOPY) -O binary $^ $@

# Include remaining core make rules.
include $(TOP)/py/mkrules.mk
