/**
* @file app_settings.h
* @brief Header file for Application Settings functions.
* @author Marcel Maas, Thomas Wiemken, ELV Elektronik AG
**/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_SETTINGS_H
#define __APP_SETTINGS_H

#ifdef __cplusplus
 extern "C" {
#endif

// Includes --------------------------------------------------------------------
#include "eeprom_emul_types.h"
#include "flash_user_func.h"
#include <stdbool.h>
// Definitions -----------------------------------------------------------------
#define APP_DL_PORT                 10
#define APP_DL_SIZE_TWO_BYTES       2
#define APP_DL_SIZE_THREE_BYTES     3
#define APP_DEVICE_ID               0x01

#define APP_DUTYCYCLE_DEFAULT       30000 // 30 seconds
#define APP_LED_STATUS_DEFAULT      0     // LED off

// SETTINGS - APP_DEFAULT - START
#define APP_SETTING_DEFAULT         {                         \
                                      APP_DUTYCYCLE_DEFAULT,  \
                                      APP_LED_STATUS_DEFAULT, \
                                    }
// SETTINGS - APP_DEFAULT - END

// Exported types --------------------------------------------------------------
typedef struct
{
  uint32_t u32_app_dutycycle;
  uint8_t u8_app_led_status;
} application_settings_t;

// Exported macro --------------------------------------------------------------
// Exported functions ----------------------------------------------------------

EE_Status app_eeprom_set_defaults( void );
EE_Status app_eeprom_set_settings( application_settings_t settings );
EE_Status app_eeprom_get_settings( application_settings_t *settings );

void app_settings_print( application_settings_t settings );

void app_settings_process_dl( uint8_t *buffer, uint8_t buffer_size );

#ifdef __cplusplus
}
#endif

/**
  * @}
  */

#endif /* __APP_SETTINGS_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
