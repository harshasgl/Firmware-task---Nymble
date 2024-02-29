/*Header files*/
#include<stdio.h>		//standard input/output
#include<stdlib.h>		//memory allocation
#include<string.h>		//string manipulation
#include<windows.h>		//windows specific header file

/*Macro definitions*/
#define BAUD 2400		//defining Baud rate
#define DATA_SIZE 1000	//defining the size of the data which is to be transmitted or received

/*function to print data transmission speed*/
void Speed(int datasize, double seconds)
{
	/*calculating total bits transmitted*/
	int bits_transmitted = datasize * 8;
	double speed = bits_transmitted / seconds;
	printf("Data transmission speed: %f bits/second\n", speed);
}

int main()
{
	/*Variable declaration*/
	HANDLE hSerial;				//handle for serial communication
	
	/*Structure to configure the parameters of serial ports like baud rate, byte size, stop bits and parity*/
	/*Initializing all the structure members to zero*/
	DCB Serial_parameters = { 0 };

	/*It contains timeout values used in communication operation*/
	/*Initializing all the members of structure to zero*/
	COMMTIMEOUTS comm_timeouts = { 0 };

	/*create file function to open a handle to serial port COM3 for reading and writing */
	hSerial = CreateFile( "COM3", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
	/*check if the serial port is opened or not*/
	if(hSerial == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "Error opening the serial port\n");
		return 1;
	}

	/*Making the DCBlength member equal to the size of the structure*/
	Serial_parameters.DCBlength = sizeof(Serial_parameters);
	/*condition to check return valve of "GetCommState" function*/
	if(GetCommState(hserial, &Serial_parameters) == 0)
	{
		fprintf(stderr, "Error getting devide state\n");
		CloseHandle(hSerial);
		return 1;
	}

	/*Setting the desired parameters for serial communication*/
	Serial_parameters.BaudRate = BAUD;
	Serial_parameters.Bytesize = 8;
	Serial_parameters.StopBits = ONESTOPBIT;
	Serial_parameters.Parity = NOPARITY;		//no parity checking is used

	/*Condition to check the return value of "SetCommState" function*/
	if(SetCommState(hSerial, &Serial_parameters) == 0)
	{
		fprintf(stderr, "Error setting device parameters\n");
		CloseHandle(hSerial);		//closing the handle
		return 1;
	}

	/*timeout settings for read and write operations*/
	comm_timeouts.ReadIntervalTimeout = 50;
	comm_timeouts.ReadTotalTimeoutConstant = 50;
	comm_timeouts.ReadTotalTimeoutMultiplier = 10;
	comm_timeouts.WriteTotalTimeoutConstant = 50;
	comm_timeouts.WriteTotalTimeoutMultiplier = 10;

	if(SetCommTimeouts(hSerial, &comm_timeouts) == 0)
	{
		fprintf(stderr, "Error setting timeouts\n");
		CloseHandle(hSerial);		//closing the handle
		return 1;
	}


	/*Opening the file text.txt for reading the contents into buffer*/
	/*Opening the text file read mode*/
	FILE *fptr = fopen("text.txt", "r");
	/*Check if the file opened successfully*/
	if(fptr == NULL)
	{
		fprintf(stderr, "fopen failed\n");
		CloseHandle(hSerial);		//closing the handle
		return 1;
	}
	/*declaring a character array to store the data*/
	char buffer[DATA_SIZE];
	/*reading the contents into the buffer*/
	fread(buffer, sizeof(char), DATA_SIZE, fptr);
	/*closing the file*/
	fclose(fptr);


	DWORD bytes_written;	//unsigned 32-bit integer
	/*this function retrives the current system date and time*/
	GetSystemTimeAsFileTime((FILENAME*)&bytes_written);

	if(WriteFile(hSerial, buffer, DATA_SIZE, &bytes_written, NULL))
	{
		printf("Text data sent to MCU successfully\n");
		
		/*declaring a character arry for soring the receiving data*/
		char receive_data[DATA_SIZE];

		DWORD bytes_read;
		GetSystemTimeAsFileTime((FILETIME*)*bytes_read);
		
		/*Reading data from the serial portr into the "receive_data" buffer*/
		if(ReadFile(hSerial, receive_data, DATA_SIZE, &bytes_read, NULL))
		{
			/*printing the received data*/
			printf("Data received from MCU is: \n%s\n", receive_data);
		}
		else
		{
			fprintf(stderr, "Error reading from serial port\n");
			return 1;
		}

		GetSystemTimeAsFileTime((FILETIME*)&bytes_written);
		double seconds = (double)(bytes_written - bytes_read) / 10000000.0;
		Speed(DATA_SIZE, seconds);		//function call to print data transmission speed in bits/second

	}
	else
	{
		fprintf(stderr, "Error writing into the serial port\n");
	}

	CloseHandle(hserial);		//closing the handle
	return 1;
}
