
TARGET = h747disco_tut
DEBUG ?= 1
OPT = -Og
# Build path
BUILD_DIR = .build

######################################
# source
######################################


# C sources
C_SOURCES += Application/main.c
C_SOURCES += Common/system_stm32h7xx_dualcore_boot_cm4_cm7.c
C_SOURCES += Drivers/GPIO/GPIO_h7.c
C_SOURCES += Drivers/usart_dlt/usart1.c
C_SOURCES += Application/syscalls.c
C_SOURCES += Application/sysmem.c

# ASM sources
ASM_SOURCES += Startup/_startup_CM7_stm32h747XIH6.s



PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m7
# fpu
FPU = -mfpu=fpv5-d16
# float-abi
FLOAT-ABI = -mfloat-abi=hard
# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)
# macros for gcc
# AS defines
AS_DEFS = 
# C defines
C_DEFS += -DSTM32H747xx
C_DEFS += -DCORE_CM7
# AS includes
AS_INCLUDES = 
# C includes
C_INCLUDES += -IDrivers/CMSIS/Device/ST/STM32H7xx/Include
C_INCLUDES += -IDrivers/CMSIS/Include
C_INCLUDES += -IDrivers/GPIO
C_INCLUDES += -IDrivers/usart_dlt

# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS += $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif


# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = Config/_linker_CM7_STM32F747XIH6_FLASH.ld

# libraries
LIBS = -lc -lm -lnosys 
LIBDIR = 
LDFLAGS = $(MCU) -static -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections


all: flash $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin
	@echo -----------------------------------------------------
	@echo If You Changed HTML you should update  file "fsdata.c"!
	@echo To do it you can use call the target: make html_update
	@echo -----------------------------------------------------

# flash: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin
flash: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin
	openocd \
	-f interface/stlink.cfg -f target/stm32h7x.cfg \
	-c "init" -c "reset halt" \
	-c "flash write_image erase $(BUILD_DIR)/$(TARGET).bin 0x8000000" \
	-c "reset" -c "shutdown"

#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@
	@echo ---------
#	@echo CC $<

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	@echo linking...
	@echo ----------------------
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir $@		


clean:
	-rm -fR $(BUILD_DIR)
  

#\
#-c "verify_image $(BUILD_DIR)/h747disco_tut.bin" -c "reset" -c "shutdown"

-include $(wildcard $(BUILD_DIR)/*.d)
