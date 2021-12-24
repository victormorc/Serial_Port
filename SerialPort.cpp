//=================================================================================================
// Name        : SerialPort.cpp
// Author      : vmoreno
// Date		   : 24/12/2021
// Version     : 1.0
// Description : Serial port able to read data and transmit back.
//
//=================================================================================================

/****************************************** INCLUDES *********************************************/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <string.h>
#include <iostream>
#include "SerialPort.h"

/**************************************************************************************************
 ******************************************* FUNCTION *********************************************
 *************************************************************************************************/
int SerialPort::Open(const char *namePort, BaudRate_e baudRate, Parity_e parity, int bits, bool twoStopBits)
{
	int retValue = -1;	/**< Error by default */

	_port = namePort;			/**< Copy name of serial port to open */
	_baudRate = baudRate;		/**< Copy baud rate */
	_parity = parity;			/**< Copy parity */
	_bits = bits;				/**< Copy number of bits in data */
	_twoStopBit = twoStopBits;	/**< Copy stop bits */

	struct termios stPortOpts;	/**< Configuration options of the serial port */

	_fd = open(namePort, O_RDWR | O_NONBLOCK);	/**< Open port to read/write an as NON_BLOCK */

	/** Check if port was opened successfully */
	if (_fd < 0) {
		return retValue;	/**< Error opening port */
	}

	/** Get configuration options */
	if (tcgetattr(_fd, &stPortOpts) < 0) {
		/** Error getting configure options */
		std::cout << ":::: ERROR GETTING SERIAL PORT CONFIGURATION  ::::" << std::endl;
		return retValue;
	}
	else { /**< Change configure options */
		/** Set port speed */
		int baudSpeed;
		switch(baudRate) {
			case (eBaudRate_9600):
				baudSpeed = B9600;
			break;
			case (eBaudRate_19200):
				baudSpeed = B19200;
			break;
			case (eBaudRate_38400):
				baudSpeed = B38400;
			break;
			case (eBaudRate_57600):
				baudSpeed = B57600;
			break;
			case (eBaudRate_115200):
				baudSpeed = B115200;
			break;
		}
		cfsetispeed(&stPortOpts, baudSpeed);
		cfsetospeed(&stPortOpts, baudSpeed);

		/** Enable the receiver and set local mode. */
		stPortOpts.c_cflag |= (CLOCAL | CREAD);

        /** Set stop bit */
		if (!twoStopBits) {
			stPortOpts.c_cflag &= ~CSTOPB;	/**< One stop bit */
		}
		else {
			stPortOpts.c_cflag |= CSTOPB;	/**< Two stop bits */
		}

        /** Configure parity */
		if (parity == eParity_Even) {
			stPortOpts.c_cflag |= PARENB; 	/**< Enable parity bit */
			stPortOpts.c_cflag &= ~PARODD; 	/**< Set even parity */
		}
		else if (parity == eParity_Odd) {
			stPortOpts.c_cflag |= PARENB; 	/**< Enable parity bit */
			stPortOpts.c_cflag |= PARODD; 	/**< Set odd parity */
		}
		else {
			stPortOpts.c_cflag &= ~PARENB;	/**< Disable parity bit */
		}


        /** Set data bits */
		stPortOpts.c_cflag &= ~CSIZE;	/**< Mask the character size bits */
		if (bits == 8) {
			stPortOpts.c_cflag |=  CS8;	/**< 8 data bits */
		}
		else if (bits == 7) {
			stPortOpts.c_cflag |=  CS7;	/**< 7 data bits */
		}

        /** Disable hardware flow control */
		stPortOpts.c_cflag &= ~CRTSCTS;

        /** Enable parity check */
		if (parity != eParity_None) {
			stPortOpts.c_iflag |= (INPCK | ISTRIP);		/**< Enable check and strip parity */
		}
		else {
			stPortOpts.c_iflag = 0;	/**< No software flow control */
		}

        /** Set input/output mode (non-canonical, no echo,...) */
		stPortOpts.c_lflag = 0;		/**< Raw input */
		stPortOpts.c_oflag = 0;		/**< Raw output */

        /** Set the port without timeout if no data arrives */
		stPortOpts.c_cc[VTIME] = 0; /**<  No timeout, return without blocking */
		stPortOpts.c_cc[VMIN]  = 0; /**<  Minimum number of characters to read */

        /** Flush port */
        tcflush(_fd, TCIFLUSH);
        tcflush(_fd, TCOFLUSH);

        /** Set new options */
        if(tcsetattr(_fd, TCSANOW, &stPortOpts) != 0)
        {
        	std::cout << "::::::: FAILED TO CONFIGURE PORT SETTINGS  :::::::" << std::endl;
            return retValue;
        }
	}

	return retValue = 0;	/**< No error */
}

/**************************************************************************************************
 ******************************************* FUNCTION *********************************************
 *************************************************************************************************/
int SerialPort::ReadData(void *pBuffer, int numBytes)
{
	int retValue = -1;	/**< Error by default */
	int bytesReceived;	/**< Bytes received */

	/** Read serial port */
	if ((bytesReceived = read(_fd, pBuffer, numBytes)) < 0) {
		/** Error reading data */
		if (errno == EAGAIN) {
			return retValue = 0;	/**< No data available at this time */
		}
		return retValue;	/**< Error return -1 */
	}

	/** If no problem reading port, return number of bytes received */
	return retValue = bytesReceived;
}

/**************************************************************************************************
 ******************************************* FUNCTION *********************************************
 *************************************************************************************************/
int SerialPort::WriteData(void *pBuffer, int numBytes)
{
	int retValue = -1;	/**< Error by default */
	int bytesSent;		/**< Bytes sent */

	/** Write serial port */
	if ((bytesSent = write(_fd, pBuffer, numBytes)) < 0) {
		/** Error writing data */
		return retValue;
	}

	/** If no problem writing port, return number of bytes written */
	return retValue = bytesSent;
}

/**************************************************************************************************
 ******************************************* FUNCTION *********************************************
 *************************************************************************************************/
int SerialPort::Close(void)
{
	int retValue = -1;	/**< Error by default */

	if (close(_fd) < 0) {
		/** Error closing file descriptor */
		return retValue;
	}

	return retValue = 0;
}


