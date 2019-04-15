################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Contexto.c \
../src/Memoria.c \
../src/Consola.c \
../src/Utilidades.c 

OBJS += \
./src/Contexto.o \
./src/Memoria.o \
./src/Consola.o \
./src/Utilidades.o 

C_DEPS += \
./src/Contexto.d \
./src/Memoria.d \
./src/Consola.d \
./src/Utilidades.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


