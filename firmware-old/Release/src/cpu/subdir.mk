################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/cpu/cpu.c \
../src/cpu/rtc.c \
../src/cpu/systick.c 

OBJS += \
./src/cpu/cpu.o \
./src/cpu/rtc.o \
./src/cpu/systick.o 

C_DEPS += \
./src/cpu/cpu.d \
./src/cpu/rtc.d \
./src/cpu/systick.d 


# Each subdirectory must supply rules for building sources it contributes
src/cpu/%.o: ../src/cpu/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DNO_BOARD_LIB -DNDEBUG -D__CODE_RED -DCORE_M3 -D__USE_LPCOPEN -D__LPC17XX__ -I"/home/michi/projects/LPCXpresso/lpc_chip_175x_6x/inc" -I"/home/michi/projects/LPCXpresso/lpc_chip_175x_6x/inc/usbd" -I"/home/michi/projects/SkyNet-Dongle/firmware/src/usb" -Os -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


