#include "sismo_detector.h"
#include "mpu6050.h"
#include "fsl_debug_console.h"

typedef enum
{
    SISMO_APAGADO,      /* el MPU6050 no respondio al iniciar */
    SISMO_CALIBRANDO,   /* fijando la linea base (tarjeta quieta) */
    SISMO_VIGILANDO,    /* esperando una sacudida */
    SISMO_EVENTO,       /* sacudida en curso: se junta el pico */
    SISMO_ENFRIAMIENTO  /* ya se aviso: se ignora un rato */
} estado_sismo_t;

static const uint16_t umbrales_mg[SISMO_NIVEL_MAXIMO] = {SISMO_UMBRAL_NIVEL1_MG, SISMO_UMBRAL_NIVEL2_MG,
                                                          SISMO_UMBRAL_NIVEL3_MG, SISMO_UMBRAL_NIVEL4_MG,
                                                          SISMO_UMBRAL_NIVEL5_MG};

static estado_sismo_t estado = SISMO_APAGADO;

/* Linea base por eje en punto fijo Q6 (mg * 64). Sigue la gravedad y los cambios lentos
 * de inclinacion (constante de tiempo = SISMO_FILTRO_DIV muestras, ~0.32 s), asi no hay que
 * calibrar la orientacion. */
static int32_t base_q6[3];

static uint16_t muestras_calibradas = 0U;
static uint8_t errores_seguidos     = 0U;
static uint32_t ultima_muestra_ms   = 0U;
static uint32_t inicio_evento_ms    = 0U;
static uint32_t fin_evento_ms       = 0U;
static uint16_t pico_mg             = 0U;
static uint16_t ultimo_pico_mg      = 0U;

bool sismo_detector_init(LPI2C_Type *base, uint32_t reloj_i2c_hz)
{
    if (MPU6050_Init(base, reloj_i2c_hz) != kStatus_Success)
    {
        estado = SISMO_APAGADO;
        return false;
    }

    muestras_calibradas = 0U;
    errores_seguidos    = 0U;
    estado              = SISMO_CALIBRANDO;
    return true;
}

static int32_t valor_absoluto(int32_t v)
{
    return (v < 0) ? -v : v;
}

/* Actualiza la linea base con la muestra y regresa cuanto se separa de ella (suma de los 3 ejes) */
static uint16_t actualizar_y_medir(const mpu6050_accel_t *a)
{
    const int32_t m[3] = {a->x_mg, a->y_mg, a->z_mg};
    int32_t separacion = 0;

    for (uint8_t i = 0U; i < 3U; i++)
    {
        if (muestras_calibradas == 0U)
        {
            base_q6[i] = m[i] * 64; /* primera muestra: arrancar la linea base aqui */
        }

        separacion += valor_absoluto(m[i] - (base_q6[i] / 64));
        base_q6[i] += ((m[i] * 64) - base_q6[i]) / (int32_t)SISMO_FILTRO_DIV;
    }

    return (separacion > 65535) ? 65535U : (uint16_t)separacion;
}

static uint8_t nivel_desde_pico(uint16_t pico)
{
    uint8_t nivel = 0U;

    for (uint8_t i = 0U; i < SISMO_NIVEL_MAXIMO; i++)
    {
        if (pico >= umbrales_mg[i])
        {
            nivel = (uint8_t)(i + 1U);
        }
    }
    return nivel;
}

bool sismo_detector_tarea(uint32_t ahora_ms, uint8_t *nivel)
{
    mpu6050_accel_t accel;
    uint16_t separacion_mg;

    if (estado == SISMO_APAGADO)
    {
        return false;
    }

    /* Una muestra cada 10 ms */
    if ((ahora_ms - ultima_muestra_ms) < SISMO_PERIODO_MUESTREO_MS)
    {
        return false;
    }
    ultima_muestra_ms = ahora_ms;

    if (MPU6050_LeerAceleracion(&accel) != kStatus_Success)
    {
        if (errores_seguidos < 255U)
        {
            errores_seguidos++;
        }
        if (errores_seguidos == SISMO_ERRORES_PARA_AVISO)
        {
            PRINTF("MPU6050 sin respuesta: revisa SDA/SCL y la alimentacion\r\n");
        }
        return false;
    }
    errores_seguidos = 0U;

    separacion_mg = actualizar_y_medir(&accel);

    switch (estado)
    {
        case SISMO_CALIBRANDO:
            muestras_calibradas++;
            if (muestras_calibradas >= SISMO_MUESTRAS_CALIBRACION)
            {
                estado = SISMO_VIGILANDO;
                PRINTF("Detector de sismos listo\r\n");
            }
            break;

        case SISMO_VIGILANDO:
            if (separacion_mg >= umbrales_mg[0])
            {
                inicio_evento_ms = ahora_ms;
                pico_mg          = separacion_mg;
                estado           = SISMO_EVENTO;
            }
            break;

        case SISMO_EVENTO:
            if (separacion_mg > pico_mg)
            {
                pico_mg = separacion_mg;
            }

            if ((ahora_ms - inicio_evento_ms) >= SISMO_VENTANA_EVENTO_MS)
            {
                ultimo_pico_mg = pico_mg;
                fin_evento_ms  = ahora_ms;
                estado         = SISMO_ENFRIAMIENTO;
                *nivel         = nivel_desde_pico(pico_mg);
                return true;
            }
            break;

        case SISMO_ENFRIAMIENTO:
            if ((ahora_ms - fin_evento_ms) >= SISMO_ENFRIAMIENTO_MS)
            {
                estado = SISMO_VIGILANDO;
            }
            break;

        default:
            break;
    }

    return false;
}

uint16_t sismo_detector_ultimo_pico_mg(void)
{
    return ultimo_pico_mg;
}
