/*
 * SerialDevice.h
 *
 *  Created on: 12/mar/2010
 *      Author: studenti
 */

#ifndef SERIALDEVICE_H_
#define SERIALDEVICE_H_

#include <stdlib.h>

class SerialDevice {
protected:
	char* port;      		//Nome della porta(es /dev/ttyS0)
    char* errorExplained;	//Stringa che conterrà l'ultimo errore dato
    int openPort(char* port, int bauRate, int dataBits, int parity, int stopBits);
public:
    void getError(char* error);
    int readData(char*);
    int sendData(char*);
	SerialDevice();
	virtual ~SerialDevice();
};

#endif /* SERIALDEVICE_H_ */
