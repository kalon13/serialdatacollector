/*
 * SerialDevice.cpp
 *
 *  Created on: 12/mar/2010
 *      Author: studenti
 */

#include "SerialDevice.h"
#include <termios.h>	//Comunicazione seriale
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ioctl.h>

using namespace std;

SerialDevice::SerialDevice() {
    DEBUG = false;			/* Per far vedere o no le scritte di debug*/
    TIMEOUT = 50000;		/* time to wait for port to respond, in microseconds */
    MAXATTEMPTS = 200;    	/* maximum number of attempts to read characters */
    WAITCHARTIME = 1000;  	/* time to wait for a char to arrive. */
    portNum = 0;
    communicationOpened = false;
    errorExplained = new char[128];
}

SerialDevice::~SerialDevice() {
	//Cancella i puntatori
	/*delete(port);
    delete(errorExplained);*/
    //Chiude la comunicazione
	delete errorExplained;
    if(portNum>0)
    	close(portNum);
}

void SerialDevice::getError(char** er) {
    *er = errorExplained;
}

int SerialDevice::readData(unsigned char* data, int lengthExpected)
{
	if(!communicationOpened){
    	errorExplained = "La comunicazione con il dispositivo non e' aperta\n";
        return -1;
	}

	int n, bytesRead, attempts;
	char inchar;
	struct timeval timeout;
	fd_set readfs;    /* file descriptor set */

	/* select will wait for port to respond or timeout */
	timeout.tv_usec = TIMEOUT;  /* microseconds */
	timeout.tv_sec  = 0;        /* seconds */
	FD_ZERO(&readfs);
	FD_SET(portNum, &readfs);  /* set testing for portHandle */
	if (DEBUG) cout << "Waiting for port to respond\n";
	//portCount = select(maxPorts, &readfs, NULL, NULL, &timeout);  /* block until input becomes available */
	if (!FD_ISSET(portNum, &readfs)) {
		if (DEBUG) cout << " - timeout expired!\n" ;
		return -1;
	}
	if (DEBUG) cout << "Time remaining " << timeout.tv_usec/1000 << "ms.\n";

	/* Read data into the response buffer.
	 * until we get enough data or exceed the maximum
	 * number of attempts
	 */
	bytesRead = 0;
	attempts = 0;
	while (bytesRead < lengthExpected && attempts++ < MAXATTEMPTS) {
		n = read(portNum, &inchar, 1);
		if (DEBUG)
			cout << n << inchar;
		if (n == 1)
			data[bytesRead++] = inchar;
		else
			sleep(WAITCHARTIME);  /* sleep a while for next byte. */
	}
	if (DEBUG) cout << "\nattempts" << attempts;
	if (DEBUG) cout << "\nreceiveData: bytes read: " << bytesRead << "\texpected: " << lengthExpected << endl;

	if (bytesRead != lengthExpected) {
		errorExplained = "Risposta di lunghezza non aspettata\n";
		return -1;
	}
	else
		return bytesRead;
}

int SerialDevice::sendData(unsigned char* data, int dataLength)
{
	if(!communicationOpened){
    	errorExplained = "La comunicazione con il dispositivo non e' aperta\n";
        return -1;
	}

    int bytesWritten;

    /* write data to the serial port */
    bytesWritten = write(portNum, &data[0], dataLength);
    if (bytesWritten >=0)
        return bytesWritten;
    else
    {
    	errorExplained = "Errore nell'invio di dati al dispositivo\n";
        return -1;
    }
}

bool SerialDevice::openCommunication(char* port, int baudRate, int dataBits, PARITY parity, int stopBits) {

    /*int portHandle;
    int status;

    portHandle = open(port, O_RDWR);

    if(portHandle<0) {
        errorExplained = "Errore nell'apertura del dispositivo\n";
        return false;
    }
    this->port = port;
    this->portNum = portHandle;*/

    int portHandle;

    portHandle = open(port, O_RDWR);

    if(portHandle<0) {
        errorExplained = "Errore nell'apertura del dispositivo!\n";
        return false;
    }

    this->port = port;
    this->portNum = portHandle;

    tcflush(portNum, TCIOFLUSH);

    /*TODO: Maledire
     * Maledire queste 2 righe
     */
    int n = fcntl(portNum, F_GETFL, 0);
    fcntl(portNum, F_SETFL, n & ~O_NDELAY);

    if (tcgetattr(portNum, &oldtio)!=0) {
       errorExplained = "tcgetattr() 2 failed";
       return false;
    }

    if(!setPortParameters(baudRate, dataBits, parity, stopBits))
    	return false;

    communicationOpened = true;
    return true;
}


bool SerialDevice::setPortParameters(int baudRate, int dataBits, PARITY parity, int stopBits) {
	struct termios newtio;
	if (tcgetattr(portNum, &newtio)!=0)	{
	  errorExplained  = "tcgetattr() 3 failed";
	  return false;
	}

	speed_t _baud=0;
	switch (baudRate) {
		#ifdef B0
		   case      0: _baud=B0;     break;
		#endif
		#ifdef B50
		   case     50: _baud=B50;    break;
		#endif
		#ifdef B75
		   case     75: _baud=B75;    break;
		#endif
		#ifdef B110
		   case    110: _baud=B110;   break;
		#endif
		#ifdef B134
		   case    134: _baud=B134;   break;
		#endif
		#ifdef B150
		   case    150: _baud=B150;   break;
		#endif
		#ifdef B200
		   case    200: _baud=B200;   break;
		#endif
		#ifdef B300
		   case    300: _baud=B300;   break;
		#endif
		#ifdef B600
		   case    600: _baud=B600;   break;
		#endif
		#ifdef B1200
		   case   1200: _baud=B1200;  break;
		#endif
		#ifdef B1800
		   case   1800: _baud=B1800;  break;
		#endif
		#ifdef B2400
		   case   2400: _baud=B2400;  break;
		#endif
		#ifdef B4800
		   case   4800: _baud=B4800;  break;
		#endif
		#ifdef B7200
		   case   7200: _baud=B7200;  break;
		#endif
		#ifdef B9600
		   case   9600: _baud=B9600;  break;
		#endif
		#ifdef B14400
		   case  14400: _baud=B14400; break;
		#endif
		#ifdef B19200
		   case  19200: _baud=B19200; break;
		#endif
		#ifdef B28800
		   case  28800: _baud=B28800; break;
		#endif
		#ifdef B38400
		   case  38400: _baud=B38400; break;
		#endif
		#ifdef B57600
		   case  57600: _baud=B57600; break;
		#endif
		#ifdef B76800
		   case  76800: _baud=B76800; break;
		#endif
		#ifdef B115200
		   case 115200: _baud=B115200; break;
		#endif
		#ifdef B128000
		   case 128000: _baud=B128000; break;
		#endif
		#ifdef B230400
		   case 230400: _baud=B230400; break;
		#endif
		#ifdef B460800
		   case 460800: _baud=B460800; break;
		#endif
		#ifdef B576000
		   case 576000: _baud=B576000; break;
		#endif
		#ifdef B921600
		   case 921600: _baud=B921600; break;
		#endif
	   default:		_baud=B4800;	break;
	}
	cfsetospeed(&newtio, (speed_t)_baud);
	cfsetispeed(&newtio, (speed_t)_baud);

	/* We generate mark and space parity ourself. */
	if (dataBits == 7 && (parity==MARK || parity == SPACE))
	  dataBits = 8;
	switch (dataBits)	{
	case 5:
	  newtio.c_cflag = (newtio.c_cflag & ~CSIZE) | CS5;
	  break;
	case 6:
	  newtio.c_cflag = (newtio.c_cflag & ~CSIZE) | CS6;
	  break;
	case 7:
	  newtio.c_cflag = (newtio.c_cflag & ~CSIZE) | CS7;
	  break;
	case 8:
	default:
	  newtio.c_cflag = (newtio.c_cflag & ~CSIZE) | CS8;
	  break;
	}

	newtio.c_cflag |= CLOCAL | CREAD;

	//parity
	newtio.c_cflag &= ~(PARENB | PARODD);
	if (parity == EVEN)
	  newtio.c_cflag |= PARENB;
	else if (parity == ODD)
	  newtio.c_cflag |= (PARENB | PARODD);

	//hardware handshake
	newtio.c_cflag &= ~CRTSCTS;

	//stopbits
	if (stopBits==2)
	  newtio.c_cflag |= CSTOPB;
	else
	  newtio.c_cflag &= ~CSTOPB;

	newtio.c_iflag=IGNBRK;

	//software handshake
	/*if (softwareHandshake)
	  newtio.c_iflag |= IXON | IXOFF;
	else*/
	  newtio.c_iflag &= ~(IXON|IXOFF|IXANY);

	newtio.c_lflag=0;
	newtio.c_oflag=0;

	newtio.c_cc[VTIME]=1;
	newtio.c_cc[VMIN]=60;

	//   tcflush(m_fd, TCIFLUSH);
	if (tcsetattr(portNum, TCSANOW, &newtio)!=0)	{
	  errorExplained = "tcsetattr() 1 failed";
	  return false;
	}

	int mcs=0;
	ioctl(portNum, TIOCMGET, &mcs);
	mcs |= TIOCM_RTS;
	ioctl(portNum, TIOCMSET, &mcs);

	if (tcgetattr(portNum, &newtio)!=0) {
	  errorExplained = "tcgetattr() 4 failed";
	  return false;
	}

	//hardware handshake
	/*if (hardwareHandshake)
	  newtio.c_cflag |= CRTSCTS;
	else*/
	  newtio.c_cflag &= ~CRTSCTS;

	if (tcsetattr(portNum, TCSANOW, &newtio)!=0) {
	  errorExplained = "tcsetattr() 2 failed";
	  return false;
	}
	return true;
}

/*
 * TODO: Non serve più
 * Ma teniamolo in caso...
 */

bool SerialDevice::setPortParameters2(int baudRate, int dataBits, PARITY parity, int stopBits) {

    struct termios portOptions;
    int BAUD, DATABITS, PARITY, PARITYON, STOPBITS;

    int status;

    /*  get port options for speed, etc. */
    tcgetattr(portNum, &portOptions);
    tcflush(portNum, TCIFLUSH);

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
    status = tcsetattr(portNum, TCSANOW, &portOptions);

    if (status != 0) {
        errorExplained = "Errore nell'impostazioni dei parametri della porta\n";
        return false;  //FALLITO
    }

    return true;
}

bool SerialDevice::tryOpenCommunication(char* port) {
    int portHandle;

    portHandle = open(port, O_RDONLY);

    if(portHandle<0)
        return false;

    close(portHandle);
    return true;
}

void SerialDevice::closeCommunication() {
    //Chiude la comunicazione
    if(portNum)
    	close(portNum);
}
bool SerialDevice::isConnected() {
	return communicationOpened;
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
