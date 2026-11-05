################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/fsl_clock.c \
../drivers/fsl_common.c \
../drivers/fsl_common_arm.c \
../drivers/fsl_enet.c \
../drivers/fsl_gpio.c \
../drivers/fsl_lpflexcomm.c \
../drivers/fsl_lpuart.c \
../drivers/fsl_power.c \
../drivers/fsl_reset.c \
../drivers/fsl_spc.c 

C_DEPS += \
./drivers/fsl_clock.d \
./drivers/fsl_common.d \
./drivers/fsl_common_arm.d \
./drivers/fsl_enet.d \
./drivers/fsl_gpio.d \
./drivers/fsl_lpflexcomm.d \
./drivers/fsl_lpuart.d \
./drivers/fsl_power.d \
./drivers/fsl_reset.d \
./drivers/fsl_spc.d 

OBJS += \
./drivers/fsl_clock.o \
./drivers/fsl_common.o \
./drivers/fsl_common_arm.o \
./drivers/fsl_enet.o \
./drivers/fsl_gpio.o \
./drivers/fsl_lpflexcomm.o \
./drivers/fsl_lpuart.o \
./drivers/fsl_power.o \
./drivers/fsl_reset.o \
./drivers/fsl_spc.o 


# Each subdirectory must supply rules for building sources it contributes
drivers/%.o: ../drivers/%.c drivers/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DCPU_MCXN947VDF -DCPU_MCXN947VDF_cm33 -DCPU_MCXN947VDF_cm33_core0 -DPRINTF_ADVANCED_ENABLE=1 -DMCUXPRESSO_SDK -DLWIP_DISABLE_PBUF_POOL_SIZE_SANITY_CHECKS=1 -DCHECKSUM_GEN_UDP=1 -DCHECKSUM_GEN_TCP=1 -DCHECKSUM_GEN_ICMP=1 -DCHECKSUM_GEN_ICMP6=1 -DCHECKSUM_CHECK_IP=1 -DCHECKSUM_CHECK_UDP=1 -DCHECKSUM_CHECK_TCP=1 -DCHECKSUM_CHECK_ICMP=1 -DCHECKSUM_CHECK_ICMP6=1 -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\source" -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\phy" -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\utilities" -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\drivers\flash" -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\component\silicon_id" -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\lwip\port" -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\lwip\src" -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\lwip\src\include" -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\drivers" -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\lwip\contrib\apps\ping" -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\device" -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\startup" -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\component\uart" -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\component\lists" -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\component\phy" -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\lwip\port\sys_arch\dynamic" -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\component\gpio" -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\CMSIS" -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\board" -I"C:\Users\garci\OneDrive - ITESO\Documentos\ITESO\Redes\Practica1_Ethernet\frdmmcxn947_lwip_ping_bm\lwip\template" -O0 -fno-common -g3 -gdwarf-4 -mcpu=cortex-m33 -c -ffunction-sections -fdata-sections -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-drivers

clean-drivers:
	-$(RM) ./drivers/fsl_clock.d ./drivers/fsl_clock.o ./drivers/fsl_common.d ./drivers/fsl_common.o ./drivers/fsl_common_arm.d ./drivers/fsl_common_arm.o ./drivers/fsl_enet.d ./drivers/fsl_enet.o ./drivers/fsl_gpio.d ./drivers/fsl_gpio.o ./drivers/fsl_lpflexcomm.d ./drivers/fsl_lpflexcomm.o ./drivers/fsl_lpuart.d ./drivers/fsl_lpuart.o ./drivers/fsl_power.d ./drivers/fsl_power.o ./drivers/fsl_reset.d ./drivers/fsl_reset.o ./drivers/fsl_spc.d ./drivers/fsl_spc.o

.PHONY: clean-drivers

