################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Primitivas.c \
../main.c \
../socket_Kernel-CPU.c \
../socket_UMV-CPU.c 

OBJS += \
./Primitivas.o \
./main.o \
./socket_Kernel-CPU.o \
./socket_UMV-CPU.o 

C_DEPS += \
./Primitivas.d \
./main.d \
./socket_Kernel-CPU.d \
./socket_UMV-CPU.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/root/git/data/MiniGral_lib" -I/usr/include -I/usr/local/include -I/usr/lib/gcc/x86_64-linux-gnu/4.7/include -I/usr/lib/gcc/x86_64-linux-gnu/4.7/include-fixed -I/usr/include/x86_64-linux-gnu -I"/root/git/data/MiniGral_lib/sockets" -I"/root/git/data/MiniGral_lib/Debug/sockets" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


