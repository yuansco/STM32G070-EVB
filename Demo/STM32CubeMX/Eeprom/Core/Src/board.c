
#include "at24c02.h"
#include "board.h"
#include "main.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include <stdio.h>
#include <string.h>

/******************************************************************************/
/* Extern Handle Type */

extern I2C_HandleTypeDef hi2c1;

#if defined(UART_DEBUG)
extern UART_HandleTypeDef huart1;
#endif




/******************************************************************************/
/* UART Debug  */

#if defined(__GNUC__) && defined(UART_DEBUG) && !defined(UART_DEBUG_GPIO)
int _write(int fd, char * ptr, int len)
{
  HAL_UART_Transmit(&huart1, (uint8_t *) ptr, len, HAL_MAX_DELAY);
  return len;
}
#elif defined (__ICCARM__) && defined(UART_DEBUG)
#include "LowLevelIOInterface.h"
size_t __write(int handle, const unsigned char * buffer, size_t size)
{
  HAL_UART_Transmit(&huart1, (uint8_t *) buffer, size, HAL_MAX_DELAY);
  return size;
}
#elif defined (__CC_ARM) && defined(UART_DEBUG)
int fputc(int ch, FILE *f)
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
  return ch;
}
#endif

/******************************************************************************/

/**
 * flash init function 
 * https://stackoverflow.com/questions/73390378/stm32g0b1ce-can-the-boot-option-bits-be-used-to-jump-to-system-bootloader
 * 
 * RM0454 Reference manual - STM32G0x0 advanced Arm®-based 32-bit MCUs
 * https://www.st.com/resource/en/reference_manual/rm0454-stm32g0x0-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
 * Bit 24 nBOOT_SEL: BOOT0 signal source selection
 * This option bit defines the source of the BOOT0 signal.
 * 0: BOOT0 pin (legacy mode)
 * 1: nBOOT0 option bit
 */

void FLASH_Init() {

  // Basic de-bounce for reset
  HAL_Delay(100);

  // Read, modify & write user option bits
  // nBOOT_SEL = 1; will select boot area from BOOT0 pin

  uint32_t optBits = FLASH->OPTR;

  PRINTF("optBits = 0x%02x \r\n", (unsigned int) optBits);

  if ((optBits & FLASH_OPTR_nBOOT_SEL) != FLASH_OPTR_nBOOT_SEL) {
    PRINTF("nBOOT_SEL = 0\r\n");
    return;
  }

  PRINTF("Update nBOOT_SEL to 0\r\n");

  optBits &= ~(FLASH_OPTR_nBOOT_SEL);

  // Unlock flash
  HAL_FLASH_Unlock();

  // Clear OPTLOCK
  HAL_FLASH_OB_Unlock();

  // Set up struct with desired bits
  FLASH_OBProgramInitTypeDef optionBytesSetting = {0};
  optionBytesSetting.OptionType = OPTIONBYTE_USER;
  optionBytesSetting.USERConfig = optBits;
  optionBytesSetting.USERType = OB_USER_nBOOT0;

  // Write Option Bytes
  HAL_FLASHEx_OBProgram(&optionBytesSetting);

  HAL_Delay(10);

  // Soft reset
  PRINTF("Reboot!\r\n\r\n");
  HAL_FLASH_OB_Launch();
  NVIC_SystemReset();    // is not reached

}


/******************************************************************************/
/* I2C Scan function */

/**
 * scan 0x00 ~ 0x7f (0~127) 
 * retries: 2 times
 * timeout: 10 mS
 */

#ifdef I2C_SCAN

#define I2C_SCAN_TIMEOUT_MS 10
#define I2C_SCAN_RETRIES 2

void i2c_scan() {
  
  uint8_t i, re;

 	PRINTF("\r\nScanning I2C bus:\r\n");

 	for (i = 1; i < 128; i++) {

 	  re = HAL_I2C_IsDeviceReady(&hi2c1, (i << 1),
            I2C_SCAN_RETRIES, I2C_SCAN_TIMEOUT_MS);

 	  if (re == HAL_OK) {
      // received ACK
 		  PRINTF("\r\n0x%02X\r\n", i); 
      continue;
 	  }

    // No ACK received at that address
 		PRINTF(".");
 	}
 	PRINTF("\r\n");
}

#endif /* I2C_SCAN */

/******************************************************************************/
/* Task 1 */

void task_1(void) {

  //PRINTF("Task 1 running!\r\n");
}

/******************************************************************************/
/* OLED SSD1306 */

#ifdef SSD1306_OLED

#define BUFF_SIZE 10

#define Y_OFFSET_LINE_0 0
#define Y_OFFSET_LINE_1 18
#define Y_OFFSET_LINE_2 36

static char line_1_str[BUFF_SIZE] = "Hi STM32  ";
static char line_2_str[BUFF_SIZE] = "          ";
static char line_3_str[BUFF_SIZE] = "          ";

void ssd1306_show(void) {

  ssd1306_Fill(Black);

  ssd1306_SetCursor(2, Y_OFFSET_LINE_0);
  ssd1306_WriteString(&line_1_str[0], Font_11x18, White);

  ssd1306_SetCursor(2, Y_OFFSET_LINE_1);
  ssd1306_WriteString(&line_2_str[0], Font_11x18, White);

  ssd1306_SetCursor(2, Y_OFFSET_LINE_2);
  ssd1306_WriteString(&line_3_str[0], Font_11x18, White);

  ssd1306_UpdateScreen();
}

#else
void ssd1306_show(void) {return;}
#endif



/******************************************************************************/
/* board init function */

void BOARD_Init() {
  PRINTF("\r\nVer: %s-%s\r\n", VERSION, BUILD_DATE);

#ifdef FLASH_INIT
  FLASH_Init();
#endif

#ifdef SSD1306_OLED
  if (ssd1306_Init()) {
    PRINTF("ssd1306 init fail!\r\n");
    i2c_scan();
    //Error_Handler();
  } else {
    ssd1306_show();
  }
#endif
#ifdef AT24C02_EEPROM
  at24c02_Init();
#endif

  HAL_GPIO_WritePin(CONFIG_LED_ERROR_PORT, CONFIG_LED_ERROR_PIN, GPIO_PIN_SET);
  PRINTF("Init Done\r\n");

}

/******************************************************************************/
/* board loop function */

void BOARD_Loop() {

  int tick_task_1 = HAL_GetTick();
  int tick_led = HAL_GetTick();

  while (1)
  {

#ifdef CONFIG_TASK1
    if((HAL_GetTick() - tick_task_1) >= CONFIG_TASK1_TIME_MS) {
      tick_task_1 = HAL_GetTick();
      task_1();
    }
#endif

#ifdef CONFIG_LED_NORMAL
    if((HAL_GetTick() - tick_led) >= CONFIG_LED_NORMAL_BLINK_MS) {
      tick_led = HAL_GetTick();
      HAL_GPIO_TogglePin(CONFIG_LED_NORMAL_PORT, CONFIG_LED_NORMAL_PIN);
    }
#endif

  }
}

