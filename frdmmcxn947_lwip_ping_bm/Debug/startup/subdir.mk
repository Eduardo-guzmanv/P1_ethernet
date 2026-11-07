################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../startup/boot_multicore_slave.c \
../startup/startup_mcxn947_cm33_core0.c 

C_DEPS += \
./startup/boot_multicore_slave.d \
./startup/startup_mcxn947_cm33_core0.d 

OBJS += \
./startup/boot_multicore_slave.o \
./startup/startup_mcxn947_cm33_core0.o 


# Each subdirectory must supply rules for building sources it contributes
startup/%.o: ../startup/%.c startup/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DCPU_MCXN947VDF -DCPU_MCXN947VDF_cm33 -DCPU_MCXN947VDF_cm33_core0 -DPRINTF_ADVANCED_ENABLE=1 -DMCUXPRESSO_SDK -DLWIP_DISABLE_PBUF_POOL_SIZE_SANITY_CHECKS=1 -DCHECKSUM_GEN_UDP=1 -DCHECKSUM_GEN_TCP=1 -DCHECKSUM_GEN_ICMP=1 -DCHECKSUM_GEN_ICMP6=1 -DCHECKSUM_CHECK_IP=1 -DCHECKSUM_CHECK_UDP=1 -DCHECKSUM_CHECK_TCP=1 -DCHECKSUM_CHECK_ICMP=1 -DCHECKSUM_CHECK_ICMP6=1 -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\eduar\OneDrive\Documentos\ITESO\7mo_semestre\redes\P1_ethernet\frdmmcxn947_lwip_ping_bm\source" -I"C:\Users\eduar\OneDrive\Documentos\ITESO\7mo_semestre\redes\P1_ethernet\frdmmcxn947_lwip_ping_bm\phy" -I"C:\Users\eduar\OneDrive\Documentos\ITESO\7mo_semestre\redes\P1_ethernet\frdmmcxn947_lwip_ping_bm\utilities" -I"C:\Users\eduar\OneDrive\Documentos\ITESO\7mo_semestre\redes\P1_ethernet\frdmmcxn947_lwip_ping_bm\drivers\flash" -I"C:\Users\eduar\OneDrive\Documentos\ITESO\7mo_semestre\redes\P1_ethernet\frdmmcxn947_lwip_ping_bm\component\silicon_id" -I"C:\Users\eduar\OneDrive\Documentos\ITESO\7mo_semestre\redes\P1_ethernet\frdmmcxn947_lwip_ping_bm\lwip\port" -I"C:\Users\eduar\OneDrive\Documentos\ITESO\7mo_semestre\redes\P1_ethernet\frdmmcxn947_lwip_ping_bm\lwip\src" -I"C:\Users\eduar\OneDrive\Documentos\ITESO\7mo_semestre\redes\P1_ethernet\frdmmcxn947_lwip_ping_bm\lwip\src\include" -I"C:\Users\eduar\OneDrive\Documentos\ITESO\7mo_semestre\redes\P1_ethernet\frdmmcxn947_lwip_ping_bm\drivers" -I"C:\Users\eduar\OneDrive\Documentos\ITESO\7mo_semestre\redes\P1_ethernet\frdmmcxn947_lwip_ping_bm\lwip\contrib\apps\ping" -I"C:\Users\eduar\OneDrive\Documentos\ITESO\7mo_semestre\redes\P1_ethernet\frdmmcxn947_lwip_ping_bm\device" -I"C:\Users\eduar\OneDrive\Documentos\ITESO\7mo_semestre\redes\P1_ethernet\frdmmcxn947_lwip_ping_bm\startup" -I"C:\Users\eduar\OneDrive\Documentos\ITESO\7mo_semestre\redes\P1_ethernet\frdmmcxn947_lwip_ping_bm\component\uart" -I"C:\Users\eduar\OneDrive\Documentos\ITESO\7mo_semestre\redes\P1_ethernet\frdmmcxn947_lwip_ping_bm\component\lists" -I"C:\Users\eduar\OneDrive\Documentos\ITESO\7mo_semestre\redes\P1_ethernet\frdmmcxn947_lwip_ping_bm\component\phy" -I"C:\Users\eduar\OneDrive\Documentos\ITESO\7mo_semestre\redes\P1_ethernet\frdmmcxn947_lwip_ping_bm\lwip\port\sys_arch\dynamic" -I"C:\Users\eduar\OneDrive\Documentos\ITESO\7mo_semestre\redes\P1_ethernet\frdmmcxn947_lwip_ping_bm\component\gpio" -I"C:\Users\eduar\OneDrive\Documentos\ITESO\7mo_semestre\redes\P1_ethernet\frdmmcxn947_lwip_ping_bm\CMSIS" -I"C:\Users\eduar\OneDrive\Documentos\ITESO\7mo_semestre\redes\P1_ethernet\frdmmcxn947_lwip_ping_bm\board" -O0 -fno-common -g3 -gdwarf-4 -mcpu=cortex-m33 -c -ffunction-sections -fdata-sections -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-startup

clean-startup:
	-$(RM) ./startup/boot_multicore_slave.d ./startup/boot_multicore_slave.o ./startup/startup_mcxn947_cm33_core0.d ./startup/startup_mcxn947_cm33_core0.o

.PHONY: clean-startup

