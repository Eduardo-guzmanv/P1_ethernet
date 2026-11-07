################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lwip/src/core/altcp.c \
../lwip/src/core/altcp_alloc.c \
../lwip/src/core/altcp_tcp.c \
../lwip/src/core/def.c \
../lwip/src/core/dns.c \
../lwip/src/core/inet_chksum.c \
../lwip/src/core/init.c \
../lwip/src/core/ip.c \
../lwip/src/core/mem.c \
../lwip/src/core/memp.c \
../lwip/src/core/netif.c \
../lwip/src/core/pbuf.c \
../lwip/src/core/raw.c \
../lwip/src/core/stats.c \
../lwip/src/core/sys.c \
../lwip/src/core/tcp.c \
../lwip/src/core/tcp_in.c \
../lwip/src/core/tcp_out.c \
../lwip/src/core/timeouts.c \
../lwip/src/core/udp.c 

C_DEPS += \
./lwip/src/core/altcp.d \
./lwip/src/core/altcp_alloc.d \
./lwip/src/core/altcp_tcp.d \
./lwip/src/core/def.d \
./lwip/src/core/dns.d \
./lwip/src/core/inet_chksum.d \
./lwip/src/core/init.d \
./lwip/src/core/ip.d \
./lwip/src/core/mem.d \
./lwip/src/core/memp.d \
./lwip/src/core/netif.d \
./lwip/src/core/pbuf.d \
./lwip/src/core/raw.d \
./lwip/src/core/stats.d \
./lwip/src/core/sys.d \
./lwip/src/core/tcp.d \
./lwip/src/core/tcp_in.d \
./lwip/src/core/tcp_out.d \
./lwip/src/core/timeouts.d \
./lwip/src/core/udp.d 

OBJS += \
./lwip/src/core/altcp.o \
./lwip/src/core/altcp_alloc.o \
./lwip/src/core/altcp_tcp.o \
./lwip/src/core/def.o \
./lwip/src/core/dns.o \
./lwip/src/core/inet_chksum.o \
./lwip/src/core/init.o \
./lwip/src/core/ip.o \
./lwip/src/core/mem.o \
./lwip/src/core/memp.o \
./lwip/src/core/netif.o \
./lwip/src/core/pbuf.o \
./lwip/src/core/raw.o \
./lwip/src/core/stats.o \
./lwip/src/core/sys.o \
./lwip/src/core/tcp.o \
./lwip/src/core/tcp_in.o \
./lwip/src/core/tcp_out.o \
./lwip/src/core/timeouts.o \
./lwip/src/core/udp.o 


# Each subdirectory must supply rules for building sources it contributes
lwip/src/core/%.o: ../lwip/src/core/%.c lwip/src/core/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DCPU_MCXN947VDF -DCPU_MCXN947VDF_cm33 -DCPU_MCXN947VDF_cm33_core0 -DPRINTF_ADVANCED_ENABLE=1 -DMCUXPRESSO_SDK -DLWIP_DISABLE_PBUF_POOL_SIZE_SANITY_CHECKS=1 -DCHECKSUM_GEN_UDP=1 -DCHECKSUM_GEN_TCP=1 -DCHECKSUM_GEN_ICMP=1 -DCHECKSUM_GEN_ICMP6=1 -DCHECKSUM_CHECK_IP=1 -DCHECKSUM_CHECK_UDP=1 -DCHECKSUM_CHECK_TCP=1 -DCHECKSUM_CHECK_ICMP=1 -DCHECKSUM_CHECK_ICMP6=1 -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\eduar\OneDrive\Documentos\ITESO\7mo_semestre\redes\P1_ethernet\frdmmcxn947_lwip_ping_bm\source" -I"C:\Users\eduar\OneDrive\Documentos\ITESO\7mo_semestre\redes\P1_ethernet\frdmmcxn947_lwip_ping_bm\phy" -I"C:\Users\eduar\OneDrive\Documentos\ITESO\7mo_semestre\redes\P1_ethernet\frdmmcxn947_lwip_ping_bm\utilities" -I"C:\Users\eduar\OneDrive\Documentos\ITESO\7mo_semestre\redes\P1_ethernet\frdmmcxn947_lwip_ping_bm\drivers\flash" -I"C:\Users\eduar\OneDrive\Documentos\ITESO\7mo_semestre\redes\P1_ethernet\frdmmcxn947_lwip_ping_bm\component\silicon_id" -I"C:\Users\eduar\OneDrive\Documentos\ITESO\7mo_semestre\redes\P1_ethernet\frdmmcxn947_lwip_ping_bm\lwip\port" -I"C:\Users\eduar\OneDrive\Documentos\ITESO\7mo_semestre\redes\P1_ethernet\frdmmcxn947_lwip_ping_bm\lwip\src" -I"C:\Users\eduar\OneDrive\Documentos\ITESO\7mo_semestre\redes\P1_ethernet\frdmmcxn947_lwip_ping_bm\lwip\src\include" -I"C:\Users\eduar\OneDrive\Documentos\ITESO\7mo_semestre\redes\P1_ethernet\frdmmcxn947_lwip_ping_bm\drivers" -I"C:\Users\eduar\OneDrive\Documentos\ITESO\7mo_semestre\redes\P1_ethernet\frdmmcxn947_lwip_ping_bm\lwip\contrib\apps\ping" -I"C:\Users\eduar\OneDrive\Documentos\ITESO\7mo_semestre\redes\P1_ethernet\frdmmcxn947_lwip_ping_bm\device" -I"C:\Users\eduar\OneDrive\Documentos\ITESO\7mo_semestre\redes\P1_ethernet\frdmmcxn947_lwip_ping_bm\startup" -I"C:\Users\eduar\OneDrive\Documentos\ITESO\7mo_semestre\redes\P1_ethernet\frdmmcxn947_lwip_ping_bm\component\uart" -I"C:\Users\eduar\OneDrive\Documentos\ITESO\7mo_semestre\redes\P1_ethernet\frdmmcxn947_lwip_ping_bm\component\lists" -I"C:\Users\eduar\OneDrive\Documentos\ITESO\7mo_semestre\redes\P1_ethernet\frdmmcxn947_lwip_ping_bm\component\phy" -I"C:\Users\eduar\OneDrive\Documentos\ITESO\7mo_semestre\redes\P1_ethernet\frdmmcxn947_lwip_ping_bm\lwip\port\sys_arch\dynamic" -I"C:\Users\eduar\OneDrive\Documentos\ITESO\7mo_semestre\redes\P1_ethernet\frdmmcxn947_lwip_ping_bm\component\gpio" -I"C:\Users\eduar\OneDrive\Documentos\ITESO\7mo_semestre\redes\P1_ethernet\frdmmcxn947_lwip_ping_bm\CMSIS" -I"C:\Users\eduar\OneDrive\Documentos\ITESO\7mo_semestre\redes\P1_ethernet\frdmmcxn947_lwip_ping_bm\board" -O0 -fno-common -g3 -gdwarf-4 -mcpu=cortex-m33 -c -ffunction-sections -fdata-sections -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-lwip-2f-src-2f-core

clean-lwip-2f-src-2f-core:
	-$(RM) ./lwip/src/core/altcp.d ./lwip/src/core/altcp.o ./lwip/src/core/altcp_alloc.d ./lwip/src/core/altcp_alloc.o ./lwip/src/core/altcp_tcp.d ./lwip/src/core/altcp_tcp.o ./lwip/src/core/def.d ./lwip/src/core/def.o ./lwip/src/core/dns.d ./lwip/src/core/dns.o ./lwip/src/core/inet_chksum.d ./lwip/src/core/inet_chksum.o ./lwip/src/core/init.d ./lwip/src/core/init.o ./lwip/src/core/ip.d ./lwip/src/core/ip.o ./lwip/src/core/mem.d ./lwip/src/core/mem.o ./lwip/src/core/memp.d ./lwip/src/core/memp.o ./lwip/src/core/netif.d ./lwip/src/core/netif.o ./lwip/src/core/pbuf.d ./lwip/src/core/pbuf.o ./lwip/src/core/raw.d ./lwip/src/core/raw.o ./lwip/src/core/stats.d ./lwip/src/core/stats.o ./lwip/src/core/sys.d ./lwip/src/core/sys.o ./lwip/src/core/tcp.d ./lwip/src/core/tcp.o ./lwip/src/core/tcp_in.d ./lwip/src/core/tcp_in.o ./lwip/src/core/tcp_out.d ./lwip/src/core/tcp_out.o ./lwip/src/core/timeouts.d ./lwip/src/core/timeouts.o ./lwip/src/core/udp.d ./lwip/src/core/udp.o

.PHONY: clean-lwip-2f-src-2f-core

