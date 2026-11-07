/*
 * Driver del acelerometro MPU6050 por I2C (LPI2C del MCX N947).
 * Solo usa el acelerometro: sin giroscopio, sin angulos, sin flotantes.
 */
#ifndef MPU6050_H
#define MPU6050_H

#include <stdint.h>
#include <stddef.h>
#include "fsl_common.h"
#include "fsl_lpi2c.h"

/* AD0 a GND = 0x68, AD0 a 3V3 = 0x69 */
#ifndef MPU6050_DIRECCION_I2C
#define MPU6050_DIRECCION_I2C 0x68U
#endif

/* Velocidad del bus (el MPU6050 soporta hasta 400 kHz) */
#ifndef MPU6050_BAUDRATE_HZ
#define MPU6050_BAUDRATE_HZ 100000U
#endif

/* Aceleracion en milli-g (1000 = 1 g). Rango del sensor: +-2 g */
typedef struct
{
    int16_t x_mg;
    int16_t y_mg;
    int16_t z_mg;
} mpu6050_accel_t;

/* Inicia el LPI2C, verifica WHO_AM_I y configura el sensor:
 * +-2 g, filtro pasa-bajas de 44 Hz, 100 muestras/s.
 * Regresa kStatus_Success si el sensor respondio y se configuro. */
status_t MPU6050_Init(LPI2C_Type *base, uint32_t reloj_i2c_hz);

/* Lee los 3 ejes de aceleracion (bloqueante, ~1 ms a 100 kHz).
 * Llamar solo desde el lazo principal, nunca desde una interrupcion. */
status_t MPU6050_LeerAceleracion(mpu6050_accel_t *accel);

#endif /* MPU6050_H */
