################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CMD.c \
../Colas_Operaciones.c \
../IO.c \
../PCP.c \
../PCP_Operaciones_CPU.c \
../PLP.c \
../PLP_Operaciones_Programa.c \
../PLP_Operaciones_UMV.c \
../main.c 

OBJS += \
./CMD.o \
./Colas_Operaciones.o \
./IO.o \
./PCP.o \
./PCP_Operaciones_CPU.o \
./PLP.o \
./PLP_Operaciones_Programa.o \
./PLP_Operaciones_UMV.o \
./main.o 

C_DEPS += \
./CMD.d \
./Colas_Operaciones.d \
./IO.d \
./PCP.d \
./PCP_Operaciones_CPU.d \
./PLP.d \
./PLP_Operaciones_Programa.d \
./PLP_Operaciones_UMV.d \
./main.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/git/tp-2014-1c-david-y-goliat/MiniGral_lib/Sockets" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


