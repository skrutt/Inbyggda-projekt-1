/*
 * super_paketet.c
 *
 * Created: 2015-09-10 13:28:36
 *  Author: peol0071
 */ 
#include "super_paketet.h"
#include "Usartlib.h"
#include <util/delay.h>

#define PREAMBLE 0x55FF
#define PACKAGE_SIZE sizeof(super_paketet)

//function pointers for setting link mode
void (*enable_transmit_func)();
void (*disable_transmit_func)();

//Init function for controlling radio module
void set_link_mode_functions(void (*enable_transmit), void (*disable_transmit))
{
	enable_transmit_func = enable_transmit;
	disable_transmit_func = disable_transmit;
}
//Wrapper
inline void set_link_mode_transmit()
{
	if (enable_transmit_func)
	{
		enable_transmit_func();
	}
}
//Wrapper
inline void set_link_mode_receive()
{
	if (disable_transmit_func)
	{
		disable_transmit_func();
	}
}


void InitUART( unsigned int baud )
{
	int baudfactor = (F_CPU/16/baud-1);
	UBRR0H = (unsigned char)(baudfactor>>8);	//set the baud rate
	UBRR0L = (unsigned char)baudfactor;
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);			//enable UART receiver and transmitter
	/* Set frame format: 8data */
	UCSR0C = (3<<UCSZ00);
	//Activate interrupts
	UCSR0B |= (1 << UDRIE0) + (1 << RXCIE0);
}


//make crc checksum
uint8_t do_crc(uint8_t in_data[], uint8_t length)
{
	uint8_t ret = 0;
	for (int i = 0; i < length; i++)
	{
		//xor
		ret ^= in_data[i];
	}
	return ret;
}

//Call from isr
super_paketet process_data_for_package(char incomming_byte)
{
	static char buffer[PACKAGE_SIZE + 1];
	//static uint8_t		counter = 0;
	super_paketet *package = (super_paketet *)buffer;
	super_paketet *new_package = (super_paketet *)(buffer + 1);
	
	//move into last place in buffer
	buffer[PACKAGE_SIZE] = incomming_byte;
	//counter++;
	
	//Copy new content
	*package = *new_package;
	//check for package
	
	
	if (package->adress == ADRESS)
	{
		//Woo paket!
		//Crc osv
		if (package->crc == do_crc((uint8_t*)package, PACKAGE_SIZE - 1))
		{
			return *package;
		}
	}
	super_paketet fail_return;
	fail_return.adress = 0;
	
	return fail_return;
}

//Fetch new packages if there is one
super_paketet check_for_package()
{
	char incomming_byte;
	//Check if we have data
	while (ReceiveByte(&incomming_byte))
	{
		super_paketet incomming_package;
		//Send new data for processing
		incomming_package = process_data_for_package(incomming_byte);
		//Check if we got a package
		if (incomming_package.adress != 0)
		{
			return incomming_package;
		}
	}
	super_paketet fail_return;
	fail_return.adress = 0;
	
	return fail_return;
}
//pad package and send over usart
void send_package(super_paketet outgoing_package)
{
	outgoing_package.crc = do_crc((uint8_t*)&outgoing_package, PACKAGE_SIZE-1);
	const int outgoing_data_length = sizeof(super_paketet) + 2;
	
	char outgoing_data[outgoing_data_length]; // package + preamble
	
	*(uint16_t*)outgoing_data = PREAMBLE;
	*(super_paketet*)(outgoing_data + 2) = outgoing_package;
	
	send_string(outgoing_data, outgoing_data_length);
}

//Send package that requires an answer
//Append request bit to type and wait for answer
//return 0 if we got answer
int8_t send_request_package(super_paketet *outgoing_package, int timeout)
{
	super_paketet internal_package = *outgoing_package;
	//Pad with request bit
	internal_package.type |= REQUEST_TYPE;	

	//Send to buffer
	send_package(internal_package);
	
	//Wait for buffer to be empty
	flush_usart();
	
	//Toggle transmit pin to listen
	set_link_mode_receive();
	
	//Wait for package for some time
	for (int i = 0; i < timeout; i++)
	{
		//Check if we have got a package
		internal_package = check_for_package();
		//is it what we want?
		if (internal_package.adress != 0 && internal_package.type == outgoing_package->type)
		{
			//Woo package!
			*outgoing_package = internal_package;
			//Return success
			return 0;
			
		}
		_delay_ms(1);
	}
	
	//Toggle pin back
	set_link_mode_transmit();
	
	//Return fail
	return -1;

}
