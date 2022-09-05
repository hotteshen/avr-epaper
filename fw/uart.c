
#include "uart.h"
#include "fifo.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

//internal use macro
#define CONCAT(a, b)       a ## b
#define CONCAT3(a, b, c)   a ## b ## c

#define RX_PORT D
#define RX_PIN 1

#define UART_TXC_VECTOR  USART_TX_vect
#define UART_RXC_VECTOR  USART_RX_vect
#define UART_UDRE_VECTOR USART_UDRE_vect

#define COMPORTNo 0

//register of PORT and bit define
#define PORTREG(No)        CONCAT(PORT, No)
#define PINREG(No)         CONCAT(PIN, No)
#define UDRREG(No)         CONCAT(UDR, No)
#define DDRREG(No)         CONCAT(DDR, No)
#define TXCBIT(No)         CONCAT(TXC, No)
#define RXCBIT(No)         CONCAT(RXC, No)
#define RXENBIT(No)        CONCAT(RXEN, No)
#define TXENBIT(No)        CONCAT(TXEN, No)
#define URSELBIT(No)       CONCAT(URSEL, No)
#define RXCIEBIT(No)       CONCAT(RXCIE, No)
#define TXCIEBIT(No)       CONCAT(TXCIE, No)
#define UDRIEBIT(No)       CONCAT(UDRIE, No)

//comport register
#define UBRRHREG(No)       CONCAT3(UBRR, No, H)
#define UBRRLREG(No)       CONCAT3(UBRR, No, L)
#define UCSRAREG(No)       CONCAT3(UCSR, No, A)
#define UCSRBREG(No)       CONCAT3(UCSR, No, B)
#define UCSRCREG(No)       CONCAT3(UCSR, No, C)
#define UCSZBIT(No1, No2)  CONCAT3(UCSZ, No1, No2)
#define U2XBIT(No)         CONCAT(U2X, No)

DEFINE_STATIC_FIFO(rx_fifo,128);
DEFINE_STATIC_FIFO(tx_fifo,128);

static volatile uint8_t ncount;

void uart_init()
{
	PORTREG(RX_PORT) |= (1 << RX_PIN);
	UCSRAREG(COMPORTNo) = 0;
	UCSRBREG(COMPORTNo) = (1 << RXENBIT(COMPORTNo))|
			(1 << TXENBIT(COMPORTNo))|
			(1 << TXCIEBIT(COMPORTNo))|
			(1 << RXCIEBIT(COMPORTNo))|
			(1 << UDRIEBIT(COMPORTNo));
	UCSRCREG(COMPORTNo) = (1 << UCSZBIT(COMPORTNo, 1))|
			(1 << UCSZBIT(COMPORTNo, 0));
//#define BAUD 62500
#define BAUD 38400
#include <util/setbaud.h>
	UBRRHREG(COMPORTNo) = UBRRH_VALUE;
	UBRRLREG(COMPORTNo) = UBRRL_VALUE;
	#if USE_2X
	UCSRAREG(COMPORTNo) |= (1 << U2XBIT(COMPORTNo));
	#else
	UCSRAREG(COMPORTNo) &= ~(1UL << U2XBIT(COMPORTNo));
	#endif
}

intptr_t uart_get_byte(uint8_t *byte)
{
	intptr_t ret;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
    	if((ret=fifo_get(&rx_fifo,byte)))
    	{
    		if(*byte == '\n')
    			ncount--;
    	}
    }
	return ret;
}

intptr_t uart_send_byte(uint8_t byte)
{
	intptr_t ret;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
    	ret = fifo_put(&tx_fifo, byte);
		UCSRBREG(COMPORTNo) |= (1 << UDRIEBIT(COMPORTNo));
	}
	return ret;
}

uint8_t uart_getnbytes()
{
	uint8_t ret;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
    	ret=ncount;
	}
	return ret;
}

ISR(UART_TXC_VECTOR)
{
}

ISR(UART_RXC_VECTOR)
{
	uint8_t c;
	c = UDRREG(COMPORTNo);
	fifo_put(&rx_fifo,c);
	if(c == '\n')
		ncount++;
}

ISR(UART_UDRE_VECTOR)
{
	uint8_t c;
	if (fifo_get(&tx_fifo,&c))
	{
		UDRREG(COMPORTNo) = c;
	}
	else
	{
		UCSRBREG(COMPORTNo) &= ~(1UL << UDRIEBIT(COMPORTNo));
	}
}
