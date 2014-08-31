################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lib/pantryc-environment.c \
../lib/pantryc-milter.c \
../lib/pantryc-scanner.c \
../lib/pantryc-sqlite.c \
../lib/pantryc-util.c \
../lib/pantryc.c 

OBJS += \
./lib/pantryc-environment.o \
./lib/pantryc-milter.o \
./lib/pantryc-scanner.o \
./lib/pantryc-sqlite.o \
./lib/pantryc-util.o \
./lib/pantryc.o 

C_DEPS += \
./lib/pantryc-environment.d \
./lib/pantryc-milter.d \
./lib/pantryc-scanner.d \
./lib/pantryc-sqlite.d \
./lib/pantryc-util.d \
./lib/pantryc.d 


# Each subdirectory must supply rules for building sources it contributes
lib/%.o: ../lib/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/usr/include/gmime-2.6 -I/usr/include/glib-2.0 -I/usr/lib/i386-linux-gnu/glib-2.0/include -I"/home/forlax/Workspace/pantryc/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


