/*
 * SerialGps.cpp
 *
 *  Created on: 12/mar/2010
 *      Author: luca
 */

#include "SerialGps.h"
#include <iostream>
//#include <stdlib.h>
#include <cstring>
//#include <unistd.h>
//#include <fcntl.h>
//#include <sys/types.h>
//#include <time.h>
//#include <termios.h>
#include "SerialDevice.h"

using namespace std;

SerialGps::SerialGps() {
	// TODO Auto-generated constructor stub
	SerialDevice::SerialDevice();
}

SerialGps::~SerialGps() {
	// TODO Auto-generated destructor stub
}
/*
bool SerialGps::openCommunication(char* port, int baudRate, int dataBits, int parity, int stopBits) {
	return SerialDevice::openCommunication(port,baudRate,dataBits,parity,stopBits);
}*/

bool SerialGps::openCommunication(char* port) {
	int baudRate=4800, dataBits = 8, parity = 0, stopBits = 1;
	return SerialDevice::openCommunication(port,baudRate,dataBits,parity,stopBits);
}

// questo metodo ritona numero da 0 a 4 che identifica che tipologia di stringa
// NMEA è stata letta
unsigned short int SerialGps::decode(unsigned char *sentence)
{
	 if(strncmp((const char *)sentence,"$GPRMC",6) == 0)
		 return 1;
	 if(strncmp((const char *)sentence,"$GPGGA",6) == 0)
		 return 2;
	 if(strncmp((const char *)sentence,"$GPGSA",6) == 0)
		 return 3;
	 if(strncmp((const char *)sentence,"$GPGSV",6) == 0)
		 return 4;
	 else
		 return 0;

}
/*
void decode_GPRMC(unsigned char *sentence,this.NMEA_GPRMC *gprmc)
{
	sscanf((const char*)sentence+7, "%lf,%c,%lf,%c,%lf,%c,%lf,%lf,%d,%lf,%c,%c",
	    (&gprmc->utc),(&gprmc->status),(&gprmc->latitude),(&gprmc->latitude_hemisphere),(&gprmc->longitude),(&gprmc->longitude_hemisphere),
	    (&gprmc->speed),(&gprmc->course),(&gprmc->utc_date),(&gprmc->magnetic_variation),(&gprmc->magnetic_variation_direction),
	    (&gprmc->mode_indicator));

}
void decode_GPGGA(unsigned char *sentence,NMEA_GPRMC *gpgga)
{
	sscanf((const char*)sentence+7, "%lf,%lf,%c,%lf,%c,%d,%d,%lf,%lf,%c,%lf,%c,%lf,%d",
	&gpgga->utc,&gpgga->latitude,&gpgga->latitude_hemisphere,&gpgga->longitude,&gpgga->longitude_hemisphere,
	&gpgga->position_fix,&gpgga->number_of_satellites,&gpgga->horizontal_dilution_precision,&gpgga->antenna_height,
	&gpgga->units_height,&gpgga->geoidal_height,&gpgga->units_separation,&gpgga->time_last_update,&gpgga->dgps_id);

}

void SerialGps::store_data(unsigned char *data,int byte)
{
FILE *gprmc;
FILE *gpgga;
unsigned int byte_read,i,j,count,type_sentence,checksum;
unsigned char sentence[2048];


i=0;
j=0;
count=0;

	if(byte> 0)
	{
	  //lunghezza
		 for(i=0; i<byte; i++)
		  {
		      if(data[i] == (unsigned char)'$')
		           {
		              count++;
		           }
		           else
		           {
		              count = 0;
		           }

		           if(count == 1)
		           {
		              j = 1;
		              sentence[0] = '$';

		           }

		           if(j > 0 && data[i]!=(unsigned char)'$')
		           {
		              sentence[j] = data[i];
		              j++;
		           }

		           if(data[i] == '\n' && j > 0)
		           {
		              sentence[j] = '\0';
		              j = -1;
		           }
	  }
	  type_sentence = decode(sentence);

	  switch(type_sentence)
	  {
	  case 1:
		 gprmc = fopen ("gprmc.cvs", "a");
		  //stampo sul file gprmc.csv i dati GPRMC
		  if(CheckChecksum(sentence))
			  printf("CHECKSUM CORRETTO");
		  else
			  printf("CHECKSUM SBAGLIATO");
		 printf("%s",sentence);
		  fprintf(gprmc,(const char*)sentence);
		  fclose(gprmc);
	  break;
	  case 2:
		  if(CheckChecksum(sentence))
			  printf("CHECKSUM CORRETTO");
		  else
			  printf("CHECKSUM SBAGLIATO");
		  gpgga = fopen ("gpgga.cvs", "a");
		  //stampo sul file gpgga.csv i dati GPGGA
		  printf("%s",sentence);
		  fprintf(gpgga,(const char*)sentence);
		  fclose(gpgga);
		  break;
	  default:
		  break;
	  }

	}
	else
	{
		//return error
	}
}*/

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

bool SerialGps::getGPGGAString(char* str){
	bool find = false;
	unsigned char* data;
	int length;

	do {
		length = SerialDevice::readData(&data[0],84);
		if(decode(data)==2){
			if(CheckChecksum(data))
				find = true;
		}
	}
	while(!find && length>0);

	str = (char*)data;
	return true;
}


