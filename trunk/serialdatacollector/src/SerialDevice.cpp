/*
 * SerialDevice.cpp
 *
 *  Created on: 12/mar/2010
 *      Author: studenti
 */

#include "SerialDevice.h"
#include <termios.h>
#include <fcntl.h>
#include <stdlib.h>

SerialDevice::SerialDevice() {
	// TODO Auto-generated constructor stub

}

SerialDevice::~SerialDevice() {
	// TODO Auto-generated destructor stub
}

void SerialDevice::getError(char* error) {
    error = errorExplained;
}

int SerialDevice::openPort(char* port, int baudRate, int dataBits, int parity, int stopBits){

    struct termios portOptions;
    int portHandle;
    int status;
    int BAUD, DATABITS, PARITY, PARITYON, STOPBITS;

    portHandle = open(port, O_RDWR);

    if(portHandle<0) {
        errorExplained = "Errore nell'apertura del dispositivo";
        return -1;
    }

    /*  get port options for speed, etc. */
    tcgetattr(portHandle, &portOptions);
    tcflush(portHandle, TCIFLUSH);

    /* baudrate settings */
    switch (baudRate) {
    case 38400:
        BAUD = B38400;
        break;
    case 19200:
        BAUD  = B19200;
        break;
    case 9600:
        BAUD  = B9600;
        break;
    default:
        BAUD = B4800;
    }

    /* databits settings */
    switch (dataBits) {
    case 8:
        DATABITS = CS8;
        break;
    case 7:
        DATABITS = CS7;
        break;
    default:
        DATABITS = CS8;
    }

    /* stop bits */
    switch (stopBits) {
    case 1:
        STOPBITS = 0;
        break;
    case 2:
        STOPBITS = CSTOPB;
        break;
    default:
        STOPBITS = 0;

    }

    /* parity */
    switch (parity) {
    case 0: /* no parity */
        PARITYON = 0;
        PARITY = 0;
        break;
    case 1: /* odd parity */
        PARITYON = PARENB;
        PARITY = PARODD;
        break;
    case 2: /* event parity */
        PARITYON = PARENB;
        PARITY = 0;
        break;
    default: /* none */
        PARITYON = 0;
        PARITY = 0;
    }

    /* We are not setting CRTSCTS which turns on hardware flow control */
    portOptions.c_cflag = BAUD | DATABITS | STOPBITS | PARITYON | PARITY | CLOCAL | CREAD;

    /*  set for non-canonical (raw processing, no echo, etc.) */
    portOptions.c_iflag = IGNPAR; /* ignore parity check */
    portOptions.c_oflag = 0; /* raw output */
    portOptions.c_lflag = 0; /* raw input  */

    cfsetospeed(&portOptions, BAUD);  /* redundant with the cflag setting, above */
    cfsetispeed(&portOptions, BAUD);
    status = tcsetattr(portHandle, TCSANOW, &portOptions);

    if (status != 0)
    {
        errorExplained = "Errore nell'impostazioni dei parametri della porta";
        return -1;  //FALLITO
    }
    return status;  //OK
}
