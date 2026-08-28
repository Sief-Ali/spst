#include "app.h"

#include <stdio.h> 

#include "logger.h"
#include "analog.h"
#include "board.h"


void APP_Init(void)
{
    Logger_Log(
      LOG_BOOT,
      "System Ready");
}

void APP_Run(void)
{

    Logger_Log(
      LOG_EVENT,
      "Application Running");
    
      while (1)
      {
          analog_ldr_readings_t ldr_readings;
          if (Analog_ReadLdrs(
              &east_ldr_adc_config.input,
              &west_ldr_adc_config.input,
              &ldr_readings) != 0U)
          {
              // Process the LDR readings
              // For example, you can log the readings
              char message[100];
              snprintf(message, sizeof(message), "LDR Readings - East: %u, West: %u", ldr_readings.east, ldr_readings.west);
              Logger_Log(LOG_INFO, message);
          } else {
              Logger_Log(LOG_ERROR, "Failed to read LDRs");
          }
      }
}