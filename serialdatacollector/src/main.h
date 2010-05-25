/*
 * main.h
 *
 *  Created on: 18/mar/2010
 *      Author: studenti
 */
#ifndef MAIN_H_
#define MAIN_H_

#include <vector>
#include "RawSeed.h"
#include "SerialDevice.h"

#define MAX_SENSOR 8
#define DIM_BUFFER_GPS 1
#define DIM_BUFFER_IMU 32
#define DIM_BUFFER_HOKUYO 8

using namespace std;

typedef vector<string> svec;
enum DevId {NOTHING=9, GPS=0, IMU=1, CAM=2, HOK=3};
enum Stato {PRONTO, ATTIVO, PAUSA, TERMINATO};

struct ThreadedDevice {
	DevId identifier;
	void* device;
	char* path;
	Stato stato;
	pthread_t pid;
	short debug;	//0=Nessuna scritta; //1=Poche scritte; //2=Tutto
};

RawSeed* dataset;
int num_disp;
vector<ThreadedDevice> d;

void* camAcquisition(void* i);
void* gpsAcquisition(void* i);
void* imuAcquisition(void* i);
void* hokAcquisition(void* i);

void PlayThread(svec arg, int command);	//0=Play, 1=Pause, 2=Stop
bool InsertGPS(char* percorso_porta="/dev/ttyACM0", char* filename="/GPS.csv", int baudRate=38400, int dataBits=8, PARITY parity=NONE, int stopBits=1);
bool InsertIMU(char* percorso_porta="/dev/ttyUSB0", char* filename="/IMU_STRETCHED.csv", int baudRate=38400, int dataBits=8, PARITY parity=NONE, int stopBits=1);
bool InsertCAM(char* percorso_porta="http://192.168.10.100/mjpg/video.mjpg", int attesa=200);
bool InsertCAM(int numero_porta, int attesa=200);
/*#ifndef HOKUYO
	bool InsertHOK(int numero_porta=0, char* filename="/HOKUYO.csv");
#endif*/

void* Shell();
void cmdStart(svec arg);
void cmdStop(svec arg);
void cmdPause(svec arg);
void cmdShow(svec arg);
void cmdDebug(svec arg);
bool cmdQuit();
void cmdHelp(svec arg);
void cmdInsert(svec arg);
void cmdCalibration();

bool some_thread_active(bool conta_in_pausa = false);
string devKind(DevId n);
string thrState(Stato stato);

#endif /* MAIN_H_ */
