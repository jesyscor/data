################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../main.c 

OBJS += \
./main.o 

C_DEPS += \
./main.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/usr/include -I/usr/local/include -I/usr/lib/gcc/i686-linux-gnu/4.7/include -I/usr/lib/gcc/i686-linux-gnu/4.7/include-fixed -I/usr/lib/gcc/i686-linux-gnu -I"/home/utnso/git/data/MiniGral_lib" -I"/home/utnso/git/data/MiniGral_lib/sockets" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


