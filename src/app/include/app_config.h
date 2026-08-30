#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <stdint.h>

#define EEPROM_CONFIG_ADDRESS 0U
#define EEPROM_CONFIG_SIZE    19U

typedef struct
{
    uint16_t dead_band;
    uint8_t travel_limit_lower;
    uint8_t travel_limit_upper;
    uint8_t east_park_angle;
    uint16_t cloud_entry_level;
    uint16_t cloud_exit_level;
    uint16_t cloud_confirmation_time_s;
    uint8_t night_start_hour;
    uint8_t night_start_minute;
    uint8_t night_end_hour;
    uint8_t night_end_minute;
    uint8_t day_start_hour;
    uint8_t day_start_minute;
    uint8_t day_end_hour;
    uint8_t day_end_minute;
} app_runtime_config_t;

typedef enum
{
    APP_CFG_DEAD_BAND = 0U,
    APP_CFG_TRAVEL_LIMIT_LOWER,
    APP_CFG_TRAVEL_LIMIT_UPPER,
    APP_CFG_EAST_PARK_ANGLE,
    APP_CFG_CLOUD_ENTRY_LEVEL,
    APP_CFG_CLOUD_EXIT_LEVEL,
    APP_CFG_CLOUD_CONFIRMATION_TIME_S,
    APP_CFG_NIGHT_START_HOUR,
    APP_CFG_NIGHT_START_MINUTE,
    APP_CFG_NIGHT_END_HOUR,
    APP_CFG_NIGHT_END_MINUTE,
    APP_CFG_DAY_START_HOUR,
    APP_CFG_DAY_START_MINUTE,
    APP_CFG_DAY_END_HOUR,
    APP_CFG_DAY_END_MINUTE,
    APP_CFG_COUNT
} app_cfg_id_t;

typedef struct
{
    app_cfg_id_t id;
    uint16_t eeprom_address;
    uint8_t width;
} app_cfg_map_entry_t;

const app_runtime_config_t *APP_Config_GetDefault(void);
uint16_t APP_Config_GetValue(const app_runtime_config_t *config, app_cfg_id_t id);
void APP_Config_SetValue(app_runtime_config_t *config, app_cfg_id_t id, uint16_t value);
void APP_Config_WriteEeprom(const app_runtime_config_t *config);
void APP_Config_ReadEeprom(app_runtime_config_t *config);
uint8_t APP_Config_MatchesDefaults(const app_runtime_config_t *config);
void APP_Config_ClearRegion(void);

#endif /* APP_CONFIG_H */
