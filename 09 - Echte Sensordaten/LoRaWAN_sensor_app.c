/**
* @file LoRaWAN_sensor_app.c
* @brief Source file for Application functions.
* @author Marcel Maas, Thomas Wiemken, ELV Elektronik AG
**/


/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app.h"
#include "app_settings.h"
#include "eeprom_emul_types.h"
#include "eeprom.h"
#include "Region.h"
#include "flash_user_func.h"
#include "base.h"
#include "hw_gpio.h"
#include "utilities_def.h"
#include "stm32_lpm.h"
#include "stm32_seq.h"
#include <stdbool.h>
#include <stdint.h>
#include "user_timer.h"
#include "stm32_lpm_if.h"
#include <stdio.h>
#include "adc_if.h"
#include "i2c.h"
#include "ELV-AM-TH1.h"

// Definitions -----------------------------------------------------------------
// Typedefs --------------------------------------------------------------------
// Variables -------------------------------------------------------------------
static application_settings_t app_settings              = APP_SETTING_DEFAULT;
static UTIL_TIMER_Object_t app_tx_timer;
static UTIL_TIMER_Object_t app_check_loramac_timer;
static uint32_t remaining_time_tx_timer                 = 0;

base_callbacks_t base_app_cb =
{
  .base_post_join                                       = app_post_join,
  .base_process_downlink                                = app_process_downlink,
  .base_user_button_event                               = app_user_button_event,
  .base_enable_irqs                                     = app_enable_irqs,
  .base_disable_irqs                                    = app_disable_irqs,
};

static lpm_callbacks_t lpm_app_cb =
{
  .lpm_exit_stop_mode                                   = app_exit_stop_mode,
};

// Prototypes ------------------------------------------------------------------
// Exported functions ----------------------------------------------------------
void app_init( void )
{
  base_print_bl_and_app_version( ( uint8_t * )APP_APPLICATION_NAME_STR, ( uint8_t * )APP_APPLICATION_VERSION_STR );

  lpm_init_cb( &lpm_app_cb );
  flash_user_func_init( false );

  app_eeprom_get_settings( &app_settings );
  app_settings_print( app_settings );
  
  // Init the ELV-AM-TH1 depending hardware
  elv_am_th1_init();

  UTIL_SEQ_RegTask( ( 1 << CFG_SEQ_Task_lora_tx_task ), UTIL_SEQ_RFU, app_send_tx_data_cb );
  UTIL_TIMER_Create( &app_check_loramac_timer,  0xFFFFFFFFU, UTIL_TIMER_ONESHOT, app_check_loramac_cb, NULL );
  UTIL_TIMER_SetPeriod( &app_check_loramac_timer, APP_LORAMAC_CHECK_BUSY_INTERVAL );
  UTIL_TIMER_Create( &app_tx_timer,  0xFFFFFFFFU, UTIL_TIMER_ONESHOT, app_on_tx_timer_event_cb, NULL );

  base_init( &base_app_cb, APP_LORAWAN_ADR_STATE, APP_LORAWAN_DATA_RATE );

  base_join();
}

void app_send_tx_data_cb( void )
{
  UTIL_TIMER_Time_t next_tx_in = 0;

  app_set_lorawan_payload();
  UTIL_TIMER_Start( &app_check_loramac_timer );
  base_tx( &next_tx_in );
}

void app_set_lorawan_payload( void )
{
  uint8_t u8_payload_idx = 0;
  uint16_t bat = base_get_supply_level();

  LmHandlerAppData_t *app_data = base_get_app_data_ptr();

  app_data->Port = APP_LORAWAN_PORT;

  app_data->Buffer[u8_payload_idx++] = base_get_tx_reason();     // TX-Reason
  app_data->Buffer[u8_payload_idx++] = 0x00;                     // Reserved
  app_data->Buffer[u8_payload_idx++] = 0x00;                     // Reserved
  app_data->Buffer[u8_payload_idx++] = ( uint8_t ) ( bat >> 8 ); // Supply voltage (high byte)
  app_data->Buffer[u8_payload_idx++] = ( uint8_t ) ( bat );      // Supply voltage (low byte)

  if( elv_am_th1_is_present() )
  {
    th1_data_values_t t_th1_data_values = { 0 };  // Struct for the measured data values

    elv_am_th1_do_measurements( &t_th1_data_values );

    APP_LOG( TS_OFF, VLEVEL_L, "Check Value:     %5u %%\r\n", t_th1_data_values.u8_HDC2080_humidity );

    app_data->Buffer[u8_payload_idx++] = 0x02;                                                            // Datatype: Temperature
    app_data->Buffer[u8_payload_idx++] = ( uint8_t ) ( t_th1_data_values.i16_ntc_temperature >> 8 );      // Temperature [High Byte]
    app_data->Buffer[u8_payload_idx++] = ( uint8_t ) ( t_th1_data_values.i16_ntc_temperature );           // Temperature [Low Byte]

    app_data->Buffer[u8_payload_idx++] = 0x03;                                                            // Datatype: Temperature + Relative Humidity
    app_data->Buffer[u8_payload_idx++] = ( uint8_t ) ( t_th1_data_values.i16_HDC2080_temperature >> 8 );  // Temperature [High Byte]
    app_data->Buffer[u8_payload_idx++] = ( uint8_t ) ( t_th1_data_values.i16_HDC2080_temperature );       // Temperature [Low Byte]
    app_data->Buffer[u8_payload_idx++] = t_th1_data_values.u8_HDC2080_humidity;                           // Relative Humidity
  }

  app_data->BufferSize = u8_payload_idx;  // Watch out! The maximum payload size must not exceed 51 bytes!
}

void app_check_loramac_cb( void *context )
{
  if( LoRaMacIsBusy() )
  {
    UTIL_TIMER_Start( &app_check_loramac_timer );
  }
  else
  {
    app_post_loramac_busy();
  }
}

void app_post_join( void )
{
  app_on_tx_timer_event_cb( NULL );
}

void app_post_loramac_busy( void )
{
  base_set_tx_reason( TX_REASON_UNDEFINED_EVENT );

  user_timer_restart( &app_tx_timer, &remaining_time_tx_timer, app_get_dutycycle() );

  base_enable_irqs();
}

void app_process_downlink( uint8_t port, uint8_t *buffer, uint8_t buffer_size )
{
  switch( port )
  {
    case APP_DL_PORT:
      app_settings_process_dl( buffer, buffer_size ); // Process the downlink data
      app_eeprom_get_settings( &app_settings ); // Load the new stored application settings
      remaining_time_tx_timer = 0;              // Set the variable to zero to use the new duty cycle
      app_settings_print( app_settings );       // Print the new application settings over UART
      break;
    default:
      break;
  }
}

void app_user_button_event( void )
{
  UTIL_TIMER_GetRemainingTime( &app_tx_timer, &remaining_time_tx_timer );
  UTIL_TIMER_Stop( &app_tx_timer );

  base_set_tx_reason( TX_REASON_USER_BUTTON_EVENT );
  base_set_lora_msg_type( LORAMAC_HANDLER_CONFIRMED_MSG );

  // start LoRaWAN transmit task
  UTIL_SEQ_SetTask( ( 1 << CFG_SEQ_Task_lora_tx_task ), CFG_SEQ_Prio_0 );
}

void app_cyclic_event( void )
{
  // start LoRaWAN transmit task
  UTIL_SEQ_SetTask( ( 1 << CFG_SEQ_Task_lora_tx_task ), CFG_SEQ_Prio_0 );
}

void app_on_tx_timer_event_cb( void *context )
{
  base_disable_irqs();

  base_set_tx_reason( TX_REASON_APP_CYCLE_EVENT );
  base_set_lora_msg_type( LORAMAC_HANDLER_UNCONFIRMED_MSG );
  
  app_cyclic_event();
}

void app_set_dutycycle( uint32_t value )
{
  app_settings.u32_app_dutycycle = value;
}

uint32_t app_get_dutycycle( void )
{
  if( app_settings.u32_app_dutycycle > BASE_DUTYCYCLE_CORRECTION_MS )
  {
    return app_settings.u32_app_dutycycle - BASE_DUTYCYCLE_CORRECTION_MS;
  }
  else
  {
    return app_settings.u32_app_dutycycle;
  }
}

void app_exit_stop_mode( void )
{
  /* Add code here that should be executed after waking up from sleep mode */
  i2c_mod2_init();
}

void app_enable_irqs( void )
{
  user_timer_restart( &app_tx_timer, &remaining_time_tx_timer, app_get_dutycycle() );
  base_enable_irqs();
}

void app_disable_irqs( void )
{
  user_timer_stop( &app_tx_timer, &remaining_time_tx_timer );
  base_disable_irqs();
}
