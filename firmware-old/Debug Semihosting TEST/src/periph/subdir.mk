################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/periph/adc.c \
../src/periph/charger.c \
../src/periph/dcdc.c \
../src/periph/input.c \
../src/periph/led.c 

OBJS += \
./src/periph/adc.o \
./src/periph/charger.o \
./src/periph/dcdc.o \
./src/periph/input.o \
./src/periph/led.o 

C_DEPS += \
./src/periph/adc.d \
./src/periph/charger.d \
./src/periph/dcdc.d \
./src/periph/input.d \
./src/periph/led.d 


# Each subdirectory must supply rules for building sources it contributes
src/periph/%.o: ../src/periph/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -DNO_BOARD_LIB -DSEMIHOSTING_CONSOLE -DDEBUG -D__CODE_RED -DCORE_M3 -D__USE_LPCOPEN -D__LPC17XX__ -D__NEWLIB__ -I"/home/michi/projects/LPCXpresso/lpc_chip_175x_6x/inc" -I"/home/michi/projects/LPCXpresso/lpc_chip_175x_6x/inc/usbd" -I"/home/michi/projects/SkyNet-Dongle/firmware/src/usb" -Og -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -D__NEWLIB__ -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


