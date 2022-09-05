#ifndef TLAY2_TLAY2_H
#define TLAY2_TLAY2_H

#include <stdint.h>

void tlay2_init(void);
uint8_t * tlay2_get_data(void);
uint8_t tlay2_get_len(void);

uint8_t tlay2_check(void);
uint8_t tlay2_checkslave(void);
void tlay2_reset(void);

void tlay2_tx_init_reply(void);
void tlay2_tx_init(void);
void tlay2_tx_byte(uint8_t byte);
void tlay2_tx_u16(uint16_t byte);
void tlay2_tx_u32(uint32_t byte);
void tlay2_tx_end(void);

uint16_t tlay2_rx_u16(uint8_t * buff);
uint32_t tlay2_rx_u32(uint8_t * buff);

#endif
