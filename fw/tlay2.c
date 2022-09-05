#include "tlay2.h"
#include "uart.h"

#include <stdint.h>
#include <avr/eeprom.h>
#include <util/crc16.h>

static uint8_t tlay2_rx_buf[32];
static uint8_t tlay2_rx_len;
static uint8_t tlay2_rx_present;
static uint8_t tlay2_tx_crc;

void tlay2_init()
{
	uart_init();
	tlay2_rx_present = 0;
}

uint8_t * tlay2_get_data()
{
	return tlay2_rx_buf;
}

uint8_t tlay2_get_len()
{
	return tlay2_rx_len;
}

uint8_t tlay2_checkslave()
{
	if(!tlay2_check())
		return 0;
	return 1;
}

uint8_t tlay2_check()
{
	if(tlay2_rx_present)
		return 1;
	if (uart_getnbytes())
	{
		tlay2_rx_len=0;
		uint8_t crc=0x00;
		uint8_t c;
		do {
			uart_get_byte(&c);
			if (c == '\n')
				break;
			if (c == 0xdc)
			{
				uart_get_byte(&c);
				c ^= 0x80;
			}
			tlay2_rx_buf[tlay2_rx_len++]=c;
			crc = _crc8_ccitt_update(crc,c);

		}while(tlay2_rx_len < 32);
		tlay2_rx_len-=1;
		if(c != '\n')
			return 0;
		if (tlay2_rx_len < 2)
			return 0;
		if (crc != 0)
			return 0;
		tlay2_rx_present = 1;
		return 1;
	}
	return 0;
}

void tlay2_reset()
{
	tlay2_rx_present=0;
}

void tlay2_tx_init_reply()
{
	tlay2_tx_crc=0x00;
	tlay2_tx_byte(tlay2_rx_buf[0]);
	tlay2_tx_byte(tlay2_rx_buf[1]);
	tlay2_tx_byte(tlay2_rx_buf[2]);
	tlay2_tx_byte(tlay2_rx_buf[3]);
}

void tlay2_tx_init()
{
	tlay2_tx_crc=0x00;
}

void tlay2_tx_byte(uint8_t byte)
{
	tlay2_tx_crc=_crc8_ccitt_update(tlay2_tx_crc,byte);
	if (byte == '\n' || byte == 0xdc)
	{
		uart_send_byte(0xdc);
		byte ^=0x80;
	}
	uart_send_byte(byte);
}

void tlay2_tx_u16(uint16_t byte)
{
	tlay2_tx_byte(byte&0xff);
	tlay2_tx_byte(byte >> 8);
}

void tlay2_tx_u32(uint32_t byte)
{
	tlay2_tx_byte(byte&0xff);
	tlay2_tx_byte((byte >> 8)&0xff);
	tlay2_tx_byte((byte >> 16)&0xff);
	tlay2_tx_byte((byte >> 24)&0xff);
}

void tlay2_tx_end()
{
	tlay2_tx_byte(tlay2_tx_crc);
	uart_send_byte('\n');
}

uint16_t tlay2_rx_u16(uint8_t * buff)
{
	return ((uint16_t)buff[1] << 8) | buff[0];
}

uint32_t tlay2_rx_u32(uint8_t * buff)
{
	return ((uint32_t)buff[3] << 24) | ((uint32_t)buff[2] << 16) | ((uint32_t)buff[1] << 8) | buff[0];

}
