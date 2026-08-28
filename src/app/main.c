
#include "app.h"
#include "led.h"
#include "board.h"
#include "twi.h"
#include "logger.h"
#include "analog.h"
#include "servo.h"


/* Board LEDs initialized before the application modules start. */
static led_t * const board_led_list[] = {
    &led.ready,
    &led.processing,
    &led.error
};



int main(void) {

    // Initialize the board LEDs
    int led_length = sizeof(board_led_list) / sizeof(board_led_list[0]);
    for (int index = 0; index < led_length ; index++) {
      LED_Init(board_led_list[index]);
    } 

    /* Initialize the ADC once; this sets its configuration, not a channel. */
    Analog_Init(&west_ldr_adc_config);
    
    TWI_Init(&board_twi_config);

    Logger_Init();

    Servo_Init(&servo);

    // Your code here
    APP_Init();

    APP_Run();

    return 0;
}