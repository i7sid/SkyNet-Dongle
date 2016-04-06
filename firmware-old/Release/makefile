################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/usb/subdir.mk
-include src/spi/subdir.mk
-include src/radio/subdir.mk
-include src/periph/subdir.mk
-include src/misc/subdir.mk
-include src/mac/subdir.mk
-include src/gpio/subdir.mk
-include src/cpu/subdir.mk
-include src/basestation/wind/windvane/subdir.mk
-include src/basestation/wind/windspeed/subdir.mk
-include src/basestation/gps/subdir.mk
-include src/basestation/compass/subdir.mk
-include src/basestation/subdir.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: SkyNet-Dongle-firmware.axf

# Tool invocations
SkyNet-Dongle-firmware.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"/home/michi/projects/LPCXpresso/lpc_chip_175x_6x/Release" -L"/home/michi/projects/LPCXpresso/lpc_chip_175x_6x/libs" -Xlinker -Map="SkyNet-Dongle-firmware.map" -Xlinker --gc-sections -mcpu=cortex-m3 -mthumb -T "SkyNet-Dongle-firmware_Release.ld" -o "SkyNet-Dongle-firmware.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) SkyNet-Dongle-firmware.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "SkyNet-Dongle-firmware.axf" ; arm-none-eabi-objcopy -O binary "SkyNet-Dongle-firmware.axf" "SkyNet-Dongle-firmware.bin" ; checksum -p LPC1769 -v "SkyNet-Dongle-firmware.bin" ; arm-none-eabi-objcopy -I binary "SkyNet-Dongle-firmware.bin" -O ihex "SkyNet-Dongle-firmware.hex" ; # ; #checksum -v "SkyNet-Dongle-firmware.bin" ; # arm-none-eabi-objcopy -v -O binary "SkyNet-Dongle-firmware.axf" "SkyNet-Dongle-firmware.bin" ; # checksum -p LPC1769 -d "SkyNet-Dongle-firmware.bin"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
