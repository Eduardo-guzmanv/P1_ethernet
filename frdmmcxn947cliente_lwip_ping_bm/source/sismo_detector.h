/*
 * Detector de sacudidas para la estacion sismica.
 * Lee el MPU6050 cada 10 ms, quita la gravedad (linea base que se adapta sola)
 * y mide cuanto se separa la aceleracion de esa linea base. Si pasa el umbral,
 * junta el pico durante una ventana corta y entrega UN nivel (1..5) por evento.
 *
 * Sin flotantes, sin raiz cuadrada, sin importar como este orientada la tarjeta.
 */
#ifndef SISMO_DETECTOR_H
#define SISMO_DETECTOR_H

#include <stdint.h>
#include <stdbool.h>
#include "fsl_lpi2c.h"

/* Umbrales de cada nivel, en milli-g de aceleracion dinamica (suma |dx|+|dy|+|dz|).
 * Son valores de demostracion para sacudir la tarjeta a mano, no una escala sismologica.
 * Con SISMO_DEBUG se imprime el pico de cada evento para calibrarlos. */
#define SISMO_NIVEL_MAXIMO    5U
#define SISMO_UMBRAL_NIVEL1_MG 60U
#define SISMO_UMBRAL_NIVEL2_MG 150U
#define SISMO_UMBRAL_NIVEL3_MG 300U
#define SISMO_UMBRAL_NIVEL4_MG 600U
#define SISMO_UMBRAL_NIVEL5_MG 1200U

#define SISMO_PERIODO_MUESTREO_MS 10U   /* 100 Hz, igual que el MPU6050 */
#define SISMO_VENTANA_EVENTO_MS   300U  /* tiempo que se junta el pico antes de avisar */
#define SISMO_ENFRIAMIENTO_MS     3000U /* tras avisar, ignora nuevas sacudidas este tiempo */
#define SISMO_MUESTRAS_CALIBRACION 100U /* 1 s inicial quieto para fijar la linea base */
#define SISMO_ERRORES_PARA_AVISO  50U   /* lecturas I2C fallidas seguidas antes de avisar */

/* Rapidez con que la linea base sigue a la aceleracion (constante de tiempo en muestras).
 * Mas chico = ignora mejor inclinaciones lentas, pero atenua sacudidas muy lentas (< 1 Hz).
 * 32 muestras = ~0.32 s a 100 Hz: deja pasar sacudidas de 2 Hz o mas casi sin atenuar. */
#define SISMO_FILTRO_DIV          32U

/* Inicia I2C + MPU6050. Regresa false si el sensor no responde (el boton sigue sirviendo). */
bool sismo_detector_init(LPI2C_Type *base, uint32_t reloj_i2c_hz);

/* Llamar en cada vuelta del lazo principal con sys_now().
 * Regresa true UNA vez por sacudida y deja el nivel (1..5) en *nivel. */
bool sismo_detector_tarea(uint32_t ahora_ms, uint8_t *nivel);

/* Pico (mg) del ultimo evento avisado, para imprimirlo en terminal */
uint16_t sismo_detector_ultimo_pico_mg(void);

#endif /* SISMO_DETECTOR_H */
