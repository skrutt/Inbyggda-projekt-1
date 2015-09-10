/*
 * super_paketet.c
 *
 * Created: 2015-09-10 13:28:36
 *  Author: peol0071
 */ 
#include "super_paketet.h"
#include "Usartlib.h"

#define PREAMBLE 0x55FF
#define package_size sizeof(super_paketet)

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
	static char buffer[package_size + 1];
	//static uint8_t		counter = 0;
	super_paketet *package = (super_paketet *)buffer;
	super_paketet *new_package = (super_paketet *)(buffer + 1);
	
	//move into last place in buffer
	buffer[package_size] = incomming_byte;
	//counter++;
	
	//Check buffer
// 	if (counter >= package_size)
// 	{
// 		for (int i = 1; i < counter; i++)
// 		{
// 			//move along, and drop oldest byte
// 			buffer[i - 1] = buffer[i];
// 			//*buffer = *++buffer;
// 		}
// 	}
// 	
	//Copy new content
	*package = *new_package;
	//check for package
	
	
	if (package->adress == ADRESS)
	{
		//Woo paket!
		//Crc osv
		if (package->crc == do_crc((uint8_t*)package, package_size - 1))
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
	const int outgoing_data_length = sizeof(super_paketet) + 2;
	
	char outgoing_data[outgoing_data_length]; // package + preamble
	
	*(uint16_t*)outgoing_data = PREAMBLE;
	*(super_paketet*)(outgoing_data + 2) = outgoing_package;
	
	send_string(outgoing_data, outgoing_data_length);
}