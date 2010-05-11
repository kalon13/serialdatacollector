/*
 * SerialGps.cpp
 *
 *  Created on: 12/mar/2010
 *      Author: luca
 */

#include "SerialGps.h"
#include <iostream>
#include <stdlib.h>
#include <cstring>
//#include <unistd.h>
//#include <fcntl.h>
//#include <sys/types.h>
//#include <time.h>
//#include <termios.h>

using namespace std;

SerialGps::SerialGps() {
	SerialDevice();
}

SerialGps::~SerialGps() {
	SerialDevice::closeCommunication();
}


bool SerialGps::openCommunication(char* port, int baudRate, int dataBits, PARITY parity, int stopBits) {
	return SerialDevice::openCommunication(port,baudRate,dataBits,parity,stopBits);
}

// questo metodo ritona numero da 0 a 4 che identifica che tipologia di stringa
// NMEA è stata letta
NMEASTRING SerialGps::decode(unsigned char* sentence)
{
		if(strncmp((const char *)sentence,"$GPRMC",6) == 0)
			return GPRMC;
		if(strncmp((const char *)sentence,"$GPGGA",6) == 0)
			return GPGGA;
		if(strncmp((const char *)sentence,"$GPGSA",6) == 0)
			return GPGSA;
		if(strncmp((const char *)sentence,"$GPGSV",6) == 0)
			return GPGSV;
		else
			return SCONOSCIUTO;
}

bool SerialGps::CheckChecksum(unsigned char* packet)
{
  unsigned char Character;
  unsigned short Checksum = 0, ChecksumArrivato=0;
  int i=0 ,length ;

  length = strlen((const char*)packet);
  for(i=0;i<length;++i)
  {
    Character = packet[i];

    if (Character != (unsigned char)'$')
    {
    	if(Character != (unsigned char)'*'){
    		if(Checksum==0)
    			Checksum=Character;
    		else
    			Checksum ^= (char)Character;
			//printf("%c ",Character);
    	}
    	else
    	{
    		ChecksumArrivato = (char)packet[i+1]-48;
    		if(ChecksumArrivato>10)
    			ChecksumArrivato -= 7;
    		ChecksumArrivato *= 16;
    		int bit = (char)packet[i+2]-48;
    		if(bit>10)
    			bit -= 7;
    		ChecksumArrivato += bit;

    		i = length;
    	}
    }

  }
  if(DEBUG) cout << "\nCHECKSUM: " << Checksum << "\nCHECKSUM ASPETTATO: " << ChecksumArrivato;
  if(ChecksumArrivato==Checksum)
	  return true;
  else
	  return false;
}

bool SerialGps::readData(char** str, NMEASTRING tipo){
	bool find = false;
	unsigned char data[256];
	int length;
	int i=0,j=0, count=0;

	do {
		char* sentence = new char[128];
		length = SerialDevice::readData(&data[0],255);

		for(i=0; i<length; i++){
			if(data[i] == (unsigned char)'$')
				count++;
			else
				count = 0;

			if(count == 1) {
				j = 1;
				sentence[0] = '$';
			}

			if(j > 0 && data[i]!=(unsigned char)'$'){
				sentence[j] = data[i];
				j++;
			}

			if(data[i] == '\n' && j > 0) {
				sentence[j-1] = '\0';
				j = -1;
			}
		}


		if(decode((unsigned char*)sentence)==tipo) {
			//if(CheckChecksum((unsigned char*) sentence)) {
				find = true;
				*str = sentence;
			//}
		}
	}
	while(!find);
	return true;
}

bool SerialGps::closeCommunication(){
	SerialDevice::closeCommunication();
}


