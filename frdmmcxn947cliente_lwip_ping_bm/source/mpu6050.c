#include "mpu6050.h"

/* Registros del MPU6050 */
#define REG_SMPLRT_DIV   0x19U
#define REG_CONFIG       0x1AU
#define REG_ACCEL_CONFIG 0x1CU
#define REG_ACCEL_XOUT_H 0x3BU
#define REG_PWR_MGMT_1   0x6BU
#define REG_WHO_AM_I     0x75U

#define WHO_AM_I_ESPERADO 0x68U

/* Sensibilidad en +-2 g: 16384 LSB por g. mg = raw * 1000 / 16384 = raw * 125 / 2048 */
#define MG_NUMERADOR   125
#define MG_DENOMINADOR 2048

static LPI2C_Type *s_i2c;

/* Escribe un registro. Usa la transferencia del SDK, que siempre cierra el bus (STOP)
 * aunque haya error, asi el bus no se queda trabado. */
static status_t escribir_registro(uint8_t registro, uint8_t valor)
{
    lpi2c_master_transfer_t xfer = {0};

    xfer.flags          = kLPI2C_TransferDefaultFlag;
    xfer.slaveAddress   = MPU6050_DIRECCION_I2C;
    xfer.direction      = kLPI2C_Write;
    xfer.subaddress     = registro;
    xfer.subaddressSize = 1U;
    xfer.data           = &valor;
    xfer.dataSize       = 1U;

    return LPI2C_MasterTransferBlocking(s_i2c, &xfer);
}

/* Lee varios registros seguidos (START, direccion, registro, REPEATED START, lectura, STOP) */
static status_t leer_registros(uint8_t registro, uint8_t *datos, size_t cantidad)
{
    lpi2c_master_transfer_t xfer = {0};

    xfer.flags          = kLPI2C_TransferDefaultFlag;
    xfer.slaveAddress   = MPU6050_DIRECCION_I2C;
    xfer.direction      = kLPI2C_Read;
    xfer.subaddress     = registro;
    xfer.subaddressSize = 1U;
    xfer.data           = datos;
    xfer.dataSize       = cantidad;

    return LPI2C_MasterTransferBlocking(s_i2c, &xfer);
}

status_t MPU6050_Init(LPI2C_Type *base, uint32_t reloj_i2c_hz)
{
    lpi2c_master_config_t config;
    uint8_t quien_soy = 0U;
    status_t estado;

    s_i2c = base;

    LPI2C_MasterGetDefaultConfig(&config);
    config.baudRate_Hz = MPU6050_BAUDRATE_HZ;
    LPI2C_MasterInit(s_i2c, &config, reloj_i2c_hz);

    /* 1. Comprobar que el sensor esta conectado y es un MPU6050 */
    estado = leer_registros(REG_WHO_AM_I, &quien_soy, 1U);
    if (estado != kStatus_Success)
    {
        return estado;
    }
    if (quien_soy != WHO_AM_I_ESPERADO)
    {
        return kStatus_Fail;
    }

    /* 2. Despertar (sale de sleep) y usar el PLL del giroscopio X como reloj */
    estado = escribir_registro(REG_PWR_MGMT_1, 0x01U);
    if (estado != kStatus_Success)
    {
        return estado;
    }

    /* 3. Filtro digital pasa-bajas: DLPF_CFG = 3 (acelerometro 44 Hz, salida interna 1 kHz) */
    estado = escribir_registro(REG_CONFIG, 0x03U);
    if (estado != kStatus_Success)
    {
        return estado;
    }

    /* 4. Frecuencia de muestreo = 1 kHz / (1 + 9) = 100 Hz */
    estado = escribir_registro(REG_SMPLRT_DIV, 9U);
    if (estado != kStatus_Success)
    {
        return estado;
    }

    /* 5. Rango del acelerometro: +-2 g */
    return escribir_registro(REG_ACCEL_CONFIG, 0x00U);
}

status_t MPU6050_LeerAceleracion(mpu6050_accel_t *accel)
{
    uint8_t d[6];
    status_t estado;
    int16_t x_raw;
    int16_t y_raw;
    int16_t z_raw;

    if (accel == NULL)
    {
        return kStatus_InvalidArgument;
    }

    estado = leer_registros(REG_ACCEL_XOUT_H, d, sizeof(d));
    if (estado != kStatus_Success)
    {
        return estado;
    }

    /* El MPU6050 manda primero el byte alto de cada eje */
    x_raw = (int16_t)(((uint16_t)d[0] << 8) | d[1]);
    y_raw = (int16_t)(((uint16_t)d[2] << 8) | d[3]);
    z_raw = (int16_t)(((uint16_t)d[4] << 8) | d[5]);

    accel->x_mg = (int16_t)(((int32_t)x_raw * MG_NUMERADOR) / MG_DENOMINADOR);
    accel->y_mg = (int16_t)(((int32_t)y_raw * MG_NUMERADOR) / MG_DENOMINADOR);
    accel->z_mg = (int16_t)(((int32_t)z_raw * MG_NUMERADOR) / MG_DENOMINADOR);

    return kStatus_Success;
}
