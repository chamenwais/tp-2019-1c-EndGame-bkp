################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Consola.c \
../src/Contexto.c \
../src/Memoria.c \
../src/Utilidades.c 

OBJS += \
./src/Consola.o \
./src/Contexto.o \
./src/Memoria.o \
./src/Utilidades.o 

C_DEPS += \
./src/Consola.d \
./src/Contexto.d \
./src/Memoria.d \
./src/Utilidades.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


