################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
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
all: RDB1768cmsis2_usb_bootloader.axf

# Tool invocations
RDB1768cmsis2_usb_bootloader.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"/mnt/media/Dropbox/Studium/2014 SS/01 Bachelorarbeit/workspace/CMSISv2p00_LPC17xx/Debug" -L"/mnt/media/Dropbox/Studium/2014 SS/01 Bachelorarbeit/workspace/SkyNet-Dongle/RDB1768cmsis2_usbstack/Debug" -Xlinker --gc-sections -Xlinker -Map=RDB1768cmsis2_usb_bootloader.map -mcpu=cortex-m3 -mthumb -T "../linkscripts/linkscript.ld" -o"RDB1768cmsis2_usb_bootloader.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) RDB1768cmsis2_usb_bootloader.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size RDB1768cmsis2_usb_bootloader.axf; arm-none-eabi-objcopy -O binary RDB1768cmsis2_usb_bootloader.axf RDB1768cmsis2_usb_bootloader.bin ; checksum -p LPC1769 -d RDB1768cmsis2_usb_bootloader.bin;
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
