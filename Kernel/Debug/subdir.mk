################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../actualizarConfig.c \
../api.c \
../funcionesKernel.c \
../kernel.c \
../variablesGlobales.c 

OBJS += \
./actualizarConfig.o \
./api.o \
./funcionesKernel.o \
./kernel.o \
./variablesGlobales.o 

C_DEPS += \
./actualizarConfig.d \
./api.d \
./funcionesKernel.d \
./kernel.d \
./variablesGlobales.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


