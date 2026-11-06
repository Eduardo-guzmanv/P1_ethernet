/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2020, 2022, 2024 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "lwip/opt.h"

#if LWIP_IPV4 && LWIP_RAW

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
#include "fsl_lpuart.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* @TEST_ANCHOR */

/* IP address configuration. */
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
#define configIP_ADDR3 102
#endif

/* Netmask configuration. */
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

/* Gateway address configuration. */
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

/* Ethernet configuration. */
extern phy_lan8741_resource_t g_phy_resource;
#define EXAMPLE_ENET_BASE    ENET0
#define EXAMPLE_PHY_ADDRESS  BOARD_ENET0_PHY_ADDRESS
#define EXAMPLE_PHY_OPS      &phylan8741_ops
#define EXAMPLE_PHY_RESOURCE &g_phy_resource
#define EXAMPLE_CLOCK_FREQ   (50000000U)

/* Must be after include of app.h */
#ifndef configMAC_ADDR
#include "fsl_silicon_id.h"
#endif

#ifndef EXAMPLE_NETIF_INIT_FN
/*! @brief Network interface initialization function. */
#define EXAMPLE_NETIF_INIT_FN ethernetif0_init
#endif /* EXAMPLE_NETIF_INIT_FN */

/* Configuracion del servidor */
#define PUERTO_UDP                    5000U
#define TOTAL_ESTACIONES              3U
#define TOTAL_HISTORIAL               5U
#define INTENSIDAD_MINIMA             1U
#define INTENSIDAD_MAXIMA             5U
#define INTENSIDAD_SIMULACRO          3U
#define TIEMPO_SIN_MENSAJES_MS        6000U /* Equivale a perder 3 latidos */
#define TIEMPO_ESPERA_ALERTAS_MS      2000U
#define TIEMPO_ACTUALIZAR_PANTALLA_MS 1000U

/* Codigos ANSI para mover el cursor y poner color en la terminal */
#define PANTALLA_BORRAR  "\033[2J"
#define PANTALLA_INICIO  "\033[H"
#define PANTALLA_FINAL   "\033[J"
#define BORRAR_LINEA     "\033[K"
#define COLOR_ROJO       "\033[31m"
#define COLOR_VERDE      "\033[32m"
#define COLOR_AMARILLO   "\033[33m"
#define COLOR_NORMAL     "\033[0m"

/* Tipos de mensaje del protocolo */
typedef enum
{
    MENSAJE_REGISTRO     = 1, /* Estacion servidor: la estacion acaba de encender */
    MENSAJE_LATIDO       = 2, /* Estacion servidor: la estacion sigue conectada */
    MENSAJE_ALERTA       = 3, /* Estacion servidor: se detecto una sacudida */
    MENSAJE_CONFIRMACION = 4, /* Servidor estacion: mensaje recibido */
    MENSAJE_AVISO        = 5  /* Servidor estaciones: aviso general de sismo */
} tipo_mensaje_t;

/* Paquete que viaja por la red*/
typedef struct
{
    uint8_t tipo;
    uint8_t id_estacion;
    uint16_t secuencia;
    uint8_t intensidad;
    uint8_t es_simulacro;
} paquete_sismo_t;

/* Datos que el servidor guarda de cada estacion */
typedef struct
{
    const char *zona;
    bool ya_se_conecto;
    bool conectada;
    ip_addr_t ip;
    uint16_t puerto;
    uint32_t ultimo_mensaje_ms;
    uint16_t ultima_secuencia_alerta;
    uint8_t intensidad;
    uint32_t total_alertas;
    uint32_t total_duplicados;
} estacion_t;

/* Resumen de un sismo o simulacro */
typedef struct
{
    uint8_t id_estacion; /* Estacion mas afectada, 0 en un simulacro */
    uint8_t intensidad;
    uint8_t estaciones_afectadas;
    uint8_t estaciones_avisadas;
    bool es_simulacro;
    uint32_t momento_ms;
} evento_t;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
phy_lan8741_resource_t g_phy_resource;

static phy_handle_t phyHandle;

/* Socket UDP del servidor */
static struct udp_pcb *socket_servidor;

/* Cada estacion se guarda en la posicion (id_estacion - 1) */
static estacion_t estaciones[TOTAL_ESTACIONES] = {{.zona = "Norte"}, {.zona = "Centro"}, {.zona = "Sur"}};

/* Historial de eventos */
static evento_t historial[TOTAL_HISTORIAL];
static uint32_t total_eventos = 0U;

/* Control del evento que se esta formando */
static bool evento_abierto      = false;
static uint32_t inicio_evento_ms = 0U;

/* Contadores y control de la pantalla */
static uint16_t secuencia_avisos       = 0U;
static uint32_t paquetes_ignorados     = 0U;
static bool mostrar_historial          = true;
static bool pantalla_pendiente         = true;
static uint32_t ultima_actualizacion_ms = 0U;

/*******************************************************************************
 * Code
 ******************************************************************************/
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


/*!
 * @brief Interrupt service for SysTick timer.
 */
void SysTick_Handler(void)
{
    time_isr();
}

/* Envia un paquete a una IP y puerto. La secuencia se envia en orden de red. */
static void enviar_paquete(const paquete_sismo_t *paquete, const ip_addr_t *ip, uint16_t puerto)
{
    paquete_sismo_t paquete_red = *paquete;
    struct pbuf *memoria        = pbuf_alloc(PBUF_TRANSPORT, sizeof(paquete_sismo_t), PBUF_RAM);

    if (memoria == NULL)
    {
        return;
    }

    paquete_red.secuencia = lwip_htons(paquete->secuencia);
    memcpy(memoria->payload, &paquete_red, sizeof(paquete_sismo_t));
    udp_sendto(socket_servidor, memoria, ip, puerto);
    pbuf_free(memoria);
}

/* Confirma a una estacion que su mensaje llego, repitiendo su secuencia */
static void enviar_confirmacion(const paquete_sismo_t *paquete_recibido, const ip_addr_t *ip, uint16_t puerto)
{
    paquete_sismo_t confirmacion = {0};

    confirmacion.tipo        = MENSAJE_CONFIRMACION;
    confirmacion.id_estacion = paquete_recibido->id_estacion;
    confirmacion.secuencia   = paquete_recibido->secuencia;

    enviar_paquete(&confirmacion, ip, puerto);
}

/* Avisa a las estaciones conectadas y regresa cuantas fueron avisadas */
static uint8_t enviar_aviso(uint8_t id_estacion, uint8_t intensidad, bool es_simulacro)
{
    paquete_sismo_t aviso = {0};
    uint8_t avisadas      = 0U;

    aviso.tipo         = MENSAJE_AVISO;
    aviso.id_estacion  = id_estacion;
    aviso.secuencia    = ++secuencia_avisos;
    aviso.intensidad   = intensidad;
    aviso.es_simulacro = es_simulacro ? 1U : 0U;

    for (uint32_t i = 0U; i < TOTAL_ESTACIONES; i++)
    {
        if (estaciones[i].conectada)
        {
            enviar_paquete(&aviso, &estaciones[i].ip, estaciones[i].puerto);
            avisadas++;
        }
    }

    return avisadas;
}

/* Guarda un evento en el historial. Al llenarse, reemplaza al mas antiguo. */
static void guardar_evento(const evento_t *evento)
{
    historial[total_eventos % TOTAL_HISTORIAL] = *evento;
    total_eventos++;
}

/* Inicia un evento nuevo y borra las intensidades del evento anterior */
static void abrir_evento(void)
{
    for (uint32_t i = 0U; i < TOTAL_ESTACIONES; i++)
    {
        estaciones[i].intensidad = 0U;
    }

    evento_abierto   = true;
    inicio_evento_ms = sys_now();
}

/* Elige la zona mas afectada, guarda el evento y avisa a las estaciones */
static void cerrar_evento(void)
{
    evento_t evento = {0};

    for (uint32_t i = 0U; i < TOTAL_ESTACIONES; i++)
    {
        if (estaciones[i].intensidad > 0U)
        {
            evento.estaciones_afectadas++;
        }

        /* En caso de empate se queda la estacion con el numero mas bajo */
        if (estaciones[i].intensidad > evento.intensidad)
        {
            evento.intensidad  = estaciones[i].intensidad;
            evento.id_estacion = (uint8_t)(i + 1U);
        }
    }

    evento.momento_ms          = sys_now();
    evento.estaciones_avisadas = enviar_aviso(evento.id_estacion, evento.intensidad, false);

    guardar_evento(&evento);
    evento_abierto     = false;
    pantalla_pendiente = true;
}

/* Revisa una alerta. Una alerta repetida se confirma, pero no se cuenta otra vez. */
static void procesar_alerta(estacion_t *estacion, const paquete_sismo_t *paquete)
{
    if ((paquete->intensidad < INTENSIDAD_MINIMA) || (paquete->intensidad > INTENSIDAD_MAXIMA))
    {
        paquetes_ignorados++;
        return;
    }

    /* Se confirma siempre, porque la confirmacion anterior pudo haberse perdido */
    enviar_confirmacion(paquete, &estacion->ip, estacion->puerto);

    if (paquete->secuencia == estacion->ultima_secuencia_alerta)
    {
        estacion->total_duplicados++;
        return;
    }

    estacion->ultima_secuencia_alerta = paquete->secuencia;
    estacion->total_alertas++;

    if (!evento_abierto)
    {
        abrir_evento();
    }

    if (paquete->intensidad > estacion->intensidad)
    {
        estacion->intensidad = paquete->intensidad;
    }
}

/* Revisa un paquete valido y actualiza los datos de la estacion que lo envio */
static void procesar_paquete(const paquete_sismo_t *paquete, const ip_addr_t *ip, uint16_t puerto)
{
    estacion_t *estacion;

    if ((paquete->id_estacion < 1U) || (paquete->id_estacion > TOTAL_ESTACIONES) ||
        (paquete->tipo < MENSAJE_REGISTRO) || (paquete->tipo > MENSAJE_ALERTA))
    {
        paquetes_ignorados++;
        return;
    }

    estacion = &estaciones[paquete->id_estacion - 1U];

    /* Se guarda de donde vino el mensaje para poder contestarle a la estacion */
    ip_addr_copy(estacion->ip, *ip);
    estacion->puerto            = puerto;
    estacion->ultimo_mensaje_ms = sys_now();
    estacion->ya_se_conecto     = true;
    estacion->conectada         = true;

    switch (paquete->tipo)
    {
        case MENSAJE_REGISTRO:
            /* La estacion reinicio, asi que sus secuencias empiezan de nuevo */
            estacion->ultima_secuencia_alerta = 0U;
            enviar_confirmacion(paquete, ip, puerto);
            break;

        case MENSAJE_ALERTA:
            procesar_alerta(estacion, paquete);
            break;

        default:
            /* Un latido solo actualiza ultimo_mensaje_ms */
            break;
    }

    pantalla_pendiente = true;
}

/* Funcion que lwIP llama cada vez que llega un paquete al puerto del servidor */
static void recibir_paquete(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    paquete_sismo_t paquete;

    (void)arg;
    (void)pcb;

    if (p == NULL)
    {
        return;
    }

    if (p->tot_len == sizeof(paquete_sismo_t))
    {
        pbuf_copy_partial(p, &paquete, sizeof(paquete_sismo_t), 0U);
        paquete.secuencia = lwip_ntohs(paquete.secuencia);
        procesar_paquete(&paquete, addr, port);
    }
    else
    {
        paquetes_ignorados++;
    }

    pbuf_free(p); /* Liberar memoria */
}

/* Marca como desconectadas las estaciones que dejaron de enviar mensajes */
static void revisar_estaciones(void)
{
    uint32_t ahora_ms = sys_now();

    for (uint32_t i = 0U; i < TOTAL_ESTACIONES; i++)
    {
        if (estaciones[i].conectada && ((ahora_ms - estaciones[i].ultimo_mensaje_ms) >= TIEMPO_SIN_MENSAJES_MS))
        {
            estaciones[i].conectada = false;
            pantalla_pendiente      = true;
        }
    }
}

/* Cierra el evento cuando termina el tiempo de espera de alertas */
static void revisar_evento(void)
{
    if (evento_abierto && ((sys_now() - inicio_evento_ms) >= TIEMPO_ESPERA_ALERTAS_MS))
    {
        cerrar_evento();
    }
}

/* Envia un aviso de prueba a todas las estaciones conectadas */
static void iniciar_simulacro(void)
{
    evento_t evento = {0};

    evento.intensidad          = INTENSIDAD_SIMULACRO;
    evento.es_simulacro        = true;
    evento.momento_ms          = sys_now();
    evento.estaciones_avisadas = enviar_aviso(0U, INTENSIDAD_SIMULACRO, true);

    guardar_evento(&evento);
    pantalla_pendiente = true;
}

/* Borra alertas, contadores e historial. Las conexiones se conservan. */
static void reiniciar_datos(void)
{
    for (uint32_t i = 0U; i < TOTAL_ESTACIONES; i++)
    {
        estaciones[i].intensidad       = 0U;
        estaciones[i].total_alertas    = 0U;
        estaciones[i].total_duplicados = 0U;
    }

    total_eventos      = 0U;
    paquetes_ignorados = 0U;
    evento_abierto     = false;
    pantalla_pendiente = true;
}

/* Revisa si se presiono una tecla, sin detener el programa */
static void revisar_teclado(void)
{
    LPUART_Type *uart = (LPUART_Type *)BOARD_DEBUG_UART_BASEADDR;
    uint32_t banderas = LPUART_GetStatusFlags(uart);

    /* Si llegaron teclas de mas, se limpia el error para seguir recibiendo */
    if ((banderas & (uint32_t)kLPUART_RxOverrunFlag) != 0U)
    {
        (void)LPUART_ClearStatusFlags(uart, (uint32_t)kLPUART_RxOverrunFlag);
    }

    if ((banderas & (uint32_t)kLPUART_RxDataRegFullFlag) == 0U)
    {
        return;
    }

    switch (LPUART_ReadByte(uart))
    {
        case 's':
        case 'S':
            iniciar_simulacro();
            break;

        case 'r':
        case 'R':
            reiniciar_datos();
            break;

        case 'l':
        case 'L':
            mostrar_historial  = !mostrar_historial;
            pantalla_pendiente = true;
            break;

        default:
            break;
    }
}

/* Convierte la intensidad en texto */
static const char *obtener_clasificacion(uint8_t intensidad)
{
    if (intensidad >= 4U)
    {
        return "SEVERO";
    }

    if (intensidad == 3U)
    {
        return "MODERADO";
    }

    return "LEVE";
}

/* Regresa el color que corresponde a la intensidad */
static const char *obtener_color(uint8_t intensidad)
{
    if (intensidad >= 4U)
    {
        return COLOR_ROJO;
    }

    if (intensidad == 3U)
    {
        return COLOR_AMARILLO;
    }

    return COLOR_VERDE;
}

/* Regresa el nombre de la zona, o "Ninguna" en un simulacro */
static const char *obtener_zona(uint8_t id_estacion)
{
    if ((id_estacion < 1U) || (id_estacion > TOTAL_ESTACIONES))
    {
        return "Ninguna";
    }

    return estaciones[id_estacion - 1U].zona;
}

/* Convierte el estado de una estacion en texto */
static const char *obtener_estado(const estacion_t *estacion)
{
    if (!estacion->ya_se_conecto)
    {
        return "ESPERANDO";
    }

    if (!estacion->conectada)
    {
        return "DESCONECTADA";
    }

    if (estacion->intensidad > 0U)
    {
        return "ALERTA";
    }

    return "CONECTADA";
}

/* Imprime un evento en una linea */
static void imprimir_evento(const evento_t *evento)
{
    PRINTF("%s%s" COLOR_NORMAL, obtener_color(evento->intensidad), obtener_clasificacion(evento->intensidad));
    PRINTF(" | Zona: %s | Nivel: %u | Afectadas: %u | Avisadas: %u%s" BORRAR_LINEA "\r\n",
           obtener_zona(evento->id_estacion), evento->intensidad, evento->estaciones_afectadas,
           evento->estaciones_avisadas, evento->es_simulacro ? " | SIMULACRO" : "");
}

/* Imprime los eventos guardados, del mas reciente al mas antiguo */
static void imprimir_historial(void)
{
    uint32_t cantidad = (total_eventos < TOTAL_HISTORIAL) ? total_eventos : TOTAL_HISTORIAL;

    PRINTF(BORRAR_LINEA "\r\nHISTORIAL" BORRAR_LINEA "\r\n");

    if (cantidad == 0U)
    {
        PRINTF("Sin eventos" BORRAR_LINEA "\r\n");
    }

    for (uint32_t i = 0U; i < cantidad; i++)
    {
        uint32_t numero_evento = total_eventos - 1U - i;

        PRINTF("#%u ", (unsigned int)(numero_evento + 1U));
        imprimir_evento(&historial[numero_evento % TOTAL_HISTORIAL]);
    }
}

/* Dibuja la pantalla completa en la terminal */
static void imprimir_pantalla(void)
{
    uint32_t ahora_ms = sys_now();

    PRINTF(PANTALLA_INICIO "=== ALERTA SISMICA - SERVIDOR %d.%d.%d.%d:%u ===" BORRAR_LINEA "\r\n" BORRAR_LINEA "\r\n",
           configIP_ADDR0, configIP_ADDR1, configIP_ADDR2, configIP_ADDR3, PUERTO_UDP);
    PRINTF("ID  ZONA    ESTADO        NIVEL  ULT.MSG  ALERTAS  REPETIDAS  IP" BORRAR_LINEA "\r\n");

    for (uint32_t i = 0U; i < TOTAL_ESTACIONES; i++)
    {
        const estacion_t *estacion = &estaciones[i];

        PRINTF("%-3u %-7s %-13s %-6u ", (unsigned int)(i + 1U), estacion->zona, obtener_estado(estacion),
               estacion->intensidad);

        if (estacion->ya_se_conecto)
        {
            PRINTF("%5u s  %-8u %-10u %s", (unsigned int)((ahora_ms - estacion->ultimo_mensaje_ms) / 1000U),
                   (unsigned int)estacion->total_alertas, (unsigned int)estacion->total_duplicados,
                   ipaddr_ntoa(&estacion->ip));
        }
        else
        {
            PRINTF("    -    -        -          -");
        }

        PRINTF(BORRAR_LINEA "\r\n");
    }

    PRINTF(BORRAR_LINEA "\r\nEVENTO: ");

    if (evento_abierto)
    {
        PRINTF("Recibiendo alertas..." BORRAR_LINEA "\r\n");
    }
    else if (total_eventos == 0U)
    {
        PRINTF("Sin eventos" BORRAR_LINEA "\r\n");
    }
    else
    {
        imprimir_evento(&historial[(total_eventos - 1U) % TOTAL_HISTORIAL]);
    }

    if (mostrar_historial)
    {
        imprimir_historial();
    }

    PRINTF(BORRAR_LINEA "\r\nPaquetes ignorados: %u" BORRAR_LINEA "\r\n", (unsigned int)paquetes_ignorados);
    PRINTF("[s] simulacro  [r] reiniciar  [l] historial" BORRAR_LINEA "\r\n" PANTALLA_FINAL);
}

/* Actualiza la pantalla si hubo cambios o si ya paso un segundo */
static void actualizar_pantalla(void)
{
    uint32_t ahora_ms = sys_now();

    if (pantalla_pendiente || ((ahora_ms - ultima_actualizacion_ms) >= TIEMPO_ACTUALIZAR_PANTALLA_MS))
    {
        imprimir_pantalla();
        pantalla_pendiente      = false;
        ultima_actualizacion_ms = ahora_ms;
    }
}

/* Crea el socket UDP y lo deja escuchando en el puerto del servidor */
static bool iniciar_servidor(void)
{
    socket_servidor = udp_new();

    if (socket_servidor == NULL)
    {
        return false;
    }

    if (udp_bind(socket_servidor, IP_ADDR_ANY, PUERTO_UDP) != ERR_OK)
    {
        return false;
    }

    udp_recv(socket_servidor, recibir_paquete, NULL);
    return true;
}

/*!
 * @brief Main function.
 */
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
    /* attach 12 MHz clock to FLEXCOMM0 (debug console) */
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

    /* Set MAC address. */
#ifndef configMAC_ADDR
    (void)SILICONID_ConvertToMacAddr(&enet_config.macAddress);
#endif

    /* Get clock after hardware init. */
    enet_config.srcClockHz = EXAMPLE_CLOCK_FREQ;

    IP4_ADDR(&netif_ipaddr, configIP_ADDR0, configIP_ADDR1, configIP_ADDR2, configIP_ADDR3);
    IP4_ADDR(&netif_netmask, configNET_MASK0, configNET_MASK1, configNET_MASK2, configNET_MASK3);
    IP4_ADDR(&netif_gw, configGW_ADDR0, configGW_ADDR1, configGW_ADDR2, configGW_ADDR3);

    lwip_init();

    netif_add(&netif, &netif_ipaddr, &netif_netmask, &netif_gw, &enet_config, EXAMPLE_NETIF_INIT_FN, ethernet_input);
    netif_set_default(&netif);
    netif_set_up(&netif);

    while (ethernetif_wait_linkup(&netif, 5000) != ERR_OK)
    {
        PRINTF("PHY Auto-negotiation failed. Please check the cable connection and link partner setting.\r\n");
    }

    if (!iniciar_servidor())
    {
        PRINTF("Error: no se pudo iniciar el servidor UDP.\r\n");
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

    /* La pantalla del servidor reemplaza este mensaje y ya muestra su IP */
    PRINTF(PANTALLA_BORRAR);

    while (1)
    {
        /* Poll the driver, get any outstanding frames */
        ethernetif_input(&netif);

        sys_check_timeouts(); /* Handle all system timeouts for all core protocols */

        /* Tareas del servidor */
        revisar_estaciones();
        revisar_evento();
        revisar_teclado();
        actualizar_pantalla();
    }
}
#endif
