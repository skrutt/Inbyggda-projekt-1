//
/*
 * Lib for serial communication
 * Petter Olofsson 2015
 *
 * Created: 2015-08-31 13:24:59
 *  Author: peol0071
 */ 

#include <avr/io.h>
#include "avr/interrupt.h"
//#include "Usartlib.h"

//TX and RX buffer initializatoin
#define USART_TX_BUFFER_SIZE 100
volatile char		outgoing_data[USART_TX_BUFFER_SIZE];
volatile uint8_t	outgoing_data_counter = 0;
volatile char*		outgoing_data_head = outgoing_data;

#define USART_RX_BUFFER_SIZE 100
volatile char		incomming_data[USART_TX_BUFFER_SIZE];
volatile uint8_t	incomming_data_counter = 0;
volatile char*		incomming_data_head = incomming_data;

#define get_lock()		cli()
#define release_lock()	sei()

//Usart transmit interrupt
//This interrupt is always called when UDR is empty
ISR(USART_UDRE_vect)
{
	// check for empty transmit buffer	and if we have data
	if ( 0 != outgoing_data_counter)
	{
		outgoing_data_counter--;
		UDR0 = *outgoing_data_head; 		// start transmition
		outgoing_data_head++;
		if (outgoing_data_head >= (outgoing_data + USART_TX_BUFFER_SIZE))
		{
			outgoing_data_head = outgoing_data;	//reset ring buffer
		}
	}else{
		//Inactivate interrupt
		UCSR0B &= ~(1 << UDRIE0);
		
	}
}

//Usart receive interrupt
ISR(USART_RX_vect)
{
	//place char into buffer
	*incomming_data_head = UDR0;	//Read from UDR gets RX byte
	
	//increase head
	incomming_data_head++;
	
	
	//Check for overflow head
	if (incomming_data_head >= (incomming_data + USART_RX_BUFFER_SIZE))
	{
		//End of ring, go around
		incomming_data_head = incomming_data;
	}
	//Check for overflow counter
	if(incomming_data_counter < USART_RX_BUFFER_SIZE)
	{
		incomming_data_counter++;
	}
}

/* Read and write functions */
char ReceiveByte(char * result)
{
	get_lock();
	//If we have data
	if (incomming_data_counter > 0)
	{
		//Assign pointer to data 
		//Read buffer in reverse because we add positive
		volatile char * temp = incomming_data_head - incomming_data_counter;
		
		//Check pointer for overflow
		if (temp < incomming_data )
		{
			temp += USART_RX_BUFFER_SIZE;	//Wrap around
		}
		//Decrease counter
		incomming_data_counter--;

		//return that data
		*result = *temp;
		release_lock();
		
		return 1;
	} 
	else
	{
		//Else fail
		release_lock();
		return 0;								// return the data
	}
}
char ReceiveByteBlocking()
{
	while (incomming_data_counter == 0);	//Busy wait
	char temp;
	ReceiveByte(&temp);
	return temp;
}

void send_c(char c)
{
	get_lock();
	//Check if there is space in the buffer
	if (outgoing_data_counter < USART_TX_BUFFER_SIZE)
	{
		volatile char * temp = (outgoing_data_head + outgoing_data_counter);
	
		//Check for overflow
		if (temp >= (outgoing_data + USART_TX_BUFFER_SIZE))
		{
			temp -= USART_TX_BUFFER_SIZE;
		}

		*temp = c;		//add to queue
		outgoing_data_counter++;
		//Activate TX interrupt
		UCSR0B |= (1 << UDRIE0);
	}
	release_lock();
}

int8_t send_string(char data[], uint8_t length)
{
	if (data == 0)	//Check for valid pointer
	{
		return -1;
	}
	//Check for space in buffer
	if ((length + outgoing_data_counter )>= USART_TX_BUFFER_SIZE)
	{
		return -2;
	}
	//All good, add to buffer
	for (uint8_t i = 0; i < length; i++ )
	{
		send_c(data[i]);
	}
	
	return 0;
} 

void flush_usart()
{
	volatile static int dummy = 0;
	//Wait for empty buffer
	while(//Activate TX interrupt
	UCSR0B & (1 << UDRIE0) )
	{
		dummy++;
		
	}
	//_delay_ms(1);
}

int8_t send_string_blocking(char data[], uint8_t length)
{

	int8_t ret = 0;
		
	//Wait for empty buffer
	flush_usart();
	
	//send data
	ret = send_string(data, length);
	
	//Wait until empty again
	flush_usart();
	
	return ret;
}
