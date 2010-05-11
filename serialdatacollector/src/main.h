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

bool some_thread_active(bool conta_in_pausa = false);
string devKind(DevId n);
string thrState(Stato stato);

void* camAcquisition(void* i);
void* gpsAcquisition(void* i);
void* imuAcquisition(void* i);
void* hokAcquisition(void* i);

void StartThread(int start, int stop);
void PauseThread(int start, int stop);
void StopThread(int start, int stop);

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


#endif /* MAIN_H_ */
