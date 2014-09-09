################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../algoritmosDeUbicacion.c \
../cmd.c \
../cpuHandler.c \
../estructurasDeSegmentos.c \
../main.c \
../socket_CPU-UMV.c \
../socket_Kernel-UMV.c 

OBJS += \
./algoritmosDeUbicacion.o \
./cmd.o \
./cpuHandler.o \
./estructurasDeSegmentos.o \
./main.o \
./socket_CPU-UMV.o \
./socket_Kernel-UMV.o 

C_DEPS += \
./algoritmosDeUbicacion.d \
./cmd.d \
./cpuHandler.d \
./estructurasDeSegmentos.d \
./main.d \
./socket_CPU-UMV.d \
./socket_Kernel-UMV.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/git/tp-2014-1c-david-y-goliat/MiniGral_lib/Sockets" -I"/home/utnso/git/tp-2014-1c-david-y-goliat/UMV" -I"/home/utnso/git/tp-2014-1c-the-os-crowd/Sockets" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


