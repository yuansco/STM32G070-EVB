
#ifndef __AT24C02_H__
#define __AT24C02_H__

#include "board.h"
#include "main.h"

//#define AT24C02_DEBUG
//#undef AT24C02_DEBUG

//#define AT24C02_SELF_TEST

#define AT24C02_ADDR_0    0x50    // A0, A1 and A2 connected to GND
#define AT24C02_DATA_SIZE  256    // size of AT24C02


extern I2C_HandleTypeDef hi2c1;

#ifndef AT24C02_I2C_PORT
#define AT24C02_I2C_PORT hi2c1
#endif

#ifndef AT24C02_I2C_ADDR
#define AT24C02_I2C_ADDR AT24C02_ADDR_0
#endif

int at24c02_read(uint8_t reg, uint8_t *data, int size);
int at24c02_write(uint8_t reg, uint8_t *data, int size);
void at24c02_dump(uint8_t size);
int is_at24c02_ready(void);
int at24c02_Init(void);

#endif // __AT24C02_H__