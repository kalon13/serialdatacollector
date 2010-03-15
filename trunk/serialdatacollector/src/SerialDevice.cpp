/*
 * SerialDevice.cpp
 *
 *  Created on: 12/mar/2010
 *      Author: studenti
 */

#include "SerialDevice.h"
#include <termios.h>	//Comunicazione seriale
#include <fcntl.h>
#include <unistd.h>		//Per la sleep
#include <stdlib.h>
#include <stdio.h>
#include <iostream>		//Per la read

SerialDevice::SerialDevice() {
    DEBUG = false;			/* Per far vedere o no le scritte di debug*/
    TIMEOUT = 50000;		/* time to wait for port to respond, in microseconds */
    MAXATTEMPTS = 200;    	/* maximum number of attempts to read characters */
    WAITCHARTIME = 1000;  	/* time to wait for a char to arrive. */
    portNum = 0;
}

SerialDevice::~SerialDevice() {
	//Cancella i puntatori
	/*delete(port);
    delete(errorExplained);*/
    //Chiude la comunicazione
    if(portNum)
    	close(portNum);
}

void SerialDevice::getError(char* &error) {
    error = errorExplained;
}

int SerialDevice::readData(unsigned char* data, int lengthExpected)
{
		int n, bytesRead, attempts;
	    char inchar;
	    int maxPorts;
	    struct timeval timeout;
	    fd_set readfs;    /* file descriptor set */

	    /* select will wait for port to respond or timeout */
	    timeout.tv_usec = TIMEOUT;  /* microseconds */
	    timeout.tv_sec  = 0;        /* seconds */
	    FD_ZERO(&readfs);
	    FD_SET(portNum, &readfs);  /* set testing for portHandle */
	    if (DEBUG) printf("waiting for port to respond\n");
	    //portCount = select(maxPorts, &readfs, NULL, NULL, &timeout);  /* block until input becomes available */
	    if (!FD_ISSET(portNum, &readfs)) {
	        if (DEBUG) printf(" - timeout expired!\n");
	        return -1;
	    }
	    if (DEBUG) printf("time remaining %ld ms.\n", timeout.tv_usec/1000);

	    /* Read data into the response buffer.
	     * until we get enough data or exceed the maximum
	     * number of attempts
	     */
	    bytesRead = 0;
	    attempts = 0;
	    while (bytesRead < lengthExpected && attempts++ < MAXATTEMPTS) {
	        n = read(portNum, &inchar, 1);
	        if (DEBUG) printf(".", n, inchar);
	        if (n == 1)
	            data[bytesRead++] = inchar;
	        else
	            sleep(WAITCHARTIME);  /* sleep a while for next byte. */
	    }
	    if (DEBUG) printf("\nattempts %d", attempts);
	    if (DEBUG) printf("\nreceiveData: bytes read: %d   expected: %d\n", bytesRead, lengthExpected);

	    if (bytesRead != lengthExpected) {
	    	errorExplained = "Risposta di lunghezza non aspettata";
	        return -1;
	    }
	    else
	        return bytesRead;
}

int SerialDevice::sendData(unsigned char* data, int dataLength)
{
    int bytesWritten;

    /* write data to the serial port */
    bytesWritten = write(portNum, &data[0], dataLength);
    if (bytesWritten >=0)
        return bytesWritten;
    else
    {
    	errorExplained = "Errore nell'invio di dati al dispositivo";
        return -1;
    }
}

bool SerialDevice::openCommunication(char* port, int baudRate, int dataBits, int parity, int stopBits){

    struct termios portOptions;
    int portHandle;
    int status;
    int BAUD, DATABITS, PARITY, PARITYON, STOPBITS;

    this->port = port;
    portHandle = open(port, O_RDWR);

    if(portHandle<0) {
        errorExplained = "Errore nell'apertura del dispositivo";
        return false;
    }
    this->portNum = portHandle;

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
        return false;  //FALLITO
    }
    return true;  //OK
}

void SerialDevice::setDebug(bool val){
	DEBUG = val;
}

void SerialDevice::setTimeout(int val){
	TIMEOUT = val;
}
void SerialDevice::setMaxAttempts(int val){
	MAXATTEMPTS = val;
}
void SerialDevice::setWaitCharTime(int val){
	WAITCHARTIME = val;
}
