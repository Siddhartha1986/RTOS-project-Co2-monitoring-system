################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/coreMQTT/mqtt/PlaintextMQTT.c \
../src/coreMQTT/mqtt/using_plaintext.c 

OBJS += \
./src/coreMQTT/mqtt/PlaintextMQTT.o \
./src/coreMQTT/mqtt/using_plaintext.o 

C_DEPS += \
./src/coreMQTT/mqtt/PlaintextMQTT.d \
./src/coreMQTT/mqtt/using_plaintext.d 


# Each subdirectory must supply rules for building sources it contributes
src/coreMQTT/mqtt/%.o: ../src/coreMQTT/mqtt/%.c src/coreMQTT/mqtt/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=c11 -DDEBUG -D__CODE_RED -D__NEWLIB__ -DCORE_M3 -D__USE_LPCOPEN -DCPP_USE_HEAP -D__LPC15XX__ -I"C:\FINALVERISONTEST\lpc_board_nxp_lpcxpresso_1549\inc" -I"C:\FINALVERISONTEST\lpc_chip_15xx\inc" -I"C:\FINALVERISONTEST\FreeRTOS\inc" -I"C:\FINALVERISONTEST\FreeRTOS\src\include" -I"C:\FINALVERISONTEST\FreeRTOS\src\portable\GCC\ARM_CM3" -I"C:\FINALVERISONTEST\esp_socket\src" -I"C:\FINALVERISONTEST\esp_socket\src\uart" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m3 -mthumb -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


