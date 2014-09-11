################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: UMV-Process

dependents:
	-cd /root/git/data/MiniGral_lib/Debug && $(MAKE) all

# Tool invocations
UMV-Process: $(OBJS) $(USER_OBJS) /root/git/data/MiniGral_lib/Debug/libMiniGral_lib.so
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/root/git/data/MiniGral_lib/Debug" -o "UMV-Process" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) UMV-Process
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
/root/git/data/MiniGral_lib/Debug/libMiniGral_lib.so:

-include ../makefile.targets
