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
	gcc -I"/root/git/data/MiniGral_lib" -I/usr/include -I/usr/local/include -I/usr/lib/gcc/x86_64-linux-gnu/4.7/include -I/usr/lib/gcc/x86_64-linux-gnu/4.7/include-fixed -I/usr/include/x86_64-linux-gnu -I"/root/git/data/UMV" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


