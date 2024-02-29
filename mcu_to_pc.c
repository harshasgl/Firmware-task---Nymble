/*Header files*/
#include<avr/io.h>  		//provides access to input output registers
#include<avr/interrupt.h> 	//for interrupt related functions
#include<avr/eeprom.h>		//EEPROM operations
#include<util/delay.h>		//for creating delays

/*defining macros*/
#define BAUD 2400
#define DATA_SIZE 1000
#define CPU_FREQ 16000000UL		//cpu frequency
#define EEPROM 0x00		//starting address of EEPROM

/*function prototypes*/
void UART_initialization();
void receive_data_from_pc();
void transmit_data_to_pc();
uint8_t UART_receiveByte();
void UART_sendByte(uint8_t data);

int main()
{
	UART_initialization();		//UART 	initialization
	receive_data_from_pc();		//function call to receive data from pc and storing in EEPROM
	transmit_data_to_pc();		//function call to trasmit data stored in EEPROM to pc
	return 0;
}

void UART_initialization()
{
	/*UBRR = (CPU freqency / (16 * BAUD RATE) - 1)*/
	UBRR0H = (uint8_t)(CPU_FREQ / (16UL * BAUD) - 1) >> 8;		//high byte
	UBRR0L = (uint8_t)(CPU_FREQ / (16UL * BAUD) - 1);			//low byte

	/*Enable receiver and transmitter*/
	UCSR0B |= (1 << RXENO) | (1 << TXENO);		//initialize UART for serial communication
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);	//setting frame format: 8-bit data, 2 stop bit
}

void receive_data_from_pc()
{
	uint16_t i;		//unsigned 16-bit  integer
	for(i = 0 ; i < DATA_SIZE ; i++)
	{
		eeprom_write_byte((uint8_t *)(EEPROM + i), UART_receiveByte());
		//_delay_ms(2);	//2ms delay(if needed)
	}
}
uint8_t UART_receiveByte()
{
	while(!(UCSR0A & (1 << RXC0)));		//wait for incoming data
	return UDR0;	//UART data register whihc holds the received data
}

void transmit_data_to_pc();
{
	uint16_t i;
	for(i = 0 ; i < DATA_SIZE ; I++)
	{
		UART_sendByte(eeprom_read_byte((uint8_t *)(EEPROM + i)));
		//_delay_ms(2); //2ms delay(if needed)
	}
}
void UART_sendByte(uint8_t data)
{
	while(!(UCSR0A & (1 << UDRE0)));	//wait for empty transmitb buffer
	UDR0 = data;	//writes the value of the variable data to UART data register
}
