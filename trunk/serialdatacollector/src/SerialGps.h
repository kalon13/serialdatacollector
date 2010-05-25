/*
 * SerialGps.h
 *
 *  Created on: 12/mar/2010
 *      Author: luca
 */

#ifndef SERIALGPS_H_
#define SERIALGPS_H_
#include "SerialDevice.h"
//#include <fstream>

/*typedef struct {
		 double utc;
		 unsigned char status;
		 double latitude;
		 unsigned char latitude_hemisphere;
		 double longitude;
		 unsigned char longitude_hemisphere;
		 double speed;
		 double course;
		 unsigned int utc_date;
		 double magnetic_variation;
		 unsigned char magnetic_variation_direction;
		 unsigned char mode_indicator;
		 //short unsigned int CHECKSUM;
		 }NMEA_GPRMC;

typedef struct {
		 double utc;
		 double latitude;
		 unsigned char latitude_hemisphere;
		 double longitude;
		 unsigned char longitude_hemisphere;
		 int position_fix;
		 int number_of_satellites;
		 double horizontal_dilution_precision;
		 double antenna_height;
		 unsigned char units_height;
		 double geoidal_height;
		 unsigned char units_separation;
		 double time_last_update;
		 int dgps_id;
		 short unsigned int CHECKSUM;
		 } NMEA_GPGGA;*/

//enum NMEASTRING {GPGGA=1, GPRMC=2, GPGSA=4, GPGSV=8, SCONOSCIUTO=0};
#define NMEA_GPGGA	1
#define NMEA_GPRMC	2
#define NMEA_GPGSA	4
#define NMEA_GPGSV	8

class SerialGps:public SerialDevice {
private:
    bool CheckChecksum(unsigned char* packet);
	bool decode(unsigned char* sentence, int tipo);

	char* pathtofile;
    //void decode_GPRMC(unsigned char* sentence,NMEA_GPRMC* gprmc);
    //void decode_GPGGA(unsigned char* sentence,NMEA_GPRMC* gpgga);
public:
    /*void store_data(unsigned char* sentence,int byte);
    void print_SatNum(unsigned char* sentence, unsigned short int byte);
    void print_Long(unsigned char* sentence, unsigned short int byte);
    void print_Lang(unsigned char* sentence, unsigned short int byte);*/
    bool readData(char** str, int tipo);	//Se ci sono errori restituisce NULL altrimenti la stringa completa
    bool openCommunication(char* port, int baudRate, int dataBits, PARITY parity, int stopBits);
    bool closeCommunication();
    //bool openCommunication(char* port, int baudRate, int dataBits, int parity, int stopBits);
    //void getGPRMC(NMEA_GPRMC&);
    //void getGPGGA(NMEA_GPGGA&);

    SerialGps();
	virtual ~SerialGps();
};

#endif /* SERIALGPS_H_ */
