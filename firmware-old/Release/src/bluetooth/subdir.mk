################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/bluetooth/bluetooth.c 

OBJS += \
./src/bluetooth/bluetooth.o 

C_DEPS += \
./src/bluetooth/bluetooth.d 


# Each subdirectory must supply rules for building sources it contributes
src/bluetooth/%.o: ../src/bluetooth/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DNO_BOARD_LIB -DNDEBUG -D__CODE_RED -DCORE_M3 -D__USE_LPCOPEN -D__LPC17XX__ -I"/home/michi/projects/LPCXpresso/lpc_chip_175x_6x/inc" -I"/home/michi/projects/SkyNet-Dongle/firmware/src/usb" -I"/home/michi/projects/LPCXpresso/lpc_chip_175x_6x/inc/usbd" -Os -g -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


