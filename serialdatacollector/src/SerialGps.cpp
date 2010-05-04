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
	unsigned char p[4096];
	char risp='n';
	do{
		SerialDevice::openCommunication(port,baudRate,dataBits,parity,stopBits);
		if(SerialDevice::readData(&p[0],4096)>0) {
			//for(int i=0; i<4096; ++i)
				//cout << p[i];
			return true;
		}
		closeCommunication();
		cout << errorExplained << endl;
		if(risp!='a') {
			cout << "Ritento?(s\\N\a)";
			cin >> risp;
		}
	}
	while(risp=='s' || risp=='a');
	return false;
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

bool SerialGps::readData(char** str, NMEASTRING tipo){
	bool find = false;
	unsigned char data[256];
	int length;
	int i=0,j=0, count=0;

	do {
		char* sentence = new char[84];
		length = SerialDevice::readData(&data[0],256);

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


