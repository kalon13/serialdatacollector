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
	char* port;      			/* Nome della porta(es /dev/ttyS0)*/
    char* errorExplained;		/* Stringa che conterrà l'ultimo errore dato*/
    int portNum;				/* Valore intero che verrà associtato alla porta aperta*/

    bool DEBUG;					/* Per far vedere o no le scritte di debug*/
    int TIMEOUT;				/* time to wait for port to respond, in microseconds */
    int MAXATTEMPTS;    		/* maximum number of attempts to read characters */
    int WAITCHARTIME;  			/* time to wait for a char to arrive. */

public:
    void getError(char* &error);
    int readData(unsigned char* data, int lengthExpected);
    int sendData(unsigned char* data, int dataLength);
    bool openCommunication(char* port, int bauRate, int dataBits, int parity, int stopBits);

    //Metodi che impostano i parametri
    void setDebug(bool);
    void setTimeout(int);
    void setMaxAttempts(int);
    void setWaitCharTime(int);

	SerialDevice();
	virtual ~SerialDevice();
};

#endif /* SERIALDEVICE_H_ */
