
#include "at24c02.h"

#ifdef AT24C02_DEBUG
#define CPRINTF(format, args...) PRINTF("AT24C02: " format, ##args)
#else
#define CPRINTF(format, args...) 
#endif

static int ina3221_set_reg_pointer(uint8_t reg)
{
	uint8_t re;

	re = HAL_I2C_Master_Transmit(&hi2c1, (AT24C02_I2C_ADDR << 1),
						&reg, sizeof(reg), 1000);

	if (re != HAL_OK)
		CPRINTF("setup reg pointer fail! (%d)", re);

	return re;
}

int at24c02_read(uint8_t reg, uint8_t *data, int size) {

	int re;

	/* set offset*/
	ina3221_set_reg_pointer(reg);

	re = HAL_I2C_Master_Receive(&AT24C02_I2C_PORT, (AT24C02_I2C_ADDR << 1),
			data, size, 1000);

	if (re != HAL_OK)
		CPRINTF("read failed\n\r");

	return re;
}

int at24c02_write(uint8_t reg, uint8_t *data, int size) {

	int i, re;
	uint8_t data_write[size + 1];

	data_write[0] = reg;

	for (i = 1; i < (size + 1); i++)
		data_write[i] = data[i - 1];

	re = HAL_I2C_Master_Transmit(&AT24C02_I2C_PORT, (AT24C02_I2C_ADDR << 1),
			data_write, sizeof(data_write), 1000);

	return re;
}

void at24c02_dump(uint8_t size) {

	uint8_t v;
	int i;

	CPRINTF("dump:");

	for (i = 0; i < size; i++) {

		if (i % 8 == 0)
			PRINTF("\r\n0x%02x: ", i);

    		if (at24c02_read(i, &v, sizeof(v)) != HAL_OK)
      			PRINTF("\r\n 0x%2x read fail\r\n", i);
    		else
      			PRINTF("%2x ", v);
	}
	PRINTF("\r\n");
}

static int at24c02_read_write_test(void) {

	int i;
	uint8_t reg = 0x00;
	uint8_t buf[] = {0x53, 0x54, 0x4d, 0x33, 0x32};
	uint8_t buf_read[sizeof(buf)];

	CPRINTF("read_write_test\r\n");

	CPRINTF("reg: %02x, data: ", reg);
	for (i = 0; i < sizeof(buf); i++)
		PRINTF("%02x ", buf[i]);
	PRINTF("\r\n");

	at24c02_write(reg, buf, sizeof(buf));
	HAL_Delay(100);
	at24c02_read(reg, buf_read, sizeof(buf_read));

	for (i = 0; i < sizeof(buf); i++) {
		if (buf[i] != buf_read[i]) {
			CPRINTF("reg: %02x, test fail: %02x %02x\n\r",
					i, buf[i], buf_read[i]);
			return HAL_ERROR;
		}
	}

	CPRINTF("test pass\r\n");
	return HAL_OK;
}

/**
 * return 0 ,if AT24C02 no respond
 * return 1 ,if AT24C02 exist
 */
int is_at24c02_ready(void)
{
	return !(HAL_I2C_IsDeviceReady(&AT24C02_I2C_PORT,
				(AT24C02_I2C_ADDR << 1), 3, 10));
}

int at24c02_Init(void)
{
	CPRINTF("init\r\n");

	if (!is_at24c02_ready()) {
		CPRINTF("Can not found AT24C02!\r\n");
		return HAL_ERROR;
	}

#ifdef AT24C02_SELF_TEST
	if (at24c02_read_write_test()) {
		return HAL_ERROR;
	}
#endif
	at24c02_dump(16);
	return HAL_OK;
}
