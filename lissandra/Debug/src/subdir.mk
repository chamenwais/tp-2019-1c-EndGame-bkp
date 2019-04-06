################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/fs.c \
../src/lissandra.c \
../src/variablesGlobales.c\
../src/api.c

OBJS += \
./src/fs.o \
./src/lissandra.o \
./src/variablesGlobales.o\
./src/api.o

C_DEPS += \
./src/fs.d \
./src/lissandra.d \
./src/variablesGlobales.d\
./src/api.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


