/**
* @file downlink_led_app_settings.c
* @brief Source file for Application Settings functions.
* @author Marcel Maas, Thomas Wiemken, ELV Elektronik AG
**/

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_settings.h"
#include "eeprom_emul_types.h"
#include "eeprom.h"
#include "flash_user_func.h"
#include "base.h"
#include "sys_app.h"

/* Definitions ---------------------------------------------------------------*/
/* Typedefs ------------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
/* Prototypes ----------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
EE_Status app_eeprom_set_defaults( void )
{
  EE_Status ee_status = EE_OK;

  ee_status = EEPROM_start_cleanup_polling_mode_if_needed( ee_status );

  ee_status = EE_WriteVariable32bits( EEPROM_EMU_DUTYCYCLE_ADDRESS, APP_DUTYCYCLE_DEFAULT );
  ee_status = EE_WriteVariable8bits( EEPROM_EMU_LED_STATUS_ADDRESS, APP_LED_STATUS_DEFAULT );
  ee_status = EEPROM_start_cleanup_polling_mode_if_needed( ee_status );
  
  return ee_status;
}

EE_Status app_eeprom_set_settings( application_settings_t settings )
{
  EE_Status ee_status = EE_OK;

  ee_status = EEPROM_write_ee_variable_32bits( EEPROM_EMU_DUTYCYCLE_ADDRESS, settings.u32_app_dutycycle );
  ee_status = EEPROM_write_ee_variable_8bits( EEPROM_EMU_LED_STATUS_ADDRESS, settings.u8_app_led_status );
  
  return ee_status;
}

EE_Status app_eeprom_get_settings( application_settings_t *settings )
{
  EE_Status ee_status = EE_OK;

  ee_status = EEPROM_read_ee_variable_32bits( EEPROM_EMU_DUTYCYCLE_ADDRESS, &settings->u32_app_dutycycle );
  ee_status = EEPROM_read_ee_variable_8bits( EEPROM_EMU_LED_STATUS_ADDRESS, &settings->u8_app_led_status );
  
  return ee_status;
}

void app_settings_print( application_settings_t settings )
{
  APP_LOG( TS_OFF, VLEVEL_L, "############ ELV-BM-TRX1 Template - settings ############\r\n" );
  if( settings.u32_app_dutycycle != 0 )
  {
    APP_LOG( TS_OFF, VLEVEL_L, "Interval: %u seconds\r\n", settings.u32_app_dutycycle / 1000 );
  }
  else
  {
    APP_LOG( TS_OFF, VLEVEL_L, "Interval: off\r\n" );
  }

  if( settings.u8_app_led_status != 0 )
  {
    APP_LOG( TS_OFF, VLEVEL_L, "LED: on\r\n" );
  }
  else
  {
    APP_LOG( TS_OFF, VLEVEL_L, "LED: off\r\n" );
  }
  APP_LOG( TS_OFF, VLEVEL_L, "#########################################################\r\n" );
}

void app_settings_process_dl( uint8_t *buffer, uint8_t buffer_size )
{
  application_settings_t temp_settings;

  if( buffer[0] == APP_DEVICE_ID )
  {
    if( buffer_size == APP_DL_SIZE_THREE_BYTES )
    {
      app_eeprom_get_settings( &temp_settings );
      
      temp_settings.u32_app_dutycycle = ( buffer[1] * 30000 );  // set new application duty cycle
      temp_settings.u8_app_led_status = buffer[2];              // set new LED status

      app_eeprom_set_settings( temp_settings );
    }
  }
}
