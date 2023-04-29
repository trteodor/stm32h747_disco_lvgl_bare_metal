
TARGET = h747disco_tut
DEBUG ?= 1
OPT = -Og
# Build path
BUILD_DIR = .build



######################################
# lvgl START
######################################
lvgl_src_ex = $(wildcard Middlewares/lvgl/examples/)
lvgl_src_ex1 = $(wildcard Middlewares/lvgl/examples/get_started/)

lvgl_src = $(wildcard Middlewares/lvgl/src/*/)
lvgl_src1 = $(wildcard Middlewares/lvgl/src/*/*/)
lvgl_src2 = $(wildcard Middlewares/lvgl/src/*/*/*)
lvgl_src3 = $(wildcard Middlewares/lvgl/src/*/*/*/*)


DIRECTORIES_LVGL += $(sort $(dir $(wildcard $(lvgl_src_ex))))
#DIRECTORIES_LVGL += $(sort $(dir $(wildcard $(lvgl_src_ex1))))
DIRECTORIES_LVGL += $(sort $(dir $(wildcard $(lvgl_src))))
DIRECTORIES_LVGL += $(sort $(dir $(wildcard $(lvgl_src1))))
DIRECTORIES_LVGL += $(sort $(dir $(wildcard $(lvgl_src2))))
DIRECTORIES_LVGL += $(sort $(dir $(wildcard $(lvgl_src3))))

DIRECTORIES_LVGL_OUT = $(sort $(dir $(wildcard $(DIRECTORIES_LVGL))))

LVGL_SRCC := $(foreach dir,$(DIRECTORIES_LVGL_OUT),$(wildcard $(dir)*.c))
LVGL_SRCC += $(foreach dir,$(lvgl_src_ex1),$(wildcard $(dir)*.c))

LVGL_INC = $(foreach dir, $(DIRECTORIES_LVGL_OUT), $(addprefix -I, $(dir)))
######################################
# lvgl END
######################################




######################################
# source
######################################




# C sources
C_SOURCES += APP_CM7/main.c
C_SOURCES += APP_CM7/lvglAppMain.c
C_SOURCES += Common/system_stm32h7xx_dualcore_boot_cm4_cm7.c
C_SOURCES += Common/syscalls.c
C_SOURCES += Common/sysmem.c
C_SOURCES += Drivers/GPIO/GPIO_h7.c
C_SOURCES += Drivers/System/System.c
C_SOURCES += Drivers/SDRAM/sd_ram.c
C_SOURCES += Drivers/SDRAM/is42s32800j/is42s32800j.c
# C_SOURCES += Drivers/usart_dlt/usart1.c
C_SOURCES += Drivers/usart_dlt/UART1_dlt.c
C_SOURCES += Drivers/Disp_OTM8009A/ltdc.c
C_SOURCES += Drivers/Disp_OTM8009A/dsihost.c
C_SOURCES += Drivers/DMA2d/dma2d.c
C_SOURCES += Drivers/Disp_OTM8009A/OTM8009A_wrapper.c
C_SOURCES += Drivers/Disp_OTM8009A/otm8009a/otm8009a.c
C_SOURCES += Drivers/Disp_OTM8009A/otm8009a/otm8009a_reg.c
C_SOURCES += Middlewares/DLTuc/src/DLTuc.c
C_SOURCES += $(LVGL_SRCC)
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
C_INCLUDES += -IAPP_CM7
C_INCLUDES += -IDrivers/CMSIS/Device/ST/STM32H7xx/Include
C_INCLUDES += -IDrivers/CMSIS/Include
C_INCLUDES += -IDrivers/GPIO
C_INCLUDES += -IDrivers/usart_dlt
C_INCLUDES += -IMiddlewares/DLTuc/src
C_INCLUDES += -IConfig
C_INCLUDES += -IDrivers/System
C_INCLUDES += -IDrivers/SDRAM
C_INCLUDES += -IDrivers/SDRAM/is42s32800j
C_INCLUDES += -IDrivers/Disp_OTM8009A
C_INCLUDES += -IDrivers/DMA2d
C_INCLUDES += -IDrivers/Disp_OTM8009A/otm8009a
C_INCLUDES += $(LVGL_INC)

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
	@$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@
	@echo CC $<

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
