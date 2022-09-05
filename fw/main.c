/*
 * main.c
 *
 *  Created on: Sep 4, 2022
 *      Author: Korneliusz Osmenda
 */

#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include "WaveInit.h"

#define DC_PORT B
#define DC_PIN 1
#define RST_PORT B
#define RST_PIN 0
#define CS_PORT B
#define CS_PIN 3
#define BUSY_PORT D
#define BUSY_PIN 7

#define Source_Pixel 128
#define Source_Pixel_high Source_Pixel/256  // Source·Ö±æÂÊµÄ¸ß8Î»
#define Source_Pixel_low  Source_Pixel%256  // Source·Ö±æÂÊµÄµÍ8Î»
#define Gate_Pixel 250
#define Gate_Pixel_high   Gate_Pixel/256    // Gate·Ö±æÂÊµÄ¸ß8Î»
#define Gate_Pixel_low    Gate_Pixel%256    // Gate·Ö±æÂÊµÄµÍ8Î»
#define Picture_length    Source_Pixel/8*Gate_Pixel   // 128*250/8=4000  Ò»·ùºÚ°×+Ò»·ùºì ¹²8000

#define PORT_(port) PORT ## port
#define DDR_(port)  DDR  ## port
#define PIN_(port)  PIN  ## port

#define PORT(port) PORT_(port)
#define DDR(port)  DDR_(port)
#define PIN(port)  PIN_(port)

unsigned char LUT_Flag;

void putcu(char c)
{
	/* Wait for empty transmit buffer */
	while (!(UCSR0A & (1<<UDRE0)));
	/* Put data into buffer, sends the data */
	UDR0 = c;
}

void spi_init(void)
{
	/* Set MOSI and SCK output, all others input */
	DDRB = (1<<3)|(1<<5);

	DDR(DC_PORT) |= (1<<DC_PIN);
	DDR(RST_PORT) |= (1<<RST_PIN);
	DDR(CS_PORT) |= (1<<CS_PIN);

	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}

void spi_send_byte(uint8_t byte)
{
	/* Start transmission */
	SPDR = byte;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
}

void waitforready(void)
{
	while(!(PIN(BUSY_PORT)&(1<<BUSY_PIN)));
}

void reset(void)
{
	putcu('G');
	PORT(CS_PORT) |= (1<<CS_PIN);
	PORT(RST_PORT) |= (1<<RST_PIN);
	_delay_ms(10);
	PORT(RST_PORT) &= ~(1UL<<RST_PIN);
	_delay_ms(10);
	PORT(RST_PORT) |= (1<<RST_PIN);
	_delay_ms(10);
	waitforready();
	LUT_Flag=0;
	putcu('H');
}

void writecom(uint8_t data)
{
	PORT(DC_PORT) &= !(1<<DC_PIN);
	PORT(CS_PORT) &= ~(1UL<<CS_PIN);
	spi_send_byte(data);
	PORT(CS_PORT) |= (1<<CS_PIN);
}

void writedata(uint8_t data)
{
	PORT(DC_PORT) |= (1<<DC_PIN);
	PORT(CS_PORT) &= ~(1UL<<CS_PIN);
	putcu('q');
	spi_send_byte(data);
	putcu('w');
	PORT(CS_PORT) |= (1<<CS_PIN);
}

void lut_5S(void)
{
	  unsigned int count;
	  writecom(0x20);
	    for(count=0;count<80;count++)
	      { writedata(lut_R20_5S[count]);  }

	  writecom(0x21);
	    for(count=0;count<56;count++)
	      {writedata(lut_R21_5S[count]);}

	  writecom(0x24);
	    for(count=0;count<80;count++)
	      {writedata(lut_R24_5S[count]);}

	  if (LUT_Flag==0)
	  {
	    writecom(0x22);
	      for(count=0;count<80;count++)
	        {writedata(lut_R22_5S[count]);}
	    writecom(0x23);
	      for(count=0;count<80;count++)
	        {writedata(lut_R23_5S[count]);}
	    LUT_Flag=1;
	  }
	  else
	  {
	    writecom(0x22);
	      for(count=0;count<80;count++)
	        {writedata(lut_R23_5S[count]);}
	    writecom(0x23);
	      for(count=0;count<80;count++)
	        {writedata(lut_R22_5S[count]);}
	    LUT_Flag=0;
	  }
}

void lut_DU(void)
{
  unsigned int count;
  writecom(0x20);
    for(count=0;count<80;count++)
      { writedata(lut_R20_DU[count]);  }

  writecom(0x21);
    for(count=0;count<56;count++)
      {writedata(lut_R21_DU[count]);}

  writecom(0x24);
    for(count=0;count<80;count++)
      {writedata(lut_R24_DU[count]);}

  if (LUT_Flag==0)
  {
    writecom(0x22);
      for(count=0;count<80;count++)
        {writedata(lut_R22_DU[count]);}
    writecom(0x23);
      for(count=0;count<80;count++)
        {writedata(lut_R23_DU[count]);}
    LUT_Flag=1;
  }
  else
  {
    writecom(0x22);
      for(count=0;count<80;count++)
        {writedata(lut_R23_DU[count]);}
    writecom(0x23);
      for(count=0;count<80;count++)
        {writedata(lut_R22_DU[count]);}
    LUT_Flag=0;
  }
}

void send_white(void)
{
	unsigned int row, col;

	for (col = 0; col < Gate_Pixel; col++)
			{
		for (row = 0; row < Source_Pixel / 8; row++)
				{
			writedata(0xff);
		}
	}
}

void send_black(void)
{
	unsigned int row, col;

	for (col = 0; col < Gate_Pixel; col++)
			{
		for (row = 0; row < Source_Pixel / 8; row++)
				{
			writedata(0x00);
		}
	}
}

void init(void)
{
	reset();
	writecom(0x00);
	writedata(0xf7);

	writecom(0x01);
	writedata(0x03);
	writedata(0x00);
	writedata(0x3f);
	writedata(0x3f);
	writedata(0x03);

	writecom(0x03);
	writedata(0x00);

	writecom(0x06);
	writedata(0x17);
	writedata(0x17);
	writedata(0x3e);

	writecom(0x30);
	writedata(0x09);

	writecom(0x60);
	writedata(0x22);

	writecom(0x82);
	writedata(0x0b);

	writecom(0xe3);
	writedata(0x00);

	writecom(0x41);
	writedata(0x00);

	writecom(0x61);
	writedata(Source_Pixel_low);
	writedata(Gate_Pixel_high);
	writedata(Gate_Pixel_low);

	writecom(0x65);
	writedata(0x00);
	writedata(0x00);
	writedata(0x00);

	writecom(0x50);

	writecom(0xb7);

	writecom(0x13); // send data
	send_white();
	lut_5S();
	writecom(0x17); //update
	writedata(0xA5);
	waitforready();
	_delay_ms(1000);
	writecom(0x50);
	writedata(0xD7);
}

void uart_init(void)
{
	DDRD |=(1<<1);
	UCSR0A =0;

	#define BAUD 9600
	#include <util/setbaud.h>
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
	#if USE_2X
	UCSR0A |= (1UL << U2X0);
	#else
	UCSR0A &= ~(1UL << U2X0);
	#endif

	UCSR0B = (1 << TXEN0);
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);


}

int main(void)
{
	spi_init();
	DDR(BUSY_PORT) &= ~(1UL<<BUSY_PIN);

	uart_init();
	putcu('A');
	init();
	putcu('B');

	while(1)
	{
		putcu('C');

		writecom(0x13); // send data
		send_white();
		lut_DU();
		writecom(0x17); //update
		writedata(0xA5);
		waitforready();
		_delay_ms(10000);
		putcu('D');

		writecom(0x13); // send data
		send_black();
		lut_DU();
		writecom(0x17); //update
		writedata(0xA5);
		waitforready();
		_delay_ms(10000);

	}

	return 0;
}
