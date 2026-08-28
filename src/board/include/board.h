#ifndef BOARD_H
#define BOARD_H

#include "led.h"
#include "twi.h"
#include "adc_types.h"
#include "pwm_driver.h"
#include "gpio.h"
#include "servo.h"

/* =========================
 * Status LEDs
 * ========================= */

#define LED_READY       GPIO_PIN(B, 0, GPIO_OUT)
#define LED_PROCESSING  GPIO_PIN(B, 1, GPIO_OUT)
#define LED_ERROR       GPIO_PIN(B, 2, GPIO_OUT)

typedef struct
{
    led_t ready;
    led_t processing;
    led_t error;
} board_leds_t;

extern board_leds_t led;


/* =========================
 * LDR Sensors
 * ========================= */

#define LDR_EAST   ADC_CHANNEL_0
#define LDR_WEST   ADC_CHANNEL_1

extern adc_config_t east_ldr_adc_config;
extern adc_config_t west_ldr_adc_config;


/* =========================
 * Servo
 * ========================= */

#define SERVO_PIN   GPIO_PIN(D, 5, GPIO_OUT)

extern const pwm_config_t servo_pwm_config;

extern servo_config_t servo_config;
extern servo_t servo;


/* =========================
 * I²C / TWI
 * ========================= */

extern twi_config_t board_twi_config;


/* =========================
 * UART
 * ========================= */

#define BOARD_UART_BAUDRATE  9600

#endif