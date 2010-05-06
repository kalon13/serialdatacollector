/*
 * SerialDevice.h
 *
 *  Created on: 12/mar/2010
 *      Author: studenti
 */

#ifndef SERIALDEVICE_H_
#define SERIALDEVICE_H_

#include <stdlib.h>
#include <termios.h>

enum PARITY {NONE=0, EVEN=1, ODD=2, MARK=3, SPACE=4};

class SerialDevice {
protected:
	char* port;      			/* Nome della porta(es /dev/ttyS0)*/
    char* errorExplained;		/* Stringa che conterrà l'ultimo errore dato*/
    int portNum;				/* Valore intero che verrà associtato alla porta aperta*/
    struct termios oldtio;

    bool DEBUG;					/* Per far vedere o no le scritte di debug*/
    int TIMEOUT;				/* time to wait for port to respond, in microseconds */
    int MAXATTEMPTS;    		/* maximum number of attempts to read characters */
    int WAITCHARTIME;  			/* time to wait for a char to arrive. */
    bool communicationOpened;

    bool setPortParameters(int baudRate, int dataBits, PARITY parity, int stopBits);
    bool setPortParameters2(int baudRate, int dataBits, PARITY parity, int stopBits);

public:
    void getError(char** er);
    virtual int readData(unsigned char* data, int lengthExpected);
    virtual int sendData(unsigned char* data, int dataLength);
    virtual bool openCommunication(char* port, int baudRate, int dataBits, PARITY parity, int stopBits);
    virtual bool closeCommunication();
    static bool tryOpenCommunication(char* port);
    bool isConnected();			/* Ritorna vero se la comunicazione è aperta altrimento falso */
    void setDebug(bool);

    /* Metodi che impostano i parametri per la read seriale. Se si fa l'ovverride di readData non servono più */
    void setTimeout(int);
    void setMaxAttempts(int);
    void setWaitCharTime(int);

	SerialDevice();
	virtual ~SerialDevice();
};

#endif /* SERIALDEVICE_H_ */
