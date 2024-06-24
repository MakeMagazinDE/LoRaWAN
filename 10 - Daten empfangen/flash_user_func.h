/**
* @file flash_user_func.h
* @brief Header file for user flash functions.
* @author Marcel Maas, Thomas Wiemken, ELV Elektronik AG
**/


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FLASH_USER_FUNC_H
#define __FLASH_USER_FUNC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "eeprom_emul_types.h"
/* Exported types ------------------------------------------------------------*/
#define EEPROM_EMU_DATA_INIT_VALUE    0x00000002

typedef enum EEPROM_EMU_VirtTable
{
  EEPROM_EMU_DATA_INIT_ADDRESS = 0x0001,  // First virtual address
  EEPROM_EMU_DUTYCYCLE_ADDRESS,           // 0x0002
  EEPROM_EMU_LED_STATUS_ADDRESS,          // 0x0003
  EEPROM_EMU_VirtTable_SIZE   // Used to calculate the number of variables
} EEPROM_EMU_VirtTable;

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
EE_Status flash_user_func_init( bool b_force_default );
EE_Status flash_user_func_reinit( void );
EE_Status flash_user_func_reset( void );
EE_Status flash_user_func_eeprom_data_set_default( void );

/* Private types -------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

/**
  * @}
  */

#endif /* __FLASH_USER_FUNC_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
