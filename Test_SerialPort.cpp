//=================================================================================================
// Name        : Test_SerialPort.cpp
// Author      : vmoreno
// Date		   : 24/12/2021
// Version     : 1.0
// Description : Testing application, configure serial port to receive data and send back.
//               Configuration is entered as a command line parameters.
//=================================================================================================

/****************************************** INCLUDES *********************************************/
#include <iostream>
#include <atomic>
#include <string>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "SerialPort.h"

/****************************************** DEFINES  *********************************************/
constexpr char VERSION[] = "v1.0";			/**< Version of sofware */
std::atomic<bool> run {true};				/**< Flag to keep running the program */
static void usage(const char *nameApp);		/**< Print usage of software */
static void signalHandler(int signum);		/**< Handler of signals */
static void printVersion(void);				/**< Print version of software */
static Parity_e parseParity(char parity); 	/**< Parse parity */


/**************************************************************************************************
 ******************************************* FUNCTION *********************************************
 *************************************************************************************************/
int main(int argc, char *argv[])
{
	char buffer[1000];	/**< Buffer to store data */

	/** Values to set-up the serial port */
	BaudRate_e eBaudRate;
	Parity_e eParity;
	int	bits;
	bool twoStopBits;

	/** Signal handler */
	signal(SIGTERM, signalHandler);
	signal(SIGINT, signalHandler);
	signal(SIGKILL, signalHandler);

	/** Parse commands */
	if ((argc != 2) && (argc != 4))
	{
		/** Print usage */
		usage(reinterpret_cast<const char *>(argv[0]));
		return -1;
	}
	else if (argc == 2) {
		/** Check if ask for version */
		if ((strcmp(argv[1], "-v") == 0) || (strcmp(argv[1], "--version") == 0)) {
			/** Print version */
			printVersion();
			return 0;
		}
		/** Check if ask for help */
		else if ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0)) {
			/** Print usage */
			usage(reinterpret_cast<const char *>(argv[0]));
			return 0;
		}
		else {
			/** Print usage */
			usage(reinterpret_cast<const char *>(argv[0]));
			return -1;
		}
	}
	else {
		/** Check baud rate is within allow */
		char *pNum;
		long baud = strtol(argv[2], &pNum, 10);
		switch (baud) {
		case 9600:
			eBaudRate = eBaudRate_9600;
			break;
		case 19200:
			eBaudRate = eBaudRate_19200;
			break;
		case 38400:
			eBaudRate = eBaudRate_38400;
			break;
		case 57600:
			eBaudRate = eBaudRate_57600;
			break;
		case 115200:
			eBaudRate = eBaudRate_115200;
			break;
		default:
			std::cout << "BAUD RATE NOT SUPPORTED" << std::endl;
			usage(reinterpret_cast<const char *>(argv[0]));
			return -1;
		}

		/** Check data bits, parity and stop bits */
		if (strlen(argv[3]) == 3) {
			/** Data bits */
			if ((argv[3][0] != '7') && (argv[3][0] != '8')) {
				std::cout << "DATA BITS NOT SUPPORTED" << std::endl;
				usage(reinterpret_cast<const char *>(argv[0]));
				return -1;
			}
			/** Parity */
			else if ((argv[3][1] != 'N') && (argv[3][1] != 'O') && (argv[3][1] != 'E')) {
				std::cout << "PARITY NOT SUPPORTED" << std::endl;
				usage(reinterpret_cast<const char *>(argv[0]));
				return -1;
			}
			/** Stop bit */
			else if ((argv[3][2] != '1') && (argv[3][2] != '2')) {
				std::cout << "STOP BITS NOT SUPPORTED" << std::endl;
				usage(reinterpret_cast<const char *>(argv[0]));
				return -1;
			}
			/** All was OK, parse data */
			else {
				bits = (argv[3][0] == '7') ? 7 : 8;		/**< Set data bits */
				eParity = parseParity(argv[3][1]);		/**< Set parity */
				twoStopBits = (argv[3][2] == '2') ? true : false;	/**< Set stop bits */
			}
		}
		else {
			usage(reinterpret_cast<const char *>(argv[0]));
			return -1;
		}
	}

	/** Print version */
	printVersion();

	/** Starting TEST */
	std::cout << ":::::::::::::::::: TEST STARTED  :::::::::::::::::" << std::endl;

	SerialPort serialPort;	/**< Serial port to used */

	/** Open port */
	if (serialPort.Open(argv[1], eBaudRate, eParity, bits, twoStopBits) == -1) {
		/** Error opening serial port */
		std::cout << ":: ERROR OPENING SERIAL PORT, UNABLE TO TEST IT ::" << std::endl;
		std::cout << ":::::::::::::::::: TEST ABORTED ::::::::::::::::::" << std::endl;
		serialPort.Close();	/**< In case was open, but not set-up */
		return -1;
	}

	/** Receive data and transmit back */
	while(run) {
		int bytesReceived;	/**< Bytes received */

		/** Receive data */
		if ((bytesReceived = serialPort.ReadData((void *)buffer, sizeof(buffer))) < 0) {
			if (errno != EAGAIN) {
				std::cout << "::::::::::: ERROR READING SERIAL PORT  :::::::::::" << std::endl;
			}
		}
		/** Check if data was received */
		else if (bytesReceived > 0) {
			/** Print message received */
			std::cout << ":::::::::::::::::: DATA RECEIVED :::::::::::::::::" << std::endl;
			printf("Data: %s\n", buffer);
			std::cout << "::::::::::::::::::::::::::::::::::::::::::::::::::" << std::endl;		
			/** If no error happens, transmit the same data */
			if (serialPort.WriteData((void *)buffer, bytesReceived) < 0) {
				std::cout << "::::::::::: ERROR WRITING SERIAL PORT ::::::::::::" << std::endl;
			}
		}
	}

	/** Close port */
	serialPort.Close();

	/** Finish TEST */
	std::cout << ":::::::::::::::::: TEST FINISHED :::::::::::::::::" << std::endl;

	return 0;
}

/**************************************************************************************************
 ******************************************* FUNCTION *********************************************
 *************************************************************************************************/
void signalHandler(int signum)
{
	/** Catch signal */
	run = false;
}

/**************************************************************************************************
 ******************************************* FUNCTION *********************************************
 *************************************************************************************************/
void usage(const char *nameApp)
{
	/** Print usage of application to the user */
	std::cout << "Usage: " << nameApp << " arg1 arg2 arg3" << std::endl;
	std::cout << "\t arg1: Path of the serial port" << std::endl;
	std::cout << "\t arg2: Baud rate (9600, 19200, 38400, 57600, 115200)" << std::endl;
	std::cout << "\t arg3: Data bits (7,8), Parity(N,O,E) and Bit stop(1,2)" << std::endl;
	std::cout << std::endl;
	std::cout << "Example: " << nameApp << " /dev/ttyS0 9600 8N1" << std::endl;
}

/**************************************************************************************************
 ******************************************* FUNCTION *********************************************
 *************************************************************************************************/
void printVersion(void)
{
	/** Print version */
	std::cout << ":::::::::::: TEST SERIAL PORT: " << VERSION << " ::::::::::::" << std::endl;
}

/**************************************************************************************************
 ******************************************* FUNCTION *********************************************
 *************************************************************************************************/
Parity_e parseParity(char parity)
{
	Parity_e eParity;

	if (parity == 'N') {
		eParity = eParity_None;
	}
	else if (parity == 'O') {
		eParity = eParity_Odd;
	}
	else {
		eParity = eParity_Even;
	}

	return eParity;
}
