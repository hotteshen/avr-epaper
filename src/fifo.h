#ifndef FIFO_DEF_H_
#define FIFO_DEF_H_

#include <stdint.h>

typedef struct
{
	volatile uint8_t * const data;
	const volatile uint8_t * const data_end;
	volatile uint8_t* volatile wrptr;
	volatile uint8_t* volatile rdptr;
}fifo_t;

#define DEFINE_FIFO(name,size) \
	uint8_t name ## _data[size]; \
	fifo_t name = {name ## _data, name ## _data + size, name ## _data, name ## _data};

#define DEFINE_STATIC_FIFO(name,size) \
		static uint8_t name ## _data[size]; \
		static fifo_t name = {name ## _data, name ## _data + size, name ## _data, name ## _data};

static inline void fifo_clear(fifo_t* fifo)
{
	fifo->wrptr = fifo->data;
	fifo->rdptr = fifo->data;
}

static inline intptr_t fifo_put(fifo_t* fifo,uint8_t byte)
{
	volatile uint8_t *tmwr = fifo->wrptr + 1;
	if(tmwr == fifo->data_end)
		tmwr = fifo->data;
	if (tmwr == fifo->rdptr)
			return 0;
	*tmwr = byte;
	fifo->wrptr = tmwr;
	return 1;
}

static inline intptr_t fifo_get(fifo_t* fifo,uint8_t *byte)
{
	if (fifo->wrptr == fifo->rdptr)
		return 0;
	volatile uint8_t *tmrd = fifo->rdptr + 1;
	if(tmrd == fifo->data_end)
		tmrd = fifo->data;
	*byte = *tmrd;
	fifo->rdptr = tmrd;
	return 1;
}

static inline intptr_t fifo_check(fifo_t* fifo)
{
	int ret = (fifo->wrptr - fifo->rdptr);
	if (ret < 0) ret +=(fifo->data_end - fifo->data);
	return ret;
}

#endif /* FIFO_H_ */
