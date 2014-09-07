################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/usbcontrol.c \
../src/usbhw_lpc.c \
../src/usbinit.c \
../src/usbstdreq.c 

OBJS += \
./src/usbcontrol.o \
./src/usbhw_lpc.o \
./src/usbinit.o \
./src/usbstdreq.o 

C_DEPS += \
./src/usbcontrol.d \
./src/usbhw_lpc.d \
./src/usbinit.d \
./src/usbstdreq.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__USE_CMSIS=CMSISv2p00_LPC17xx -D__CODE_RED -D__REDLIB__ -I"/mnt/media/Dropbox/Studium/2014 SS/01 Bachelorarbeit/workspace/CMSISv2p00_LPC17xx/inc" -I"/mnt/media/Dropbox/Studium/2014 SS/01 Bachelorarbeit/workspace/SkyNet-Dongle/RDB1768cmsis2_usbstack/inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -mcpu=cortex-m3 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


