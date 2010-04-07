/*
 * main.h
 *
 *  Created on: 18/mar/2010
 *      Author: studenti
 */

#include "RawSeed.h"
#include "SerialDevice.h"
#include "Camera.h"
#include <boost/thread.hpp>
#include <vector>

#define MAX_SENSOR 8

#ifndef MAIN_H_
#define MAIN_H_

#define DIM_BUFFER_GPS 1
#define DIM_BUFFER_IMU 32

enum DevId {NOTHING=9, GPS=0, IMU=1, CAM=2};
enum Stato {PRONTO, ATTIVO, PAUSA, TERMINATO};

/*TODO: Miglioramento
 * Eventualmente trasformare l'array dei thread in un vector e creare una struttura che riunisce
 * threadeddevice e il thread
 */
struct ThreadedDevice {
	DevId identifier;
	void* device;
	char* path;
	Stato stato;
	int pid_t;
	short debug;	//0=Nessuna scritta; //1=Poche scritte; //2=Tutto
};

vector<ThreadedDevice> d;
int num_disp;
pthread_t thr[MAX_SENSOR];
int pidt[MAX_SENSOR];
RawSeed* dataset;

void* camAcquisition(void* i);
void* gpsAcquisition(void* i);
void* imuAcquisition(void* i);

typedef vector<string> svec;

void* Shell();
void cmdStart(svec arg);
void cmdStop(svec arg);
void cmdPause(svec arg);
void cmdShow(svec arg);
void cmdDebug(svec arg);
bool cmdQuit();
void cmdHelp(svec arg);
void cmdInsert(svec arg);

bool some_thread_active();
string devKind(DevId n);
string thrState(Stato stato);


#endif /* MAIN_H_ */
