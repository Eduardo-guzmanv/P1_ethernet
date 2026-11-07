#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/udp.h"
#include "lwip/timeouts.h"
#include "lwip/init.h"
#include "netif/ethernet.h"
#include "ethernetif.h"
#include "pin_mux.h"
#include "board.h"
#include "fsl_phy.h"
#include "fsl_enet.h"
#include "fsl_phylan8741.h"
#include "fsl_port.h"
#include "fsl_lpflexcomm.h"
#include "mpu6050.h"
#include "sismo_detector.h"

#define ID_ESTACION 2U
#define INTENSIDAD_BOTON 3U
#define configMAC_ADDR {0x54, 0x27, 0x8D, 0x00, 0x00, (0x10 + ID_ESTACION)}

#ifndef configIP_ADDR0
#define configIP_ADDR0 192
#endif
#ifndef configIP_ADDR1
#define configIP_ADDR1 168
#endif
#ifndef configIP_ADDR2
#define configIP_ADDR2 0
#endif
#ifndef configIP_ADDR3
#define configIP_ADDR3 (110 + ID_ESTACION)
#endif

#ifndef configNET_MASK0
#define configNET_MASK0 255
#endif
#ifndef configNET_MASK1
#define configNET_MASK1 255
#endif
#ifndef configNET_MASK2
#define configNET_MASK2 255
#endif
#ifndef configNET_MASK3
#define configNET_MASK3 0
#endif

#ifndef configGW_ADDR0
#define configGW_ADDR0 192
#endif
#ifndef configGW_ADDR1
#define configGW_ADDR1 168
#endif
#ifndef configGW_ADDR2
#define configGW_ADDR2 0
#endif
#ifndef configGW_ADDR3
#define configGW_ADDR3 100
#endif

extern phy_lan8741_resource_t g_phy_resource;
#define EXAMPLE_ENET_BASE    ENET0
#define EXAMPLE_PHY_ADDRESS  BOARD_ENET0_PHY_ADDRESS
#define EXAMPLE_PHY_OPS      &phylan8741_ops
#define EXAMPLE_PHY_RESOURCE &g_phy_resource
#define EXAMPLE_CLOCK_FREQ   (50000000U)

#ifndef configMAC_ADDR
#include "fsl_silicon_id.h"
#endif

#ifndef EXAMPLE_NETIF_INIT_FN
#define EXAMPLE_NETIF_INIT_FN ethernetif0_init
#endif

#define PUERTO_UDP              5000U
#define TIEMPO_REGISTRO_MS      1000U
#define TIEMPO_LATIDO_MS        2000U
#define TIEMPO_REINTENTO_MS     500U
#define TIEMPO_LEER_BOTON_MS    20U
#define TIEMPO_LED_ENCENDIDO_MS 5000U
#define MAXIMO_REINTENTOS       3U

#define BOTON_PRESIONADO 0U
#define BOTON_SUELTO     1U

#define MPU_I2C_BASE     LPI2C2
#define MPU_I2C_INSTANCE 2U
#define MPU_I2C_CLK_HZ   12000000U

typedef enum
{
    MENSAJE_REGISTRO     = 1,
    MENSAJE_LATIDO       = 2,
    MENSAJE_ALERTA       = 3,
    MENSAJE_CONFIRMACION = 4,
    MENSAJE_AVISO        = 5
} tipo_mensaje_t;

typedef struct
{
    uint8_t tipo;
    uint8_t id_estacion;
    uint16_t secuencia;
    uint8_t intensidad;
    uint8_t es_simulacro;
} paquete_sismo_t;

typedef enum
{
    ESTACION_SIN_REGISTRO,
    ESTACION_REGISTRADA,
    ESTACION_ESPERANDO_CONFIRMACION
} estado_estacion_t;

void enviar_alerta(uint8_t intensidad);

phy_lan8741_resource_t g_phy_resource;

static phy_handle_t phyHandle;
static struct udp_pcb *socket_cliente;
static ip_addr_t ip_servidor;

static estado_estacion_t estado_estacion = ESTACION_SIN_REGISTRO;
static uint16_t secuencia_actual         = 1U;

static uint16_t secuencia_alerta  = 0U;
static uint8_t intensidad_alerta  = 0U;
static uint8_t reintentos_alerta  = 0U;

static uint32_t ultimo_registro_ms     = 0U;
static uint32_t ultimo_latido_ms       = 0U;
static uint32_t ultimo_envio_alerta_ms = 0U;
static uint32_t ultima_lectura_boton_ms = 0U;
static uint32_t led_encendido_ms       = 0U;

static uint8_t estado_anterior_boton = BOTON_SUELTO;
static bool led_encendido            = false;

static void MDIO_Init(void)
{
    (void)CLOCK_EnableClock(s_enetClock[ENET_GetInstance(EXAMPLE_ENET_BASE)]);
    ENET_SetSMI(EXAMPLE_ENET_BASE, CLOCK_GetCoreSysClkFreq());
}

static status_t MDIO_Write(uint8_t phyAddr, uint8_t regAddr, uint16_t data)
{
    return ENET_MDIOWrite(EXAMPLE_ENET_BASE, phyAddr, regAddr, data);
}

static status_t MDIO_Read(uint8_t phyAddr, uint8_t regAddr, uint16_t *pData)
{
    return ENET_MDIORead(EXAMPLE_ENET_BASE, phyAddr, regAddr, pData);
}

void SysTick_Handler(void)
{
    time_isr();
}

static void enviar_paquete(uint8_t tipo, uint16_t secuencia, uint8_t intensidad)
{
    paquete_sismo_t paquete = {0};
    struct pbuf *memoria    = pbuf_alloc(PBUF_TRANSPORT, sizeof(paquete_sismo_t), PBUF_RAM);

    if (memoria == NULL)
    {
        return;
    }

    paquete.tipo        = tipo;
    paquete.id_estacion = ID_ESTACION;
    paquete.secuencia   = lwip_htons(secuencia);
    paquete.intensidad  = intensidad;

    memcpy(memoria->payload, &paquete, sizeof(paquete_sismo_t));
    udp_sendto(socket_cliente, memoria, &ip_servidor, PUERTO_UDP);
    pbuf_free(memoria);
}

static uint16_t obtener_nueva_secuencia(void)
{
    secuencia_actual++;

    if (secuencia_actual == 0U)
    {
        secuencia_actual = 1U;
    }

    return secuencia_actual;
}

void enviar_alerta(uint8_t intensidad)
{
    if (estado_estacion == ESTACION_SIN_REGISTRO)
    {
        PRINTF("Alerta no enviada\r\n");
        return;
    }

    if (estado_estacion == ESTACION_ESPERANDO_CONFIRMACION)
    {
        PRINTF("Alerta no enviada\r\n");
        return;
    }

    secuencia_alerta       = obtener_nueva_secuencia();
    intensidad_alerta      = intensidad;
    reintentos_alerta      = 0U;
    ultimo_envio_alerta_ms = sys_now();
    estado_estacion        = ESTACION_ESPERANDO_CONFIRMACION;

    enviar_paquete(MENSAJE_ALERTA, secuencia_alerta, intensidad_alerta);
    PRINTF("Alerta enviada: nivel %u, secuencia %u\r\n", intensidad_alerta, secuencia_alerta);
}

static void procesar_confirmacion(const paquete_sismo_t *paquete)
{
    if ((estado_estacion == ESTACION_SIN_REGISTRO) && (paquete->secuencia == 1U))
    {
        estado_estacion  = ESTACION_REGISTRADA;
        ultimo_latido_ms = sys_now();
        PRINTF("Estacion registrada en el servidor\r\n");
    }
    else if ((estado_estacion == ESTACION_ESPERANDO_CONFIRMACION) && (paquete->secuencia == secuencia_alerta))
    {
        estado_estacion = ESTACION_REGISTRADA;
        PRINTF("Alerta confirmada por el servidor\r\n");
    }
}

static void apagar_led(void)
{
    GPIO_PinWrite(BOARD_LED_RED_GPIO, BOARD_LED_RED_GPIO_PIN, LOGIC_LED_OFF);
    GPIO_PinWrite(BOARD_LED_GREEN_GPIO, BOARD_LED_GREEN_GPIO_PIN, LOGIC_LED_OFF);
    GPIO_PinWrite(BOARD_LED_BLUE_GPIO, BOARD_LED_BLUE_GPIO_PIN, LOGIC_LED_OFF);
}

static void encender_led(uint8_t intensidad)
{
    apagar_led();

    if (intensidad >= 3U)
    {
        GPIO_PinWrite(BOARD_LED_RED_GPIO, BOARD_LED_RED_GPIO_PIN, LOGIC_LED_ON);
    }

    if (intensidad <= 3U)
    {
        GPIO_PinWrite(BOARD_LED_GREEN_GPIO, BOARD_LED_GREEN_GPIO_PIN, LOGIC_LED_ON);
    }

    led_encendido    = true;
    led_encendido_ms = sys_now();
}

static void procesar_aviso(const paquete_sismo_t *paquete)
{
    encender_led(paquete->intensidad);

    if (paquete->es_simulacro != 0U)
    {
        PRINTF("AVISO: simulacro, nivel %u\r\n", paquete->intensidad);
    }
    else
    {
        PRINTF("AVISO: sismo nivel %u, estacion mas afectada: %u\r\n", paquete->intensidad, paquete->id_estacion);
    }
}

static void recibir_paquete(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    paquete_sismo_t paquete;

    (void)arg;
    (void)pcb;
    (void)addr;
    (void)port;

    if (p == NULL)
    {
        return;
    }

    if (p->tot_len == sizeof(paquete_sismo_t))
    {
        pbuf_copy_partial(p, &paquete, sizeof(paquete_sismo_t), 0U);
        paquete.secuencia = lwip_ntohs(paquete.secuencia);

        if ((paquete.tipo == MENSAJE_CONFIRMACION) && (paquete.id_estacion == ID_ESTACION))
        {
            procesar_confirmacion(&paquete);
        }
        else if (paquete.tipo == MENSAJE_AVISO)
        {
            procesar_aviso(&paquete);
        }
    }

    pbuf_free(p);
}

static void revisar_registro(void)
{
    uint32_t ahora_ms = sys_now();

    if ((estado_estacion == ESTACION_SIN_REGISTRO) && ((ahora_ms - ultimo_registro_ms) >= TIEMPO_REGISTRO_MS))
    {
        ultimo_registro_ms = ahora_ms;
        enviar_paquete(MENSAJE_REGISTRO, 1U, 0U);
        PRINTF("Intentando registrar la estacion %u...\r\n", ID_ESTACION);
    }
}

static void revisar_latido(void)
{
    uint32_t ahora_ms = sys_now();

    if ((estado_estacion != ESTACION_SIN_REGISTRO) && ((ahora_ms - ultimo_latido_ms) >= TIEMPO_LATIDO_MS))
    {
        ultimo_latido_ms = ahora_ms;
        enviar_paquete(MENSAJE_LATIDO, obtener_nueva_secuencia(), 0U);
    }
}

static void revisar_reintento(void)
{
    uint32_t ahora_ms = sys_now();

    if ((estado_estacion != ESTACION_ESPERANDO_CONFIRMACION) ||
        ((ahora_ms - ultimo_envio_alerta_ms) < TIEMPO_REINTENTO_MS))
    {
        return;
    }

    if (reintentos_alerta >= MAXIMO_REINTENTOS)
    {
        estado_estacion = ESTACION_REGISTRADA;
        PRINTF("Error: el servidor no confirmo la alerta\r\n");
        return;
    }

    reintentos_alerta++;
    ultimo_envio_alerta_ms = ahora_ms;
    enviar_paquete(MENSAJE_ALERTA, secuencia_alerta, intensidad_alerta);
    PRINTF("Reintento %u de la alerta\r\n", reintentos_alerta);
}

static void revisar_boton(void)
{
    uint32_t ahora_ms = sys_now();
    uint8_t estado_boton;

    if ((ahora_ms - ultima_lectura_boton_ms) < TIEMPO_LEER_BOTON_MS)
    {
        return;
    }

    ultima_lectura_boton_ms = ahora_ms;
    estado_boton            = (uint8_t)GPIO_PinRead(BOARD_SW2_GPIO, BOARD_SW2_GPIO_PIN);

    if ((estado_boton == BOTON_PRESIONADO) && (estado_anterior_boton == BOTON_SUELTO))
    {
        enviar_alerta(INTENSIDAD_BOTON);
    }

    estado_anterior_boton = estado_boton;
}

static void revisar_led(void)
{
    if (led_encendido && ((sys_now() - led_encendido_ms) >= TIEMPO_LED_ENCENDIDO_MS))
    {
        apagar_led();
        led_encendido = false;
    }
}

static void iniciar_boton_y_led(void)
{
    port_pin_config_t configuracion_boton = {0};
    port_pin_config_t configuracion_led   = {0};
    gpio_pin_config_t entrada             = {kGPIO_DigitalInput, 0U};
    gpio_pin_config_t salida              = {kGPIO_DigitalOutput, LOGIC_LED_OFF};

    CLOCK_EnableClock(kCLOCK_Port0);
    CLOCK_EnableClock(kCLOCK_Port1);
    CLOCK_EnableClock(kCLOCK_Gpio0);
    CLOCK_EnableClock(kCLOCK_Gpio1);
    RESET_ReleasePeripheralReset(kPORT0_RST_SHIFT_RSTn);
    RESET_ReleasePeripheralReset(kGPIO0_RST_SHIFT_RSTn);
    RESET_ReleasePeripheralReset(kGPIO1_RST_SHIFT_RSTn);

    configuracion_boton.pullSelect  = kPORT_PullUp;
    configuracion_boton.mux         = kPORT_MuxAlt0;
    configuracion_boton.inputBuffer = kPORT_InputBufferEnable;
    PORT_SetPinConfig(PORT0, BOARD_SW2_GPIO_PIN, &configuracion_boton);
    GPIO_PinInit(BOARD_SW2_GPIO, BOARD_SW2_GPIO_PIN, &entrada);

    configuracion_led.mux = kPORT_MuxAlt0;
    PORT_SetPinConfig(PORT0, BOARD_LED_RED_GPIO_PIN, &configuracion_led);
    PORT_SetPinConfig(PORT0, BOARD_LED_GREEN_GPIO_PIN, &configuracion_led);
    PORT_SetPinConfig(PORT1, BOARD_LED_BLUE_GPIO_PIN, &configuracion_led);

    GPIO_PinInit(BOARD_LED_RED_GPIO, BOARD_LED_RED_GPIO_PIN, &salida);
    GPIO_PinInit(BOARD_LED_GREEN_GPIO, BOARD_LED_GREEN_GPIO_PIN, &salida);
    GPIO_PinInit(BOARD_LED_BLUE_GPIO, BOARD_LED_BLUE_GPIO_PIN, &salida);
}

void iniciar_i2c_mpu(void)
{
    CLOCK_SetClkDiv(kCLOCK_DivFlexcom2Clk, 1U);
    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM2);
    LP_FLEXCOMM_Init(MPU_I2C_INSTANCE, LP_FLEXCOMM_PERIPH_LPI2C);
}

static void revisar_sismo(void)
{
    uint8_t nivel = 0U;

    if (sismo_detector_tarea(sys_now(), &nivel))
    {
        PRINTF("Sacudida detectada: pico %u mg -> nivel %u\r\n", sismo_detector_ultimo_pico_mg(), nivel);
        enviar_alerta(nivel);
    }
}

static bool iniciar_cliente(void)
{
    IP4_ADDR(&ip_servidor, 192, 168, 0, 102);

    socket_cliente = udp_new();

    if (socket_cliente == NULL)
    {
        return false;
    }

    if (udp_bind(socket_cliente, IP_ADDR_ANY, PUERTO_UDP) != ERR_OK)
    {
        return false;
    }

    udp_recv(socket_cliente, recibir_paquete, NULL);
    return true;
}

int main(void)
{
    struct netif netif;
    ip4_addr_t netif_ipaddr, netif_netmask, netif_gw;
    ethernetif_config_t enet_config = {.phyHandle   = &phyHandle,
                                       .phyAddr     = EXAMPLE_PHY_ADDRESS,
                                       .phyOps      = EXAMPLE_PHY_OPS,
                                       .phyResource = EXAMPLE_PHY_RESOURCE,
#ifdef configMAC_ADDR
                                       .macAddress = configMAC_ADDR
#endif
    };

    CLOCK_EnableClock(kCLOCK_InputMux);
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    CLOCK_AttachClk(MUX_A(CM_ENETRMIICLKSEL, 0));
    CLOCK_EnableClock(kCLOCK_Enet);
    SYSCON0->PRESETCTRL2 = SYSCON_PRESETCTRL2_ENET_RST_MASK;
    SYSCON0->PRESETCTRL2 &= ~SYSCON_PRESETCTRL2_ENET_RST_MASK;

    MDIO_Init();

    g_phy_resource.read  = MDIO_Read;
    g_phy_resource.write = MDIO_Write;

    time_init();

#ifndef configMAC_ADDR
    (void)SILICONID_ConvertToMacAddr(&enet_config.macAddress);
#endif

    enet_config.srcClockHz = EXAMPLE_CLOCK_FREQ;

    IP4_ADDR(&netif_ipaddr, configIP_ADDR0, configIP_ADDR1, configIP_ADDR2, configIP_ADDR3);
    IP4_ADDR(&netif_netmask, configNET_MASK0, configNET_MASK1, configNET_MASK2, configNET_MASK3);
    IP4_ADDR(&netif_gw, configGW_ADDR0, configGW_ADDR1, configGW_ADDR2, configGW_ADDR3);

    iniciar_i2c_mpu();

    if (!sismo_detector_init(MPU_I2C_BASE, MPU_I2C_CLK_HZ)){
        PRINTF("Aviso: el MPU6050 no responde\r\n");
    }

    lwip_init();

    netif_add(&netif, &netif_ipaddr, &netif_netmask, &netif_gw, &enet_config, EXAMPLE_NETIF_INIT_FN, ethernet_input);
    netif_set_default(&netif);
    netif_set_up(&netif);

    while (ethernetif_wait_linkup(&netif, 5000) != ERR_OK)
    {
        PRINTF("PHY Auto-negotiation failed.\r\n");
    }

    iniciar_boton_y_led();

    if (!iniciar_cliente())
    {
        PRINTF("Error: no se pudo iniciar el cliente UDP.\r\n");
        while (1)
        {
        }
    }

    PRINTF("\r\n************************************************\r\n");
    PRINTF(" PING example\r\n");
    PRINTF("************************************************\r\n");
    PRINTF(" IPv4 Address     : %u.%u.%u.%u\r\n", ((u8_t *)&netif_ipaddr)[0], ((u8_t *)&netif_ipaddr)[1],
           ((u8_t *)&netif_ipaddr)[2], ((u8_t *)&netif_ipaddr)[3]);
    PRINTF(" IPv4 Subnet mask : %u.%u.%u.%u\r\n", ((u8_t *)&netif_netmask)[0], ((u8_t *)&netif_netmask)[1],
           ((u8_t *)&netif_netmask)[2], ((u8_t *)&netif_netmask)[3]);
    PRINTF(" IPv4 Gateway     : %u.%u.%u.%u\r\n", ((u8_t *)&netif_gw)[0], ((u8_t *)&netif_gw)[1],
           ((u8_t *)&netif_gw)[2], ((u8_t *)&netif_gw)[3]);
    PRINTF("************************************************\r\n");
    PRINTF(" Estacion %u lista\r\n", ID_ESTACION);
    PRINTF("************************************************\r\n");

    while (1)
    {
        ethernetif_input(&netif);

        sys_check_timeouts();

        revisar_registro();
        revisar_latido();
        revisar_reintento();
        revisar_boton();
        revisar_led();

        revisar_sismo();
    }
}
