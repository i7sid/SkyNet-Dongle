################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/basestation/wind/windspeed/windspeed.c 

OBJS += \
./src/basestation/wind/windspeed/windspeed.o 

C_DEPS += \
./src/basestation/wind/windspeed/windspeed.d 


# Each subdirectory must supply rules for building sources it contributes
src/basestation/wind/windspeed/%.o: ../src/basestation/wind/windspeed/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -DNO_BOARD_LIB -DSEMIHOSTING_CONSOLE -DDEBUG -D__CODE_RED -DCORE_M3 -D__USE_LPCOPEN -D__LPC17XX__ -D__NEWLIB__ -I"/home/michi/projects/LPCXpresso/lpc_chip_175x_6x/inc" -I"/home/michi/projects/LPCXpresso/lpc_chip_175x_6x/inc/usbd" -I"/home/michi/projects/SkyNet-Dongle/firmware/src/usb" -Og -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -D__NEWLIB__ -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


