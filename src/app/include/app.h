#ifndef APP_H
#define APP_H

/* Initializes application-level modules before entering the main loop. */
void APP_Init(void);

/* Creates the Sensor, Control and Display tasks, then starts FreeRTOS. */
void APP_StartScheduler(void);

#endif
