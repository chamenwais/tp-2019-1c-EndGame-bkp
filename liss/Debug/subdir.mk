################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../api.c \
../compactador.c \
../dump.c \
../fs.c \
../funcionesAuxiliares.c \
../funcionesComunes.c \
../lissandra.c \
../server.c \
../variablesGlobales.c 

OBJS += \
./api.o \
./compactador.o \
./dump.o \
./fs.o \
./funcionesAuxiliares.o \
./funcionesComunes.o \
./lissandra.o \
./server.o \
./variablesGlobales.o 

C_DEPS += \
./api.d \
./compactador.d \
./dump.d \
./fs.d \
./funcionesAuxiliares.d \
./funcionesComunes.d \
./lissandra.d \
./server.d \
./variablesGlobales.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


