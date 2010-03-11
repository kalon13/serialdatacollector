/* 
 * File:   serialDevice.h
 * Author: studenti
 *
 * Created on 11 marzo 2010, 14.25
 */

#ifndef _SERIALDEVICE_H
#define	_SERIALDEVICE_H

#include <stdlib.h>

class serialDevice {
protected:
    char* port = NULL;      //Nome della porta(es /dev/ttyS0)
    char* errorExplained = NULL;     //Stringa che conterrà l'ultimo errore dato
    int openPort(char* port, int bauRate, int dataBits, int parity, int stopBits);
public:
    serialDevice();
    int readData(char*);
    int sendData(char*);
    getError(char* error);
    serialDevice(const serialDevice& orig);
    virtual ~serialDevice();
private:

};

#endif	/* _SERIALDEVICE_H */

