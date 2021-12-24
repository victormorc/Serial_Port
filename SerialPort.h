//=================================================================================================
// Name        : SerialPort.h
// Author      : vmoreno
// Date		   : 24/12/2021
// Version     : 1.0
// Description : Serial port able to read data and transmit back.
//
//=================================================================================================

#ifndef SERIALPORT_H_
#define SERIALPORT_H_

/****************************************** DEFINES  *********************************************/

/**************************************************************************************************
 * \enum typeSignal_e
 * \brief Enum with several types for a signal.
 *************************************************************************************************/
typedef enum BaudRate {
	eBaudRate_9600 = 9600,
	eBaudRate_19200 = 19200,
	eBaudRate_38400 = 38400,
	eBaudRate_57600 = 57600,
	eBaudRate_115200 = 115200,
}BaudRate_e;

/**************************************************************************************************
 * \enum typeSignal_e
 * \brief Enum with several types for a signal.
 *************************************************************************************************/
typedef enum Parity {
	eParity_None,
	eParity_Odd,
	eParity_Even,
}Parity_e;

/**************************************************************************************************
 * \class SerialPort
 * \brief Class which represents a serial port.
 *************************************************************************************************/
class SerialPort {

public:

	/***************************************  FUNCTION  *******************************************
	 * Open and configure serial port with specify data.
	 * \param[in] namePort - name of port
	 * \param[in] baudRate - Baud rate to configure port
	 * \param[in] parity - Parity to configure port
	 * \param[in] bits - Number of bits to configure port
	 * \param[in] twoStopBits - Two stop bits to configure port (if true, 1 stop bit if false)
	 * \param[in, out] None.
	 * \param[out] None.
	 * \return -1 when there is an error, otherwise 0.
	 * \Comments Open port as NON_BLOCK.
	 *********************************************************************************************/
	int Open(const char *namePort, BaudRate_e baudRate, Parity_e parity, int bits, bool twoStopBits);

	/***************************************  FUNCTION  *******************************************
	 * Read data from serial port.
	 * \param[in] numBits - Number of bytes to read.
	 * \param[in, out] None.
	 * \param[out] pBuffer - Buffer to fill in with data read.
	 * \return -1 when there is an error, otherwise number of bytes read.
	 * \Comments No data is not consider as an error, method returns a 0 value.
	 *********************************************************************************************/
	int ReadData(void *pBuffer, int numBytes);

	/***************************************  FUNCTION  *******************************************
	 * Write data to serial port.
	 * \param[in] pBuffer - Buffer which contains data to output.
	 * \param[in] numBytes - Number of bytes to write.
	 * \param[in, out] None.
	 * \param[out] None.
	 * \return -1 when there is an error, otherwise number of bytes sent.
	 * \Comments None.
	 *********************************************************************************************/
	int WriteData(void *pBuffer, int numBytes);

	/***************************************  FUNCTION  *******************************************
	 * Close serial port.
	 * \param[in] None
	 * \param[in, out] None.
	 * \param[out] None.
	 * \return -1 when there is an error, otherwise 0.
	 * \Comments None.
	 *********************************************************************************************/
	int Close(void);

private:

	const char *_port; 		/**< Name of the serial port */
	int _fd;				/**< File descriptor of the serial port */
	BaudRate_e _baudRate;	/**< Baud rate of the serial port */
	Parity_e _parity;		/**< Parity of the serial port */
	int _bits;				/**< Number of bits in data in serial frame */
	bool _twoStopBit;		/**< Flag which states if there is two stop bits or only one */

};

#endif /* SERIALPORT_H_ */
