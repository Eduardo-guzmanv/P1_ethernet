################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lwip/contrib/apps/ping/ping.c 

C_DEPS += \
./lwip/contrib/apps/ping/ping.d 

OBJS += \
./lwip/contrib/apps/ping/ping.o 


# Each subdirectory must supply rules for building sources it contributes
lwip/contrib/apps/ping/%.o: ../lwip/contrib/apps/ping/%.c lwip/contrib/apps/ping/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DCPU_MCXN947VDF -DCPU_MCXN947VDF_cm33 -DCPU_MCXN947VDF_cm33_core0 -DPRINTF_ADVANCED_ENABLE=1 -DMCUXPRESSO_SDK -DLWIP_DISABLE_PBUF_POOL_SIZE_SANITY_CHECKS=1 -DCHECKSUM_GEN_UDP=1 -DCHECKSUM_GEN_TCP=1 -DCHECKSUM_GEN_ICMP=1 -DCHECKSUM_GEN_ICMP6=1 -DCHECKSUM_CHECK_IP=1 -DCHECKSUM_CHECK_UDP=1 -DCHECKSUM_CHECK_TCP=1 -DCHECKSUM_CHECK_ICMP=1 -DCHECKSUM_CHECK_ICMP6=1 -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\source" -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\phy" -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\utilities" -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\drivers\flash" -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\component\silicon_id" -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\lwip\port" -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\lwip\src" -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\lwip\src\include" -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\drivers" -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\lwip\contrib\apps\ping" -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\device" -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\startup" -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\component\uart" -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\component\lists" -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\component\phy" -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\lwip\port\sys_arch\dynamic" -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\component\gpio" -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\CMSIS" -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\board" -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\lwip\template" -O0 -fno-common -g3 -gdwarf-4 -mcpu=cortex-m33 -c -ffunction-sections -fdata-sections -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-lwip-2f-contrib-2f-apps-2f-ping

clean-lwip-2f-contrib-2f-apps-2f-ping:
	-$(RM) ./lwip/contrib/apps/ping/ping.d ./lwip/contrib/apps/ping/ping.o

.PHONY: clean-lwip-2f-contrib-2f-apps-2f-ping

